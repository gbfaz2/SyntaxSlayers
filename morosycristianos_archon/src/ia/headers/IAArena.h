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
    float _tiempoPatrulla{ 0.0f };      // ACUMULA TIEMPO EN PATRULLA
    float _duracionPatrulla{ 0.5f };    // SEGUNDOS ANTES DE CAMBIAR DIRECCION
    bool _patrullaArriba{ true };       // DIRECCION ACTUAL DE PATRULLA
    float _tiempoEntreAtaques{ 0.0f };   // ACUMULA TIEMPO ENTRE ATAQUES
    float _cooldownIA{ 1.2f };           // SEGUNDOS ENTRE CADA ATAQUE DE LA IA

    float calcularDistancia(const Combatiente& a, const Combatiente& b) const;
    void  actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia);
    void  accionPatrullar(EstadoJugador& inputIA, float dt);
    void  accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);
    void  accionAtacar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt);
    void  accionHuir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA);

public:
    IAArena() = default;

	void configurar(float alcanceAtaque); // SIRVE PARA CONFIGURAR LOS UMBRALES DE LA FSM SEGUN EL ALCANCE DE ATAQUE DEL ENEMIGO, PARA QUE SE ADAPTE A CUALQUIER PIEZA

    // RELLENA EL EstadoJugador DE _p2 SEGUN LA LOGICA DE LA FSM
    void actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt);
    
};
