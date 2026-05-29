#pragma once
#include "ETSIDI.h"
#include "tablero.h"

//solo para miliciano de momento para prueba. Si funciona, se meteran en esta clase para todos los personajes

//falta completar para el resto de personajes

enum class TipoPersonaje {
    MILICIANO,
    REY,
    EMIR,
    ASESINO_DE_ELITE,
    GUARDIA_NEGRA,
    JINETE_BEREBER,
    INFANTERIA,
    INFILTRADO,
    CABALLERIA_PESADA,
    BALLESTERO,
    GHAZI,
    CABALLERIA_LIGERA,
    CABALLERIA_ACORAZADA,
    ARQUERO_GHAZI,
    ALMOGAVAR,
    ARQUERO_CABALLO,
    TOTAL// siempre al final, cuenta cuántos hay
};


enum class EstadoPersonaje {
    IDLE = 0,  // fila 1 del spritesheet
    ATTACK = 4,  // fila 2
    HURT = 8, // fila 3
};

class dibujapersonajes 
{

    //PENDIENTE: SWITCH CASE?? O BUCLE
    //funcion de libreria etsidi para animación
    static const int MAX_SPRITES = 14; //MAX POR TIPO (MILICIANOS)
    ETSIDI::SpriteSequence* _sprites[(int)TipoPersonaje::TOTAL][MAX_SPRITES];

    //RUTA IMAGEN PARA CADA PERSONAJE
    static const char* rutaImagen(TipoPersonaje tipo);

public:
    dibujapersonajes();
    ~dibujapersonajes();

    void init();
    void update();

    //método para dibujar miliciano
    void dibujar(TipoPersonaje tipo, float x, float y, float size,
        EstadoPersonaje estado, int indice, bool enMovimiento = false, bool voltear = false);
    
    // Helper: convierte TipoPieza + BandoPieza a TipoPersonaje
    static TipoPersonaje tipoDesdePieza(TipoPieza pieza, BandoPieza bando);

};