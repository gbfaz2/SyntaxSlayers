
// Autor: Ines Alcérreca Sánchez
// Define los estados globales del juego, los modos, bandos y batallas historicas

#pragma once
#include <vector>
#include <string>
#include <cstdlib>

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
enum class BandoJugador { NINGUNO, CRISTIANO, MUSULMAN };

// Las 4 batallas historicas del juego
enum class Batalla {
    GUADALETE,       // 711  - victoria musulmana
    ALARCOS,         // 1195 - victoria musulmana
    NAVAS_TOLOSA,    // 1212 - victoria cristiana
    GRANADA          // 1492 - victoria cristiana
};

// Sortea una batalla aleatoria
inline Batalla sortearBatalla() {
    return static_cast<Batalla>(rand() % 4);
}

// Devuelve que bando tiene la iniciativa segun la batalla sorteada
inline BandoJugador iniciativa(Batalla b) {
    if (b == Batalla::GUADALETE || b == Batalla::ALARCOS)
        return BandoJugador::MUSULMAN;
    return BandoJugador::CRISTIANO;
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

// Descripcion historica de cada batalla
inline const char* descripcionBatalla(Batalla b) {
    switch (b) {
    case Batalla::GUADALETE:
        return "Los musulmanes cruzan el estrecho y vencen al rey Rodrigo.";
    case Batalla::ALARCOS:
        return "Al-Mansur derrota a Alfonso VIII de Castilla.";
    case Batalla::NAVAS_TOLOSA:
        return "Los reinos cristianos unidos rompen el poder almohade.";
    case Batalla::GRANADA:
        return "Los Reyes Catolicos completan la Reconquista peninsular.";
    default: return "";
    }
}

// Contexto historico detallado (varias lineas) para la pantalla de destino
inline std::vector<std::string> contextoBatalla(Batalla b) {
    switch (b) {
    case Batalla::GUADALETE:
        return {
            "Año 711. El caudillo bereber Tariq ibn Ziyad",
            "cruza el Estrecho de Gibraltar con 7.000 hombres.",
            "El rey visigodo Rodrigo cae derrotado junto al Guadalete.",
            "En apenas tres años la Península cae bajo dominio islámico.",
            "Comienza el largo período de Al-Ándalus."
        };
    case Batalla::ALARCOS:
        return {
            "Año 1195. El califa almohade Al-Mansur",
            "dirige su ejército contra Alfonso VIII de Castilla.",
            "En las llanuras de Alarcos, los castellanos son aplastados.",
            "La derrota frena la Reconquista durante casi dos décadas.",
            "Los almohades alcanzan su máximo poder en Hispania."
        };
    case Batalla::NAVAS_TOLOSA:
        return {
            "Año 1212. Una cruzada papal une a los reinos",
            "de Castilla, Aragón y Navarra bajo una sola bandera.",
            "En el paso de Sierra Morena, el ejército almohade",
            "es destruido por la coalición cristiana.",
            "El poder almohade jamás se recuperará."
        };
    case Batalla::GRANADA:
        return {
            "Año 1492. Tras diez años de campañas militares,",
            "los Reyes Católicos asedian el reino nazarí de Granada.",
            "El sultán Boabdil entrega las llaves el 2 de enero.",
            "La última ciudad musulmana de la Península cae.",
            "La Reconquista culmina. España nace."
        };
    default: return {};
    }
}

// Configuracion de la partida actual
struct ConfigPartida {
    ModoJuego modo = ModoJuego::NINGUNO;
    BandoJugador     bando = BandoJugador::NINGUNO;
    Batalla   batalla = Batalla::GUADALETE;
    BandoJugador     turno1 = BandoJugador::NINGUNO;   // quien empieza
};
