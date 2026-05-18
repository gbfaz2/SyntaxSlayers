#pragma once
/*
#include "Pieza.h"
#include <vector>

// DECIDE EL MEJOR MOVIMIENTO PARA LA IA EN EL TABLERO

// REPRESENTA UN MOVIMIENTO EN EL TABLERO
struct Movimiento {
    int filaOrigen{-1};
    int colOrigen{-1};
    int filaDestino{-1};
    int colDestino{-1};
};

class MinimaxTablero
{
    int _profundidad{ 3 }; // NIVELES QUE MIRA HACIA ADELANTE LA IA

    Bando _bandoIA;        // BANDO QUE CONTROLA LA IA
    Bando _bandoJugador;   // BANDO DEL JUGADOR HUMANO

public:
    // RECIBE EL BANDO QUE JUEGA LA IA
    MinimaxTablero(Bando bandoIA);

    // DEVUELVE EL MEJOR MOVIMIENTO POSIBLE DADO EL TABLERO ACTUAL
    Movimiento obtenerMejorMovimiento(Pieza** tablero, int tamanio) const;

    // DEVUELVE EL MEJOR MOVIMIENTO PARA EL JUGADOR (USADO POR SugerenciasTablero)
    Movimiento obtenerMejorMovimientoJugador(Pieza** tablero, int tamanio) const;

private:
    // ALGORITMO MINIMAX CON PODA ALFA-BETA
    int minimax(Pieza** tablero, int tamanio, int profundidad,
                int alfa, int beta, bool esMaximizador) const;

    // EVALUA LO BUENO QUE ES UN ESTADO DEL TABLERO PARA LA IA
    int evaluar(Pieza** tablero, int tamanio) const;

    // GENERA TODOS LOS MOVIMIENTOS LEGALES DE UN BANDO
    std::vector<Movimiento> generarMovimientos(Pieza** tablero, int tamanio, Bando bando) const;

    // APLICA UN MOVIMIENTO AL TABLERO (SIN MODIFICAR EL ORIGINAL)
    void aplicarMovimiento(Pieza** tablero, const Movimiento& mov) const;
    void deshacerMovimiento(Pieza** tablero, const Movimiento& mov, Pieza* piezaCapturada) const;

    // VALOR DE CADA PIEZA PARA LA HEURISTICA
    int valorPieza(const Pieza& pieza) const;
};
*/