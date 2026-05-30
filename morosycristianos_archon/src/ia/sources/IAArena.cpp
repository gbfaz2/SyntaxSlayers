// ============================================================
// IAArena.cpp
// IMPLEMENTACIÓN DE LA IA DE COMBATE EN LA ARENA
// Máquina de estados con cuatro comportamientos: PATRULLAR,
// PERSEGUIR, ATACAR y HUIR. Se actualiza cada frame desde la
// arena y rellena un EstadoJugador con los flags de movimiento
// y ataque que el motor de combate interpreta igual que el input
// del jugador humano.
// ============================================================

#include "IAArena.h" // CABECERA DE LA CLASE
#include <cmath>     // std::sqrt Y std::abs PARA CÁLCULO DE DISTANCIAS

// ============================================================
// PUNTO DE ENTRADA PRINCIPAL — LLAMADO CADA FRAME DESDE LA ARENA
// DECIDE EL ESTADO ACTUAL Y EJECUTA LA ACCIÓN CORRESPONDIENTE
// ============================================================
void IAArena::actualizar(const Combatiente& enemigo, const Combatiente& jugador,
    EstadoJugador& inputIA, float dt)
{
    float distancia = calcularDistancia(enemigo, jugador); // DISTANCIA ACTUAL ENTRE LOS DOS COMBATIENTES

    actualizarEstado(enemigo, jugador, distancia); // EVALÚA Y CAMBIA _estado SI ES NECESARIO

    switch (_estado)
    {
    case Estado::PATRULLAR: accionPatrullar(inputIA, dt);                    break; // MOVIMIENTO LATERAL DE ESPERA
    case Estado::PERSEGUIR: accionPerseguir(enemigo, jugador, inputIA);      break; // AVANZA HACIA EL JUGADOR
    case Estado::ATACAR:    accionAtacar(enemigo, jugador, inputIA, dt);     break; // GOLPEA CON COOLDOWN
    case Estado::HUIR:      accionHuir(enemigo, jugador, inputIA);           break; // SE ALEJA DEL JUGADOR
    }
}

// ============================================================
// CALCULA LA DISTANCIA EUCLIDEA ENTRE DOS COMBATIENTES
// ============================================================
float IAArena::calcularDistancia(const Combatiente& a, const Combatiente& b) const
{
    float dx = a.x() - b.x(); // DIFERENCIA DE POSICIÓN EN EL EJE X
    float dz = a.z() - b.z(); // DIFERENCIA DE POSICIÓN EN EL EJE Z
    return std::sqrt(dx * dx + dz * dz); // DISTANCIA EUCLIDEA — TEOREMA DE PITÁGORAS
}

// ============================================================
// AJUSTA LOS RANGOS DE ATAQUE Y PERSECUCIÓN SEGÚN EL ALCANCE
// DE LA PIEZA — LLAMADO AL INICIAR CADA COMBATE
// ============================================================
void IAArena::configurar(float alcanceAtaque)
{
    _distanciaAtaque = alcanceAtaque + 0.4f; // RANGO DE GOLPE CON MARGEN DE SEGURIDAD
    _distanciaPerseguir = alcanceAtaque + 3.0f; // RANGO DE AVISTAMIENTO PROPORCIONAL AL ALCANCE
}

// ============================================================
// EVALÚA VIDA Y DISTANCIA Y ACTUALIZA _estado
// PRIORIDAD: HUIR > ATACAR > PERSEGUIR > PATRULLAR
// ============================================================
void IAArena::actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia)
{
    float vidaFraccion = enemigo.vida() / enemigo.vidaMax(); // PORCENTAJE DE VIDA RESTANTE (0.0 - 1.0)

    if (vidaFraccion < _umbralHuida)          // VIDA CRÍTICA — PRIORIDAD MÁXIMA
        _estado = Estado::HUIR;
    else if (distancia <= _distanciaAtaque)   // EN RANGO DE GOLPE
        _estado = Estado::ATACAR;
    else if (distancia <= _distanciaPerseguir) // ENEMIGO A LA VISTA
        _estado = Estado::PERSEGUIR;
    else                                       // ENEMIGO DEMASIADO LEJOS
        _estado = Estado::PATRULLAR;
}

// ============================================================
// ACCIÓN PATRULLAR — MOVIMIENTO LATERAL ALTERNANDO DIRECCIÓN
// CADA _duracionPatrulla SEGUNDOS MIENTRAS NO HAY ENEMIGO A LA VISTA
// ============================================================
void IAArena::accionPatrullar(EstadoJugador& inputIA, float dt)
{
    _tiempoPatrulla += dt; // ACUMULA EL TIEMPO TRANSCURRIDO

    if (_tiempoPatrulla >= _duracionPatrulla) { // HA PASADO EL INTERVALO DE CAMBIO
        _patrullaArriba = !_patrullaArriba;     // INVIERTE LA DIRECCIÓN
        _tiempoPatrulla = 0.0f;                 // REINICIA EL CONTADOR
    }

    inputIA.delante = _patrullaArriba;   // MUEVE EN LA DIRECCIÓN ACTUAL
    inputIA.atras = !_patrullaArriba;  // OPUESTO A LA DIRECCIÓN ACTUAL
    inputIA.izquierda = false;             // SIN MOVIMIENTO LATERAL
    inputIA.derecha = false;             // SIN MOVIMIENTO LATERAL
    inputIA.atacar = false;             // SIN ATAQUE EN PATRULLA
}

// ============================================================
// ACCIÓN PERSEGUIR — MUEVE LA IA HACIA EL JUGADOR EN AMBOS EJES
// ============================================================
void IAArena::accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador,
    EstadoJugador& inputIA)
{
    // RESETEA TODOS LOS FLAGS ANTES DE DECIDIR
    inputIA.delante = inputIA.atras = inputIA.izquierda = inputIA.derecha = inputIA.atacar = false;

    float dx = jugador.x() - enemigo.x(); // VECTOR HACIA EL JUGADOR EN X
    float dz = jugador.z() - enemigo.z(); // VECTOR HACIA EL JUGADOR EN Z

    // MUEVE EN EL EJE X HACIA EL JUGADOR
    if (dx > 0.1f) inputIA.derecha = true; // JUGADOR A LA DERECHA
    else if (dx < -0.1f) inputIA.izquierda = true; // JUGADOR A LA IZQUIERDA

    // MUEVE EN EL EJE Z HACIA EL JUGADOR (Z POSITIVO = ATRÁS EN LA ARENA)
    if (dz > 0.1f) inputIA.atras = true; // JUGADOR DETRÁS
    else if (dz < -0.1f) inputIA.delante = true; // JUGADOR DELANTE
}

// ============================================================
// ACCIÓN ATACAR — GOLPEA RESPETANDO EL COOLDOWN Y SE RECOLOCA
// SI ESTÁ DEMASIADO PEGADO AL JUGADOR
// ============================================================
void IAArena::accionAtacar(const Combatiente& enemigo, const Combatiente& jugador,
    EstadoJugador& inputIA, float dt)
{
    float dx = jugador.x() - enemigo.x();     // DIFERENCIA EN X
    float dz = jugador.z() - enemigo.z();     // DIFERENCIA EN Z
    float distancia = calcularDistancia(enemigo, jugador); // DISTANCIA ACTUAL

    _tiempoEntreAtaques += dt; // ACUMULA TIEMPO DESDE EL ÚLTIMO GOLPE

    if (_tiempoEntreAtaques >= _cooldownIA) { // COOLDOWN COMPLETADO — PUEDE ATACAR
        inputIA.atacar = true;           // ACTIVA EL FLAG DE ATAQUE
        _tiempoEntreAtaques = 0.0f;           // REINICIA EL COOLDOWN
    }
    else {
        inputIA.atacar = false; // AÚN EN COOLDOWN — NO ATACA
    }

    if (distancia < _distanciaAtaque * 0.5f) {
        // DEMASIADO CERCA — SE RECOLOCA LIGERAMENTE PARA NO SOLAPARSE
        inputIA.delante = (dz > 0); // SE ALEJA UN POCO EN Z
        inputIA.atras = (dz < 0);
        inputIA.izquierda = false;
        inputIA.derecha = false;
    }
    else {
        // DISTANCIA CORRECTA — SE QUEDA QUIETO Y SOLO ATACA
        inputIA.delante = inputIA.atras = inputIA.izquierda = inputIA.derecha = false;
    }
}

// ============================================================
// ACCIÓN HUIR — SE ALEJA DEL JUGADOR POR EL EJE DOMINANTE
// ACTIVA CUANDO LA VIDA CAE POR DEBAJO DE _umbralHuida
// ============================================================
void IAArena::accionHuir(const Combatiente& enemigo, const Combatiente& jugador,
    EstadoJugador& inputIA)
{
    // RESETEA TODOS LOS FLAGS ANTES DE DECIDIR
    inputIA.delante = inputIA.atras = inputIA.izquierda = inputIA.derecha = inputIA.atacar = false;

    float dx = enemigo.x() - jugador.x(); // VECTOR DE ALEJAMIENTO EN X (OPUESTO AL JUGADOR)
    float dz = enemigo.z() - jugador.z(); // VECTOR DE ALEJAMIENTO EN Z (OPUESTO AL JUGADOR)

    if (std::abs(dx) > std::abs(dz)) {
        // EJE X DOMINANTE — HUYE EN HORIZONTAL
        if (dx > 0) inputIA.derecha = true; // SE ALEJA HACIA LA DERECHA
        else        inputIA.izquierda = true; // SE ALEJA HACIA LA IZQUIERDA
    }
    else {
        // EJE Z DOMINANTE — HUYE EN PROFUNDIDAD
        if (dz > 0.1f) inputIA.delante = true; // HUYE HACIA DELANTE
        else if (dz < -0.1f) inputIA.atras = true; // HUYE HACIA ATRÁS
    }
}