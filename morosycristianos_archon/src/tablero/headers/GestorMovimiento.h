#pragma once
#include "Tablero.h"
#include "Pieza.h"

// GESTOR DE MOVIMIENTO
// Escanea el camino entre origen y destino casilla a casilla (terrestres)
// o solo mira el destino (voladoras y teleporte).
// Devuelve un resultado que indica qué pasó.

enum class ResultadoMovimiento {
    MOVIMIENTO_OK,       // casilla libre, movimiento libre
    BLOQUEADO_ALIADO,    // hay un aliado en el camino o en el destino 
    COMBATE,             // hay un enemigo, hay que abrir la arena (COMBATE)
    MOVIMIENTO_INVALIDO  //supera los limites establecidos para cada tipo de pieza (ya sea diagonal, fuera del tablero, supera radiomov, etc...)
};

class GestorMovimiento {
public:

    //no es necesario constructor para este caso (no hay inicializaciones)
    // Método público que se usara desde tablero: recibe tablero, origen y destino
    // Devuelve qué resultado tiene el movimiento del tipo de datos ResultadoMovimiento  (combate, bloq...)
    ResultadoMovimiento resolverMovimiento(
        Pieza* pieza,
        Tablero& tablero,
        int toFila, int toCol
    );

private:

    //métodos privados internos:

    // Escanea casilla a casilla para terrestres
    //devuelve el tipo de "bloqueo"
    //recibe tablero, destino y origen
    ResultadoMovimiento escanearCaminoTerrestre(
        Pieza* pieza,
        Tablero& tablero,
        int toFila, int toCol
    );
    
    // Solo mira el destino (voladoras y teleporte)
    ResultadoMovimiento comprobarDestino(
        Pieza* pieza,
        Tablero& tablero,
        int toFila, int toCol
    );
   
};