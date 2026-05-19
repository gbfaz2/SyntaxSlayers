#include "IAArena.h"
#include <cmath> // PARA sqrt


void IAArena::actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt)
{
    float distancia = calcularDistancia(enemigo, jugador); // DISTANCIA ACTUAL ENTRE LOS DOS

    actualizarEstado(enemigo, jugador, distancia); // DECIDE EN QUE ESTADO ESTAR

    switch (_estado)
    {
    case Estado::PATRULLAR: accionPatrullar(inputIA, dt);break;
    case Estado::PERSEGUIR: accionPerseguir(enemigo, jugador, inputIA);break;
    case Estado::ATACAR: accionAtacar(enemigo, jugador, inputIA, dt); break;
    case Estado::HUIR:      accionHuir(enemigo, jugador, inputIA);break;
    }
}


float IAArena::calcularDistancia(const Combatiente& a, const Combatiente& b) const
{
    float dx = a.x() - b.x(); // DIFERENCIA EN X
    float dz = a.z() - b.z(); // DIFERENCIA EN Z
    return std::sqrt(dx * dx + dz * dz); // DISTANCIA EUCLIDEA
}

void IAArena::configurar(float alcanceAtaque) {
    _distanciaAtaque = alcanceAtaque + 0.4f;
    _distanciaPerseguir = alcanceAtaque + 6.0f; // persigue desde más lejos si tiene más alcance
}



void IAArena::actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia)
{
    float vidaFraccion = enemigo.vida() / enemigo.vidaMax(); // PORCENTAJE DE VIDA RESTANTE

    if (vidaFraccion < _umbralHuida) // VIDA MUY BAJA
    {
        _estado = Estado::HUIR;
    }
    else if (distancia <= _distanciaAtaque) // EN RANGO DE ATAQUE
    {
        _estado = Estado::ATACAR;
    }
    else if (distancia <= _distanciaPerseguir) // EN RANGO DE PERSECUCION
    {
        _estado = Estado::PERSEGUIR;
    }
    else // DEMASIADO LEJOS
    {
        _estado = Estado::PATRULLAR;
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
    inputIA.atras = !_patrullaArriba;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = false;
}


void IAArena::accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA)
{
    inputIA.delante = false;
    inputIA.atras = false;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = false;

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
    float dx = jugador.x() - enemigo.x();
    float dz = jugador.z() - enemigo.z();
    float distancia = calcularDistancia(enemigo, jugador);

    // ACUMULA TIEMPO DESDE EL ULTIMO ATAQUE
    _tiempoEntreAtaques += dt;

    // SOLO ATACA SI HA PASADO EL COOLDOWN
    if (_tiempoEntreAtaques >= _cooldownIA)
    {
        inputIA.atacar = true;
        _tiempoEntreAtaques = 0.0f; // RESETEA EL TIMER
    }
    else
    {
        inputIA.atacar = false;
    }

    // RECOLOCA LIGERAMENTE SI ESTA MUY PEGADO
    if (distancia < _distanciaAtaque * 0.5f) {
        inputIA.delante = (dz > 0);
        inputIA.atras = (dz < 0);
        inputIA.izquierda = false;
        inputIA.derecha = false;
    }
    else {
        inputIA.delante = inputIA.atras = false;
        inputIA.izquierda = inputIA.derecha = false;
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