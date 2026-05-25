#include "dibujapersonajes.h"
#include <iostream>


dibujapersonajes::dibujapersonajes() {
    for (int i = 0; i < MAX_MILICIANOS; i++)
        _milicianos[i] = nullptr;
}

dibujapersonajes::~dibujapersonajes() {
    for (int i = 0; i < MAX_MILICIANOS; i++)
        delete _milicianos[i];
}

void dibujapersonajes::init() {
    for (int i = 0; i < MAX_MILICIANOS; i++) {
        _milicianos[i] = new ETSIDI::SpriteSequence(
            "imagenes\\MILICIANO_PNG.png",
            4, 3, 150, true,
            0, 0, 32, 32
        );
        // AÑADE ESTA LÍNEA: Forzamos a que arranque el play en el estado IDLE
        _milicianos[i]->setState((int)EstadoPersonaje::IDLE, false);
    }
}



void dibujapersonajes::miliciano(float x, float y, float size, EstadoPersonaje estado, int indice, bool enMovimiento) {
    if (indice < 0 || indice >= MAX_MILICIANOS) return;
    if (!_milicianos[indice]) return;

    _milicianos[indice]->setPos(x, y);
    _milicianos[indice]->setSize(size, size * 1.3f);

    int currentFrame = _milicianos[indice]->getState();
    int baseFrame = static_cast<int>(estado);

    // Si el frame actual se sale de la fila correspondiente a esta animación
    // (es menor que el inicio de la fila o se ha pasado de la 4ª columna)
    // LÓGICA DE LA PRIMERA FILA (IDLE / ANDAR)
    if (estado == EstadoPersonaje::IDLE) {
        if (!enMovimiento) {
            // 1. Está quieto: lo congelamos en el primer frame (0)
            if (currentFrame != baseFrame) {
                _milicianos[indice]->setState(baseFrame, true);
            }
        }
        else {
            // 2. Está andando: bucle entre los frames 0, 1 y 2 (evitamos el 3 vacío)
            if (currentFrame < baseFrame || currentFrame > baseFrame + 2) {
                _milicianos[indice]->setState(baseFrame, false);
            }
        }
    }
    // LÓGICA DEL RESTO DE FILAS (ATTACK, HURT...)
    else {
        // 3. Animaciones completas de 4 frames
        if (currentFrame < baseFrame || currentFrame >= baseFrame + 4) {
            _milicianos[indice]->setState(baseFrame, false);
        }
    }

    //_milicianos[indice]->loop();
    std::cout << "Frame actual: " << _milicianos[indice]->getState() << std::endl;

    _milicianos[indice]->draw();
}

void dibujapersonajes::update() {
    for (int i = 0; i < MAX_MILICIANOS; i++) {
        // Solo intentamos actualizar si el miliciano existe
        if (_milicianos[i] != nullptr) {
            _milicianos[i]->loop();
            std::cout << "Miliciano " << i << " time: " << _milicianos[i]->getState() << std::endl;
        }

    }
}