// lo usamos para separar la lógica del dibujo. Primero probaremos a hacer un tablero vacío con un aspecto visual del Archon original
#pragma once
#include "Tablero.h"
#include "freeglut.h"
//creo las enumeraciones con las variables del raton y de las teclas especiales para signarles el mismo valor que tenemos en el freeglut.h
enum {MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON};
enum {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

class Tablerogl
{
};

