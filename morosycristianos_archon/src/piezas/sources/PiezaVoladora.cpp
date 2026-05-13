#include "PiezaVoladora.h"
#include <cmath>  // para abs()

PiezaVoladora::PiezaVoladora(std::string nombre, Bando bando,
    int vidaMax, int fuerza,
    int velAtaque, int recarga, int radioMov)
    : Pieza(nombre, bando, vidaMax, fuerza, velAtaque, recarga, radioMov)
    //le dice que primero llame al constructor de pieza con sus respectivas inicializaciones
    //de forma que no hay que volver a inicializar todo
{
    // vacío, Pieza ya inicializa todo
}

bool PiezaVoladora::puedeMoverse(int filaDestino, int colDestino) const {
    // Calcula cuántas casillas se mueve en cada dirección
    int difFila = std::abs(filaDestino - fila);  //para fila
    int difColumna = std::abs(colDestino - columna);  //para columna

    //características voladora: 
    //puede moverse en diagonal y saltar piezas
    //no puede moverse más casillas que su radioMov

    if (difFila > radioMov || difColumna > radioMov) return false; // supera su radio max, FALSO no se mueve

    return true; //permite moverse
}