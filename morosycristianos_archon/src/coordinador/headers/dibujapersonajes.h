#pragma once
#include "ETSIDI.h"
#include "tablero.h"

//Definición nombres tipos de personajes
enum class TipoPersonaje {
    REY,
    EMIR,
    MILICIANO,
    GHAZI,
    INFANTERIA,
    GUARDIA_NEGRA,
    BALLESTERO,
    ARQUERO_GHAZI,
    CABALLERIA_PESADA,
    CABALLERIA_ACORAZADA,
    CABALLERIA_LIGERA,
    JINETE_BEREBER, 
    ALMOGAVAR,
    ARQUERO_CABALLO,
    INFILTRADO,
    ASESINO_DE_ELITE,
    TOTAL, //Siempre al final, cuenta cuántos hay
};

//Definición estados de personaje 
enum class EstadoPersonaje {
    IDLE = 0,  // fila 1
    ATTACK = 4,  // fila 2
    HURT = 8, // fila 3
};

3
class dibujapersonajes 
{
    //funcion de libreria etsidi para animación
    static const int MAX_SPRITES = 14; //MAX POR TIPO (MILICIANOS)
    ETSIDI::SpriteSequence* _sprites[(int)TipoPersonaje::TOTAL][MAX_SPRITES];

    //RUTA IMAGEN PARA CADA PERSONAJE
    static const char* rutaImagen(TipoPersonaje tipo);

public:
    //Constructor y destructor
    dibujapersonajes();
    ~dibujapersonajes();

    //Métodos públicos:
    void init();
    void update();

    //Para dibujar personajes
    void dibujar(TipoPersonaje tipo, float x, float y, float size,
        EstadoPersonaje estado, int indice, bool enMovimiento = false, bool voltear = false);
    
    //Cada tipo de pieza engloba la cristiana y su correspondiente andalusí
    //Creamos método para facilitar su uso:
    //Recibe el tipo de pieza y su bando y devuelve el tipo de personaje
    static TipoPersonaje tipoDesdePieza(TipoPieza pieza, BandoPieza bando);

};