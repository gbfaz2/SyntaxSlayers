#include "IAArena.h" // INCLUYE CABECERA
#include <cmath> // PARA sqrt

void IAArena::actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt)
{
    float distancia = calcularDistancia(enemigo, jugador); // DISTANCIA ACTUAL ENTRE LOS DOS (TEOREMA DE PITÁGORAS)

    actualizarEstado(enemigo, jugador, distancia); // DECIDE EN QUE ESTADO ESTAR

    switch (_estado)
    {
    case Estado::PATRULLAR: accionPatrullar(inputIA, dt);break; // EJECUTA PATRULLA
    case Estado::PERSEGUIR: accionPerseguir(enemigo, jugador, inputIA);break; // EJECUTA PERSECUCION
    case Estado::ATACAR: accionAtacar(enemigo, jugador, inputIA, dt); break; // EJECUTA ATAQUE
    case Estado::HUIR:      accionHuir(enemigo, jugador, inputIA);break; // EJECUTA HUIDA
    }
}

float IAArena::calcularDistancia(const Combatiente& a, const Combatiente& b) const
{
    float dx = a.x() - b.x(); // DIFERENCIA EN X
    float dz = a.z() - b.z(); // DIFERENCIA EN Z
    return std::sqrt(dx * dx + dz * dz); // DISTANCIA EUCLIDEA (TEOREMA DE PITÁGORAS)
}

void IAArena::configurar(float alcanceAtaque) { // INICIA CONFIGURACION DISTANCIAS
    _distanciaAtaque = alcanceAtaque + 0.4f; // ASIGNA RANGO ATAQUE
    _distanciaPerseguir = alcanceAtaque + 3.0f; // PERSIGUE DESDE MAS LEJOS SI TIENE MAS ALCANCE
}

void IAArena::actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia)
{
    float vidaFraccion = enemigo.vida() / enemigo.vidaMax(); // PORCENTAJE DE VIDA RESTANTE

    if (vidaFraccion < _umbralHuida) // VIDA MUY BAJA
    {
        _estado = Estado::HUIR; // CAMBIA A HUIR
    }
    else if (distancia <= _distanciaAtaque) // EN RANGO DE ATAQUE
    {
        _estado = Estado::ATACAR; // CAMBIA A ATACAR
    }
    else if (distancia <= _distanciaPerseguir) // EN RANGO DE PERSECUCION
    {
        _estado = Estado::PERSEGUIR; // CAMBIA A PERSEGUIR
    }
    else // DEMASIADO LEJOS
    {
        _estado = Estado::PATRULLAR; // CAMBIA A PATRULLAR
    }
}

void IAArena::accionPatrullar(EstadoJugador& inputIA, float dt)
{
    _tiempoPatrulla += dt; // ACUMULA TIEMPO

    if (_tiempoPatrulla >= _duracionPatrulla) // TIEMPO CUMPLIDO
    {
        _patrullaArriba = !_patrullaArriba; // CAMBIA DIRECCION
        _tiempoPatrulla = 0.0f;             // RESETEA TIMER
    }

    inputIA.delante = _patrullaArriba;  // SUBE O BAJA SEGUN DIRECCION
    inputIA.atras = !_patrullaArriba;   // INVIERTE DIRECCION
    inputIA.izquierda = false;          // ANULA IZQUIERDA
    inputIA.derecha = false;            // ANULA DERECHA
    inputIA.atacar = false;             // ANULA ATAQUE
}

void IAArena::accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA)
{
    inputIA.delante = false;   // INICIALIZA DELANTE
    inputIA.atras = false;     // INICIALIZA ATRAS
    inputIA.izquierda = false; // INICIALIZA IZQUIERDA
    inputIA.derecha = false;   // INICIALIZA DERECHA
    inputIA.atacar = false;    // INICIALIZA ATAQUE

    float dx = jugador.x() - enemigo.x(); // DIFERENCIA EN X HACIA EL JUGADOR
    float dz = jugador.z() - enemigo.z(); // DIFERENCIA EN Z HACIA EL JUGADOR

    // MUEVE EN AMBOS EJES SIMULTANEAMENTE
    if (dx > 0.1f)       inputIA.derecha = true;   // JUGADOR A LA DERECHA
    else if (dx < -0.1f) inputIA.izquierda = true;  // JUGADOR A LA IZQUIERDA

    if (dz > 0.1f)       inputIA.atras = true;    // INVERTIDO: Z POSITIVO ES ATRAS
    else if (dz < -0.1f) inputIA.delante = true;  // INVERTIDO: Z NEGATIVO ES DELANTE
}

void IAArena::accionAtacar(const Combatiente& enemigo, const Combatiente& jugador,
    EstadoJugador& inputIA, float dt)  // ← AÑADIR dt
{
    float dx = jugador.x() - enemigo.x(); // DIFERENCIA EN X
    float dz = jugador.z() - enemigo.z(); // DIFERENCIA EN Z
    float distancia = calcularDistancia(enemigo, jugador); // CALCULA SEPARACION

    // ACUMULA TIEMPO DESDE EL ULTIMO ATAQUE
    _tiempoEntreAtaques += dt;

    // SOLO ATACA SI HA PASADO EL COOLDOWN
    if (_tiempoEntreAtaques >= _cooldownIA)
    {
        inputIA.atacar = true;      // ACTIVA ATAQUE
        _tiempoEntreAtaques = 0.0f; // RESETEA EL TIMER
    }
    else
    {
        inputIA.atacar = false;     // DESACTIVA ATAQUE
    }

    // RECOLOCA LIGERAMENTE SI ESTA MUY PEGADO
    if (distancia < _distanciaAtaque * 0.5f) {
        inputIA.delante = (dz > 0); // REAJUSTE ADELANTE
        inputIA.atras = (dz < 0);   // REAJUSTE ATRAS
        inputIA.izquierda = false;  // BLOQUEA IZQUIERDA
        inputIA.derecha = false;    // BLOQUEA DERECHA
    }
    else {
        inputIA.delante = inputIA.atras = false; // BLOQUEA EJE Z
        inputIA.izquierda = inputIA.derecha = false; // BLOQUEA EJE X
    }
}

void IAArena::accionHuir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA)
{
    // RESETEA INPUT ANTES DE DECIDIR
    inputIA.delante = false;
    inputIA.atras = false;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = false;

    float dx = enemigo.x() - jugador.x(); // DIRECCION OPUESTA AL JUGADOR EN X
    float dz = enemigo.z() - jugador.z(); // DIRECCION OPUESTA AL JUGADOR EN Z

    // HUYE EN LA DIRECCION CONTRARIA AL JUGADOR
    if (std::abs(dx) > std::abs(dz)) // EJE X DOMINANTE
    {
        if (dx > 0) inputIA.derecha = true;   // SE ALEJA HACIA LA DERECHA
        else        inputIA.izquierda = true;  // SE ALEJA HACIA LA IZQUIERDA
    }
    else // EJE Z DOMINANTE
    {
        if (dz > 0.1f)       inputIA.delante = true;  // HUYE EN Z POSITIVO
        else if (dz < -0.1f) inputIA.atras = true;    // HUYE EN Z NEGATIVO
    }
}