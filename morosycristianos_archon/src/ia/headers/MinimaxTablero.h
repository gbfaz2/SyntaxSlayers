#pragma once
#include "Tablero.h"
#include <vector>
#include <climits>

// PLANTILLA PARA HISTORIAL DE ESTADOS
template<typename T>
class PilaHistorial {
    std::vector<T> _pila;
public:
    void guardar(const T& estado) { _pila.push_back(estado); }  // GUARDA ESTADO
    T recuperar() { T t = _pila.back(); _pila.pop_back(); return t; } // RECUPERA Y ELIMINA
    bool vacia() const { return _pila.empty(); }                // COMPRUEBA SI VACÍA
};

// MOVIMIENTO POSIBLE PARA LA IA
struct MovimientoIA {
    int filaOrigen{ -1 };   // FILA DE LA PIEZA A MOVER
    int colOrigen{ -1 };    // COL DE LA PIEZA A MOVER
    int filaDestino{ -1 };  // FILA DESTINO
    int colDestino{ -1 };   // COL DESTINO
};

<<<<<<< Updated upstream
    Bando _bandoIA;        // BANDO QUE CONTROLA LA IA
    Bando _bandoJugador;   // BANDO DEL JUGADOR HUMANO
    private://ELIMINAR 
    // ALGORITMO MINIMAX CON PODA ALFA-BETA
    int minimax(Pieza** tablero, int tamanio, int profundidad,
                int alfa, int beta, bool esMaximizador) const;
=======
// ESTADO GUARDADO PARA DESHACER UN MOVIMIENTO
struct EstadoCasilla {
    int fila{ -1 };                  // FILA DE LA CASILLA
    int col{ -1 };                   // COL DE LA CASILLA
    TipoPieza  pieza{ pieza_nada };  // TIPO DE PIEZA
    BandoPieza bando{ bando_nada };  // BANDO
    Pieza* obj{ nullptr };           // PUNTERO AL OBJETO
};
>>>>>>> Stashed changes

class MinimaxTablero {

    int _profundidad;                // PROFUNDIDAD DE BÚSQUEDA

    int minimax(Tablero& tablero, int profundidad, bool maximizar,
        int alpha, int beta);                          // MINIMAX CON ALPHA-BETA

<<<<<<< Updated upstream
    // VALOR DE CADA PIEZA PARA LA HEURISTICA
    int valorPieza(const Pieza& pieza) const;

public:
    // RECIBE EL BANDO QUE JUEGA LA IA
    MinimaxTablero(Bando bandoIA);

    // DEVUELVE EL MEJOR MOVIMIENTO POSIBLE DADO EL TABLERO ACTUAL
    Movimiento obtenerMejorMovimiento(Pieza** tablero, int tamanio) const;

    // DEVUELVE EL MEJOR MOVIMIENTO PARA EL JUGADOR (USADO POR SugerenciasTablero)
    Movimiento obtenerMejorMovimientoJugador(Pieza** tablero, int tamanio) const;
};
*/
=======
    int evaluar(const Tablero& tablero) const;                 // EVALÚA LA POSICIÓN

    std::vector<MovimientoIA> generarMovimientos(
        const Tablero& tablero, BandoPieza bando) const;       // GENERA MOVIMIENTOS

    EstadoCasilla aplicarMovimiento(
        Tablero& tablero, const MovimientoIA& mov);            // APLICA MOVIMIENTO

    void deshacerMovimiento(Tablero& tablero, const MovimientoIA& mov,
        const EstadoCasilla& estadoAnterior); // DESHACE MOVIMIENTO

    bool hayVictoria(const Tablero& tablero) const;            // COMPRUEBA VICTORIA

public:
    MinimaxTablero(int profundidad = 2);                       // CONSTRUCTOR

    MovimientoIA calcularMejorMovimiento(Tablero& tablero);    // MÉTODO PRINCIPAL
};
>>>>>>> Stashed changes
