#pragma once
#include <cstdlib>  // rand()
#include <ctime>    // time()

enum class GameState {
    E_INTRO,
    E_MENU,
    E_AJUSTES,
    E_DESTINO,    // pantalla "Destino Historico" 
    E_TABLERO,
    E_ARENA,
    E_RANKING,
    E_GAMEOVER,
    E_PAUSA
};

enum class GameMode { NONE, PVP, PVC };
enum class Bando { NONE, CRISTIANO, MUSULMAN };

enum class Batalla {
    GUADALETE,      // 711  - ganan Musulmanes
    ALARCOS,        // 1195 - ganan Musulmanes
    NAVAS_TOLOSA,   // 1212 - ganan Cristianos
    GRANADA         // 1492 - ganan Cristianos
};

// Devuelve que bando tiene iniciativa segun la batalla
inline Bando iniciativaBatalla(Batalla b) {
    if (b == Batalla::GUADALETE || b == Batalla::ALARCOS)
        return Bando::MUSULMAN;
    return Bando::CRISTIANO;
}

inline const char* nombreBatalla(Batalla b) {
    switch (b) {
    case Batalla::GUADALETE:    return "Batalla de Guadalete (711)";
    case Batalla::ALARCOS:      return "Batalla de Alarcos (1195)";
    case Batalla::NAVAS_TOLOSA: return "Las Navas de Tolosa (1212)";
    case Batalla::GRANADA:      return "Reconquista de Granada (1492)";
    default:                    return "Batalla desconocida";
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

struct GameConfig {
    GameMode mode = GameMode::NONE;
    Bando    bando = Bando::NONE;
    Batalla  batalla = Batalla::GUADALETE;  // se sortea al confirmar en menu
    Bando    turno1 = Bando::NONE;         // quien empieza
};