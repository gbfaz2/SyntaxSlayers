#include "IAArena.h"
#include <cmath> // PARA sqrt

// ─────────────────────────────────────────────────────────────

void IAArena::actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA)
{
    float distancia = calcularDistancia(enemigo, jugador); // DISTANCIA ACTUAL ENTRE LOS DOS

    actualizarEstado(enemigo, jugador, distancia); // DECIDE EN QUE ESTADO ESTAR

    // EJECUTA LA ACCION DEL ESTADO ACTUAL
    switch (_estado)
    {
    case Estado::PATRULLAR: accionPatrullar(inputIA);                        break;
    case Estado::PERSEGUIR: accionPerseguir(enemigo, jugador, inputIA);      break;
    case Estado::ATACAR:    accionAtacar(inputIA);                           break;
    case Estado::HUIR:      accionHuir(enemigo, jugador, inputIA);           break;
    }
}

// ─────────────────────────────────────────────────────────────

float IAArena::calcularDistancia(const Combatiente& a, const Combatiente& b) const
{
    float dx = a.x() - b.x(); // DIFERENCIA EN X
    float dz = a.z() - b.z(); // DIFERENCIA EN Z
    return std::sqrt(dx * dx + dz * dz); // DISTANCIA EUCLIDEA
}

// ─────────────────────────────────────────────────────────────

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

// ─────────────────────────────────────────────────────────────

void IAArena::accionPatrullar(EstadoJugador& inputIA)
{
    // SE MUEVE HACIA ADELANTE HASTA QUE EL JUGADOR ENTRE EN RANGO
    inputIA.delante = true;
    inputIA.atras = false;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = false;
}

// ─────────────────────────────────────────────────────────────

void IAArena::accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA)
{
    // RESETEA INPUT ANTES DE DECIDIR
    inputIA.delante = false;
    inputIA.atras = false;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = false;

    float dx = jugador.x() - enemigo.x(); // DIFERENCIA EN X HACIA EL JUGADOR
    float dz = jugador.z() - enemigo.z(); // DIFERENCIA EN Z HACIA EL JUGADOR

    // MUEVE EN EL EJE DOMINANTE PARA IR HACIA EL JUGADOR
    if (std::abs(dx) > std::abs(dz)) // MAS SEPARADOS EN X QUE EN Z
    {
        if (dx > 0) inputIA.derecha = true;  // JUGADOR A LA DERECHA
        else        inputIA.izquierda = true; // JUGADOR A LA IZQUIERDA
    }
    else // MAS SEPARADOS EN Z
    {
        if (dz > 0) inputIA.delante = true; // JUGADOR DELANTE
        else        inputIA.atras = true;   // JUGADOR DETRAS
    }
}

// ─────────────────────────────────────────────────────────────

void IAArena::accionAtacar(EstadoJugador& inputIA)
{
    // PARA Y ATACA
    inputIA.delante = false;
    inputIA.atras = false;
    inputIA.izquierda = false;
    inputIA.derecha = false;
    inputIA.atacar = true; // DISPARA EL ATAQUE
}

// ─────────────────────────────────────────────────────────────

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
        if (dz > 0) inputIA.delante = true; // SE ALEJA HACIA ADELANTE
        else        inputIA.atras = true;   // SE ALEJA HACIA ATRAS
    }
}
