#pragma once
#include "Combatiente.h"
#include "InputState.h"


// CONTROLA AL ENEMIGO EN LA ARENA MEDIANTE UNA MAQUINA DE ESTADOS

class IAArena
{
    // ESTADOS POSIBLES DE LA IA
    enum class Estado {
        PATRULLAR,  // SE MUEVE SIN OBJETIVO FIJO
        PERSEGUIR,  // VA HACIA EL JUGADOR
        ATACAR,     // ESTA EN RANGO, ATACA
        HUIR        // VIDA BAJA, SE ALEJA
    };

    Estado _estado{ Estado::PATRULLAR }; // ESTADO INICIAL

    // UMBRALES QUE DISPARAN LOS CAMBIOS DE ESTADO
    float _distanciaAtaque{ 2.0f };    // DISTANCIA PARA ATACAR
    float _distanciaPerseguir{ 7.0f }; // MITAD DE LA ARENA
    float _umbralHuida{ 0.25f };       // % DE VIDA PARA HUIR (25%)

public:
    IAArena() = default;

    // RELLENA EL EstadoJugador DE _p2 SEGUN LA LOGICA DE LA FSM
    void actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);

private:
    // CALCULA DISTANCIA ENTRE LOS DOS COMBATIENTES
    float calcularDistancia(const Combatiente& a, const Combatiente& b) const;

    // TRANSICIONES ENTRE ESTADOS
    void actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia);

    // ACCIONES DE CADA ESTADO
    void accionPatrullar(EstadoJugador& inputIA);
    void accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);
    void accionAtacar(EstadoJugador& inputIA);
    void accionHuir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);
};
