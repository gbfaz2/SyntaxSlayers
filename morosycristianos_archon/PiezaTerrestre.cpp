#include "PiezaTerrestre.h"
#include <cmath>  // para abs()

PiezaTerrestre::PiezaTerrestre(std::string nombre, Bando bando,
    int vidaMax, int fuerza,
    int velAtaque, int recarga, int radioMov)
    : Pieza(nombre, bando, vidaMax, fuerza, velAtaque, recarga, radioMov)
    //le dice que primero llame al constructor de pieza con sus respectivas inicializaciones
    //de forma que no hay que volver a inicializar todo
{
    // vacío, Pieza ya inicializa todo
}

bool PiezaTerrestre::puedeMoverse(int filaDestino, int colDestino) const {
    // Calcula cuántas casillas se mueve en cada dirección
    int difFila = std::abs(filaDestino - fila);  //para fila
    int difColumna = std::abs(colDestino - columna);  //para columna

    //características terrestre: 
    // No puede moverse en diagonal
    // y no puede moverse más casillas que su radioMov
    if (difFila > 0 && difColumna > 0) return false;  // si intenta mover ambas fila y columna a la vez, diagonal, no permitido
    if (difFila + difColumna > radioMov) return false; // supera su radio max, lejos

    return true; //permite moverse
}
