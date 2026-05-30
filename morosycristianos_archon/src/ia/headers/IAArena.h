// ============================================================
// IAArena.h
// IA DE COMBATE EN LA ARENA — MÁQUINA DE ESTADOS PARA EL RIVAL
// Controla al combatiente enemigo durante los enfrentamientos 1vs1.
// Implementa cuatro estados: PATRULLAR, PERSEGUIR, ATACAR y HUIR,
// con transiciones automáticas según distancia y vida restante.
// ============================================================

#pragma once
#include "Combatiente.h" // DATOS DE POSICIÓN, VIDA Y ALCANCE DEL COMBATIENTE
#include "InputState.h"  // ESTRUCTURA EstadoJugador: FLAGS DE MOVIMIENTO Y ATAQUE

class IAArena
{
    // ESTADOS POSIBLES DE LA MÁQUINA DE ESTADOS
    enum class Estado {
        PATRULLAR, // MOVIMIENTO LATERAL MIENTRAS NO HAY ENEMIGO A LA VISTA
        PERSEGUIR, // AVANZA HACIA EL JUGADOR CUANDO LO DETECTA
        ATACAR,    // GOLPEA AL JUGADOR CUANDO ESTÁ EN RANGO
        HUIR       // SE RETIRA AL TENER LA VIDA CRÍTICA
    };

    Estado _estado{ Estado::PATRULLAR }; // ESTADO INICIAL AL EMPEZAR EL COMBATE

    float _distanciaAtaque{ 2.0f }; // RANGO MÍNIMO PARA GOLPEAR
    float _distanciaPerseguir{ 7.0f }; // RANGO A PARTIR DEL CUAL PERSIGUE
    float _umbralHuida{ 0.25f }; // PORCENTAJE DE VIDA PARA ACTIVAR HUIDA
    float _tiempoPatrulla{ 0.0f }; // ACUMULADOR DEL TIMER DE PATRULLA
    float _duracionPatrulla{ 0.5f }; // SEGUNDOS ENTRE CAMBIOS DE DIRECCIÓN EN PATRULLA
    bool  _patrullaArriba{ true }; // DIRECCIÓN ACTUAL DEL MOVIMIENTO DE PATRULLA
    float _tiempoEntreAtaques{ 0.0f }; // ACUMULADOR DEL COOLDOWN DE ATAQUE
    float _cooldownIA{ 1.2f }; // SEGUNDOS MÍNIMOS ENTRE ATAQUES CONSECUTIVOS

    // CALCULA LA DISTANCIA EUCLIDEA ENTRE DOS COMBATIENTES
    float calcularDistancia(const Combatiente& a, const Combatiente& b) const;

    // EVALÚA DISTANCIA Y VIDA Y ACTUALIZA _estado
    void actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia);

    // EJECUTA EL MOVIMIENTO DE PATRULLA CAMBIANDO DIRECCIÓN CADA _duracionPatrulla SEGUNDOS
    void accionPatrullar(EstadoJugador& inputIA, float dt);

    // MUEVE LA IA HACIA EL JUGADOR EN AMBOS EJES SIMULTÁNEAMENTE
    void accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);

    // ATACA RESPETANDO EL COOLDOWN Y SE RECOLOCA SI ESTÁ DEMASIADO PEGADO
    void accionAtacar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt);

    // HUYE EN LA DIRECCIÓN OPUESTA AL JUGADOR POR EL EJE DOMINANTE
    void accionHuir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);

public:
    IAArena() = default; // CONSTRUCTOR POR DEFECTO — SIN INICIALIZACIÓN ESPECIAL

    // AJUSTA _distanciaAtaque Y _distanciaPerseguir SEGÚN EL ALCANCE DE LA PIEZA
    void configurar(float alcanceAtaque);

    // PUNTO DE ENTRADA PRINCIPAL — LLAMA CADA FRAME DESDE LA ARENA
    // DECIDE EL ESTADO Y RELLENA inputIA CON LOS FLAGS DE MOVIMIENTO Y ATAQUE
    void actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt);
};