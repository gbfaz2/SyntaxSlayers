#include "dibujapersonajes.h"
#include <iostream>

using namespace std;


// Rutas de cada spritesheet con un switch case
const char* dibujapersonajes::rutaImagen(TipoPersonaje tipo) {
    switch (tipo) {
    case TipoPersonaje::MILICIANO:     
        return "imagenes\\MILICIANO_PNG.png";

    case TipoPersonaje::REY:
        return "imagenes\\REY_DEF.png";

    case TipoPersonaje::EMIR:           
        return "imagenes\\EMIR_DEF.png";
    case TipoPersonaje::INFILTRADO:     
        return "imagenes\\ASESINO_DEF.png";
    case TipoPersonaje::GUARDIA_NEGRA:  
        return "imagenes\\GUARDIANEGRA.png";
    case TipoPersonaje::JINETE_BEREBER: 
        return "imagenes\\JINETE_BEREBER.png";
    default:                            
        return "";
    }
}


//switch case según tipo de personaje y pieza asociada
//corregir segun vayamos metiendo personajes
TipoPersonaje dibujapersonajes::tipoDesdePieza(TipoPieza pieza, BandoPieza bando) {
    switch (pieza) {
    case pieza_cubo_p:     
        return TipoPersonaje::MILICIANO;
    case pieza_dodecaedro:  
        return TipoPersonaje::INFILTRADO;
    case pieza_cubog:       
        return TipoPersonaje::GUARDIA_NEGRA;
    case pieza_tetraedro:   
        return TipoPersonaje::JINETE_BEREBER;
    case pieza_esfera:
        return (bando == bando_rival) ? TipoPersonaje::EMIR : TipoPersonaje::REY;
    default:                return TipoPersonaje::MILICIANO;
    }
}

dibujapersonajes::dibujapersonajes() {
    for (int i = 0; i < (int)TipoPersonaje::TOTAL; i++)
        for (int j = 0; j < MAX_SPRITES; j++)
            _sprites[i][j] = nullptr;
}

dibujapersonajes::~dibujapersonajes() {
    for (int i = 0; i < (int)TipoPersonaje::TOTAL; i++)
        for (int j = 0; j < MAX_SPRITES; j++)
            delete _sprites[i][j];
}

void dibujapersonajes::init() {

    //bucle para todos segun tipo personaje , ruta etc
    for (int t = 0; t < (int)TipoPersonaje::TOTAL; t++) {
        const char* ruta = rutaImagen((TipoPersonaje)t);
        for (int i = 0; i < MAX_SPRITES; i++) {
            _sprites[t][i] = new ETSIDI::SpriteSequence(
                ruta,
                4, 3, 150, true,  // 4 cols, 3 filas, 150ms por frame
                0, 0, 32, 32
            );
            _sprites[t][i]->setState((int)EstadoPersonaje::IDLE, false);
        }
    }

}

void dibujapersonajes::dibujar(TipoPersonaje tipo, float x, float y, float size, EstadoPersonaje estado, int indice, bool enMovimiento, bool voltear)
{
    int t = (int)tipo;

    if (indice < 0 || indice >= MAX_SPRITES) return;
    if (!_sprites[t][indice]) return;

    _sprites[t][indice]->setPos(x, y);
    _sprites[t][indice]->setSize(size, size * 1.3f);

    //comprueba si hay que voltear o no (bando)
    if (voltear) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(-1.0f, 1.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        _sprites[t][indice]->draw();
        glPopMatrix();
    }
    else {
        _sprites[t][indice]->draw();
    }

    
    
    int currentFrame = _sprites[t][indice]->getState();
    int baseFrame = (int)estado;


    if (estado == EstadoPersonaje::IDLE) {
        if (!enMovimiento) {
            // Congelado en el primer frame
            if (currentFrame != baseFrame)
                _sprites[t][indice]->setState(baseFrame, true);
        }

        else {
            // Bucle frames 0-2

            if (currentFrame < baseFrame || currentFrame > baseFrame + 2)
                _sprites[t][indice]->setState(baseFrame, false);
        }
    }
    else {
        // ATTACK o HURT: 4 frames completos
        if (currentFrame < baseFrame || currentFrame >= baseFrame + 4)
            _sprites[t][indice]->setState(baseFrame, false);
    }


    /*
    // Definimos cuántos frames tiene esta animación
    int framesAnimacion = 4; // Por defecto son 4
    if (estado == EstadoPersonaje::IDLE) {
        framesAnimacion = 3; // La fila de IDLE (andar) solo tiene 3 dibujos
    }


    // Comprobamos que estamos en la fila correcta, y si no estamos:
    if (currentFrame < baseFrame || currentFrame >= baseFrame + framesAnimacion) {
        _milicianos[indice]->setState(baseFrame, false); // nos movemos a la fila correcta
    }

    // si se mueve la pieza: 
    if (enMovimiento) {
        _milicianos[indice]->pause(false); //empieza a andar
    }
    else {
        _milicianos[indice]->setState(baseFrame, true); // Pausamos en el primer frame para que se quede en posicion de reposo
    }
    */

    //_sprites[t][indice]->draw();
}


void dibujapersonajes::update() {
    for (int t = 0; t < (int)TipoPersonaje::TOTAL; t++)
        for (int i = 0; i < MAX_SPRITES; i++)
            if (_sprites[t][i])
                _sprites[t][i]->loop();
}