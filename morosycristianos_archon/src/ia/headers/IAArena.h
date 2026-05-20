#pragma once
#include "Combatiente.h"
#include "InputState.h"

class IAArena
{
    enum class Estado {
        PATRULLAR,  // MOVIMIENTO PROVISIONAL ALEATORIO
        PERSEGUIR,  // EN BUSCA DEL JUGADOR
        ATACAR,     // ENEMIGO EN RANGO
        HUIR        // RETIRADA POR VIDA BAJA
    };

    Estado _estado{ Estado::PATRULLAR }; // ESTADO INICIAL IA

    float _distanciaAtaque{ 2.0f };    // RANGO GOLPEO INDIVIDUAL
    float _distanciaPerseguir{ 7.0f }; // RANGO AVISTAMIENTO ENEMIGO
    float _umbralHuida{ 0.25f };       // PORCENTAJE AGUANTE CRITICO
    float _tiempoPatrulla{ 0.0f };      // CRONOMETRO CAMBIO RUTA
    float _duracionPatrulla{ 0.5f };    // INTERVALO GIRO PATRULLA
    bool _patrullaArriba{ true };       // BANDERA SENTIDO MOVIMIENTO
    float _tiempoEntreAtaques{ 0.0f };   // CRONOMETRO CONTROL RITMO
    float _cooldownIA{ 1.2f };           // RETARDO GOLPEO FIJO

    float calcularDistancia(const Combatiente& a, const Combatiente& b) const; // METRICA SEPARACION FIJO
    void  actualizarEstado(const Combatiente& enemigo, const Combatiente& jugador, float distancia); // MAQUINA CAMBIO ESTADOS
    void  accionPatrullar(EstadoJugador& inputIA, float dt); // LOGICA MOVIMIENTO ALEATORIO
    void  accionPerseguir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA); // LOGICA ACERCAMIENTO JUGADOR
    void  accionAtacar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt); // LOGICA GOLPEO RANGO
    void  accionHuir(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA); // LOGICA DISTANCIAMIENTO JUGADOR

public:
    IAArena() = default; // CONSTRUCTOR DEFECTO

    void configurar(float alcanceAtaque); // ADAPTACION RANGO PIEZA

    void actualizar(const Combatiente& enemigo, const Combatiente& jugador, EstadoJugador& inputIA, float dt); // DISPARADOR FISICAS MAQUINA
};