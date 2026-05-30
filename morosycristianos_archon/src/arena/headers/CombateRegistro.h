#pragma once
#include <vector>
#include <string>
#include "estadojuego.h"

// Estado de los dos combatientes en un frame concreto
struct FrameArena {
    float p1x, p1z, p1vida; // posicion y vida del jugador 1
    float p2x, p2z, p2vida;
    float p1mirX, p1mirZ; // direccion a la que mira el jugador 1
    float p2mirX, p2mirZ;
    bool  p1atacando, p2atacando; // si estan en animacion de ataque
    bool  p1danio, p2danio; // si han recibido daño
    bool  p1mov, p2mov; // en movimiento
};

// Datos completos de un combate: info de los jugadores + todos los frames grabados
struct CombateRegistro {
    std::string nombreP1, nombreP2;
    float vidaMaxP1, vidaMaxP2; // vida maxima (para q la barra de vida se guarde)
    float r1, g1, b1; //colores jugadores
    float r2, g2, b2;
    std::vector<FrameArena> frames; //secuencia de frames grabados
    bool ganoP1;
    Batalla batalla;              // batalla en la que ocurrio este combate (para el fondo de la arena)
    std::string nombrePiezaP1;   // nombre de la pieza de P1, p. ej. "Miliciano" (para elegir el sprite correcto)
    std::string nombrePiezaP2;   // nombre de la pieza de P2
};

// Todos los combates registrados durante una partida completa
struct RegistroPartida {
    std::vector<CombateRegistro> combates; //luego puedes elegir q combate quieres ver
};