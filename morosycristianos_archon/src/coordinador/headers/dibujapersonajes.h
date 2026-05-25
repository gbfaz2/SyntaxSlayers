#pragma once
#include "ETSIDI.h"

//solo para miliciano de momento para prueba. Si funciona, se meteran en esta clase para todos los personajes

enum class EstadoPersonaje {
    IDLE = 0,  // fila 1 del spritesheet
    ATTACK = 4,  // fila 2
    HURT = 8, // fila 3
};

class dibujapersonajes 
{
    //funcion de libreria etsidi para animación
    static const int MAX_MILICIANOS = 14;
    ETSIDI::SpriteSequence* _milicianos[MAX_MILICIANOS];

public:
    dibujapersonajes();
    ~dibujapersonajes();

    void init();
    void update();

    void miliciano(float x, float y, float size,
        EstadoPersonaje estado, int indice, bool enMovimiento = false);
};