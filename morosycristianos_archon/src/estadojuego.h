
// Autor: Ines Alcérreca Sánchez
// Define los estados globales del juego, los modos, bandos y batallas historicas

#pragma once
#include <cstdlib>
#include <ctime>

// Estados principales del juego
enum class EstadoJuego {
    INTRO,          // Pantalla de introduccion con animacion y skip
    MENU,           // Menu principal: modo, bando, confirmar
    AJUSTES,        // Ajustes de sonido
    DESTINO,        // Pantalla "Destino Historico": muestra la batalla sorteada
    TABLERO,        // Fase de tablero 9x9 (turnos)
    ARENA,          // Combate en arena 1vs1
    RANKING,        // Pantalla de ranking
    FINAL,          // Pantalla de fin de partida
    PAUSA           // Pausa
};

// Modo de juego
enum class ModoJuego { NINGUNO, JVJ, JVIA };

// Bando del jugador
enum class Bando { NINGUNO, CRISTIANO, MUSULMAN };

// Las 4 batallas historicas del juego
enum class Batalla {
    GUADALETE,       // 711  - victoria musulmana
    ALARCOS,         // 1195 - victoria musulmana
    NAVAS_TOLOSA,    // 1212 - victoria cristiana
    GRANADA          // 1492 - victoria cristiana
};

// Devuelve que bando tiene la iniciativa segun la batalla sorteada
inline Bando iniciativa(Batalla b) {
    if (b == Batalla::GUADALETE || b == Batalla::ALARCOS)
        return Bando::MUSULMAN;
    return Bando::CRISTIANO;
}

// Nombre de la batalla para mostrar en pantalla
inline const char* nombreBatalla(Batalla b) {
    switch (b) {
        case Batalla::GUADALETE:    return "Batalla de Guadalete (711)";
        case Batalla::ALARCOS:      return "Batalla de Alarcos (1195)";
        case Batalla::NAVAS_TOLOSA: return "Las Navas de Tolosa (1212)";
        case Batalla::GRANADA:      return "Reconquista de Granada (1492)";
        default:                    return "";
    }
}

// Sortea una batalla aleatoria
inline Batalla sortearBatalla() {
    static bool inicializado = false;
    if (!inicializado) {
        srand((unsigned int)time(nullptr));
        inicializado = true;
    }
    return (Batalla)(rand() % 4);
}

// Configuracion de la partida actual
struct ConfigPartida {
    ModoJuego modo      = ModoJuego::NINGUNO;
    Bando     bando     = Bando::NINGUNO;
    Batalla   batalla   = Batalla::GUADALETE;
    Bando     turno1    = Bando::NINGUNO;   // quien empieza
};
