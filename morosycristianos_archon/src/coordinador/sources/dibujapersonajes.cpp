#include "dibujapersonajes.h"


// Rutas de cada spritesheet según el nombre del personaje
const char* dibujapersonajes::rutaImagen(TipoPersonaje tipo) {
    switch (tipo) {
    case TipoPersonaje::REY:
        return "imagenes\\rey.png";
    case TipoPersonaje::EMIR:           
        return "imagenes\\emir.png";
    case TipoPersonaje::MILICIANO:     
        return "imagenes\\miliciano.png";
    case TipoPersonaje::GHAZI:
        return "imagenes\\ghazi.png";
    case TipoPersonaje::INFANTERIA:
        return "imagenes\\infanteria.png";
    case TipoPersonaje::GUARDIA_NEGRA:  
        return "imagenes\\guardia_negra.png";
    case TipoPersonaje::BALLESTERO:
            return "imagenes\\ballestero.png"; 
    case TipoPersonaje::ARQUERO_GHAZI:
        return "imagenes\\arquero_ghazi.png";
    case TipoPersonaje::CABALLERIA_PESADA:
        return "imagenes\\caballeria_pesada.png";  
    case TipoPersonaje::CABALLERIA_ACORAZADA:
        return "imagenes\\caballeria_acorazada.png";
    case TipoPersonaje::CABALLERIA_LIGERA:
        return "imagenes\\caballeria_ligera.png";
    case TipoPersonaje::JINETE_BEREBER: 
        return "imagenes\\jinete_bereber.png";
    case TipoPersonaje::ALMOGAVAR:
        return "imagenes\\almogavar.png";
    case TipoPersonaje::ARQUERO_CABALLO:
        return "imagenes\\arquero_caballo.png";
    case TipoPersonaje::INFILTRADO:
        return "imagenes\\infiltrado.png";
    case TipoPersonaje::ASESINO_DE_ELITE:     
        return "imagenes\\asesino_de_elite.png";
    
    
    default:                            
        return "";
    }
}


//Devuelve el tipo de personaje segun el caso
//Cada caso engloba la pieza cristiana con su correspondiente andalusi
TipoPersonaje dibujapersonajes::tipoDesdePieza(TipoPieza pieza, BandoPieza bando) {
    switch (pieza) {
    case pieza_lider:
        return (bando == bando_rival) ? TipoPersonaje::EMIR : TipoPersonaje::REY;
    case pieza_basica1:     
        return (bando == bando_rival) ? TipoPersonaje::GHAZI : TipoPersonaje::MILICIANO;
    case pieza_fuerte1:       
        return (bando == bando_rival) ? TipoPersonaje::GUARDIA_NEGRA : TipoPersonaje::INFANTERIA;
    case pieza_basica2:
        return (bando == bando_rival) ? TipoPersonaje::ARQUERO_GHAZI : TipoPersonaje::BALLESTERO;
    case pieza_fuerte2:
        return (bando == bando_rival) ? TipoPersonaje::CABALLERIA_ACORAZADA : TipoPersonaje::CABALLERIA_PESADA;
    case pieza_volador1:   
        return (bando == bando_rival) ? TipoPersonaje::JINETE_BEREBER : TipoPersonaje::CABALLERIA_LIGERA;
    case pieza_volador2:
        return (bando == bando_rival) ? TipoPersonaje::ARQUERO_CABALLO : TipoPersonaje::ALMOGAVAR;
    case pieza_teleporte:  
        return (bando == bando_rival) ? TipoPersonaje::ASESINO_DE_ELITE : TipoPersonaje::INFILTRADO;

    //Por seguridad:
    default:                return TipoPersonaje::MILICIANO;
    }
}

//Constructor que recorre cada sprite de cada personaje, inicializandolos para evitar errores
dibujapersonajes::dibujapersonajes() {
    for (int i = 0; i < (int)TipoPersonaje::TOTAL; i++)
        for (int j = 0; j < MAX_SPRITES; j++)
            _sprites[i][j] = nullptr;
}

//Destructor correspondiente
dibujapersonajes::~dibujapersonajes() {
    for (int i = 0; i < (int)TipoPersonaje::TOTAL; i++)
        for (int j = 0; j < MAX_SPRITES; j++)
            delete _sprites[i][j];
}

//Método para inicializaar los frames
void dibujapersonajes::init() {

    //bucle para recorrer cada personaje
    //usa la función SpriteSequence de la librería ETSIDI
    //
    for (int t = 0; t < (int)TipoPersonaje::TOTAL; t++) {
        const char* ruta = rutaImagen((TipoPersonaje)t);
        for (int i = 0; i < MAX_SPRITES; i++) {
            //crea matriz de sprites
            _sprites[t][i] = new ETSIDI::SpriteSequence(
                ruta,
                4, 4, 150, true,  // 4 col, 4 filas, 150ms por frame
                0, 0, 30, 30  //pixeles de cada frame
            );
            //inicializa el estado de cada sprite en false (frame congelado)
            _sprites[t][i]->setState((int)EstadoPersonaje::IDLE, false);
        }
    }

}

//Método para dibujar personajes según tipo, coordenadas, estado del personaje, comprueba si hace falta voltear y si esta en movimiento
void dibujapersonajes::dibujar(TipoPersonaje tipo, float x, float y, float size, EstadoPersonaje estado, int indice, bool enMovimiento, bool voltear)
{
    //Según tipo de personaje 
    int t = (int)tipo;

    if (indice < 0 || indice >= MAX_SPRITES) return;
    if (!_sprites[t][indice]) return;
  

    //Actualiza el estado del frame con el indice correspondiente
    int currentFrame = _sprites[t][indice]->getState();

    //segun estado del personaje 
    int baseFrame = (int)estado;

    //Para estado IDLE:
    if (estado == EstadoPersonaje::IDLE) {

        //Si la pieza esta quieta, se congela el primer frame (posicíon de reposo)
        if (!enMovimiento) {
             _sprites[t][indice]->setState(baseFrame, true);
        }

        //Pieza en movimiento:
        else {

            //Para personaje infiltrado / asesino de elite, los frames de IDLE funcionan distinto
            if (tipo == TipoPersonaje::INFILTRADO || tipo == TipoPersonaje::ASESINO_DE_ELITE) {
                //Avanza la animación hasta llegar al último frame de la fila (bolita), que se congela: 

                //Seguridad por si viene de otro estado 
                if (currentFrame < baseFrame || currentFrame > baseFrame + 3) {
                    _sprites[t][indice]->setState(baseFrame, false);
                } 
                else if (currentFrame == baseFrame + 3) {
                    //Congelar último frame (bolita)
                    _sprites[t][indice]->pause(true);
                }
                else {
                    // Si está en el frame 0, 1 o 2, avanza hacia la bolita
                    _sprites[t][indice]->pause(false);
                }


            }

            //Resto de personajes:
            else {
                // Avanza animación primera fila completa sin congelar
                if (currentFrame < baseFrame || currentFrame >= baseFrame + 3)
                    _sprites[t][indice]->setState(baseFrame, false);
                else
                    _sprites[t][indice]->pause(false); //vuelve a animar
            }
        }
    }
    
    //Para estado ATTACK / HURT:
    else {
        //Avanza animación primera fila completa sin congelar
        if (currentFrame < baseFrame || currentFrame >= baseFrame + 3)
            _sprites[t][indice]->setState(baseFrame, false);
    }

    //Actualiza la posicion y el tamaño con las funciones de la librería ETSIDI
    _sprites[t][indice]->setPos(x, y);
    _sprites[t][indice]->setSize(size, size * 1.3f);


    //Según flag voltear, giramos el personaje con coordenadas negativas
    //Dibujamos sprites del personaje con la función draw de la librería ETSIDI

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

}

//Método para actualizar 
void dibujapersonajes::update() {
    for (int t = 0; t < (int)TipoPersonaje::TOTAL; t++)
        for (int i = 0; i < MAX_SPRITES; i++)
            if (_sprites[t][i])
                _sprites[t][i]->loop();
}