#include "PiezaTeleporte.h"

PiezaTeleporte::PiezaTeleporte(std::string nombre, Bando bando,
    int vidaMax, int fuerza,
    int velAtaque, int recarga, int radioMov)
    : Pieza(nombre, bando, vidaMax, fuerza, velAtaque, recarga, radioMov)
    //le dice que primero llame al constructor de pieza con sus respectivas inicializaciones
    //de forma que no hay que volver a inicializar todo
{
    // vacío, Pieza ya inicializa todo
}


bool PiezaTeleporte::puedeMoverse(int filaDestino, int colDestino) const {
    // Cracterísticas teleporte:
    // puede moverse a cualquier casilla del tablero (no tiene limites)
    // Solo comprueba que el destino esté dentro del tablero 9x9
    if (filaDestino < 0 || filaDestino > 8) return false;
    if (colDestino < 0 || colDestino  > 8) return false;

    return true;
}
