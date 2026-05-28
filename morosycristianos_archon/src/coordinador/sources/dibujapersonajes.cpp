#include "dibujapersonajes.h"
#include <iostream>

using namespace std;


// Rutas de cada spritesheet con un switch case
const char* dibujapersonajes::rutaImagen(TipoPersonaje tipo) {
    switch (tipo) {
    case TipoPersonaje::MILICIANO:     
        return "imagenes\\MILICIANO_SISISISI.png";
    case TipoPersonaje::REY:
        return "imagenes\\REY_PRUEBA.png";
    case TipoPersonaje::EMIR:           
        return "imagenes\\EMIR_SISISI.png";
    case TipoPersonaje::ASESINO_DE_ELITE:     
        return "imagenes\\asesino_de_elite.png";
    case TipoPersonaje::GUARDIA_NEGRA:  
        return "imagenes\\guardia_negra.png";
    case TipoPersonaje::JINETE_BEREBER: 
        return "imagenes\\jinete_bereber.png";
    case TipoPersonaje::INFANTERIA:
        return "imagenes\\INFANTERIA_SISISI.png";
    case TipoPersonaje::INFILTRADO:
        return "imagenes\\INFILTRADO_SISISI.png";
    case TipoPersonaje::CABALLERIA_PESADA:
        return "imagenes\\CABALLERIA_PESADA.png";  
    case TipoPersonaje::BALLESTERO:
            return "imagenes\\BALLESTERO_SISISI.png";
    case TipoPersonaje::GHAZI:
        return "imagenes\\GHAZI.png";
    case TipoPersonaje::CABALLERIA_ACORAZADA:
        return "imagenes\\caballeria_acorazada.png";
    case TipoPersonaje::CABALLERIA_LIGERA:
        return "imagenes\\caballeria_ligera.png";
    case TipoPersonaje::ARQUERO_GHAZI:
        return "imagenes\\arquero_ghazi.png";


    default:                            
        return "";
    }
}


//switch case según tipo de personaje y pieza asociada
//corregir segun vayamos metiendo personajes
TipoPersonaje dibujapersonajes::tipoDesdePieza(TipoPieza pieza, BandoPieza bando) {
    switch (pieza) {
    case pieza_cubo_p:     
        return (bando == bando_rival) ? TipoPersonaje::GHAZI : TipoPersonaje::MILICIANO;
    case pieza_dodecaedro:  
        return (bando == bando_rival) ? TipoPersonaje::ASESINO_DE_ELITE : TipoPersonaje::INFILTRADO;
    case pieza_cubog:       
        return (bando == bando_rival) ? TipoPersonaje::GUARDIA_NEGRA : TipoPersonaje::INFANTERIA;
    case pieza_tetraedro:   
        return (bando == bando_rival) ? TipoPersonaje::JINETE_BEREBER : TipoPersonaje::CABALLERIA_LIGERA;
    case pieza_esfera:
        return (bando == bando_rival) ? TipoPersonaje::EMIR : TipoPersonaje::REY;
    case pieza_cono:
        return (bando == bando_rival) ? TipoPersonaje::CABALLERIA_ACORAZADA : TipoPersonaje::CABALLERIA_PESADA;
    case pieza_cilindro:
        return (bando == bando_rival) ? TipoPersonaje::ARQUERO_GHAZI : TipoPersonaje::BALLESTERO;
   


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
                4, 4, 150, true,  // 4 cols, 3 filas, 150ms por frame
                0, 0, 30, 30
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
  

    //primero se actualiza el estado del frame
    int currentFrame = _sprites[t][indice]->getState();
    int baseFrame = (int)estado;


    if (estado == EstadoPersonaje::IDLE) {
        if (!enMovimiento) {
            // Congelado en el primer frame
            //if (currentFrame != baseFrame)
             _sprites[t][indice]->setState(baseFrame, true);
        }

        else {
            // Bucle frames 0-2 para andar
            if (currentFrame < baseFrame || currentFrame >= baseFrame + 2)
                _sprites[t][indice]->setState(0, false);
            else 
                _sprites[t][indice]->pause(false);//vuelve a animar

        }
    }
    else {
        // ATTACK o HURT: 4 frames completos
        if (currentFrame < baseFrame || currentFrame >= baseFrame + 4)
            _sprites[t][indice]->setState(baseFrame, false);
    }

    //ahora se actualiza la pos y el tamaño
    _sprites[t][indice]->setPos(x, y);
    _sprites[t][indice]->setSize(size, size * 1.3f);

    //comprueba si hay que voltear o no, y dibuja en funcion de voltear

    if (voltear) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        //coordenadas negativas para darles la vuelta como un espejo
        glScalef(-1.0f, 1.0f, 1.0f);
        glTranslatef(-x, -y, 0.0f);
        _sprites[t][indice]->draw();
        glPopMatrix();
    }
    else {
        _sprites[t][indice]->draw();
    }

}


void dibujapersonajes::update() {
    for (int t = 0; t < (int)TipoPersonaje::TOTAL; t++)
        for (int i = 0; i < MAX_SPRITES; i++)
            if (_sprites[t][i])
                _sprites[t][i]->loop();
}