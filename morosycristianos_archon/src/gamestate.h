// gamestate.h
// Autor: Ines Alcérreca Sánchez
#pragma once

enum class GameState {
    E_INTRO,
    E_MENU,
    E_AJUSTES,
    E_TABLERO,
    E_ARENA,
    E_RANKING,
    E_GAMEOVER,
    E_PAUSA
};

enum class GameMode { NONE, PVP, PVC };
enum class Bando    { NONE, CRISTIANO, MUSULMAN };

struct GameConfig {
    GameMode mode   = GameMode::NONE;
    Bando    bando  = Bando::NONE;
};
