#pragma once
#include "Tablero.h"
#include "estadojuego.h"   // ← NivelDificultad viene de aquí
#include <vector>
#include <climits>

// ============================================================
// PLANTILLA PARA HISTORIAL DE ESTADOS (REQUISITO OOP)
// ============================================================
template<typename T>
class PilaHistorial {
    std::vector<T> _pila;
public:
    void guardar(const T& estado) { _pila.push_back(estado); }
    T recuperar() { T t = _pila.back(); _pila.pop_back(); return t; }
    bool vacia() const { return _pila.empty(); }
};

// MOVIMIENTO POSIBLE PARA LA IA
struct MovimientoIA {
    int filaOrigen{ -1 };
    int colOrigen{ -1 };
    int filaDestino{ -1 };
    int colDestino{ -1 };
};

// ESTADO GUARDADO PARA DESHACER UN MOVIMIENTO
struct EstadoCasilla {
    int fila{ -1 };
    int col{ -1 };
    TipoPieza  pieza{ pieza_nada };
    BandoPieza bando{ bando_nada };
    Pieza* obj{ nullptr };
};

class MinimaxTablero {

    int _profundidad; // PROFUNDIDAD DE BÚSQUEDA

    int minimax(Tablero& tablero, int profundidad, bool maximizar,
        int alpha, int beta);

    int evaluar(const Tablero& tablero) const;

    std::vector<MovimientoIA> generarMovimientos(
        const Tablero& tablero, BandoPieza bando) const;

    EstadoCasilla aplicarMovimiento(
        Tablero& tablero, const MovimientoIA& mov);

    void deshacerMovimiento(Tablero& tablero, const MovimientoIA& mov,
        const EstadoCasilla& estadoAnterior);

    bool hayVictoria(const Tablero& tablero) const;

public:
    // CONSTRUCTOR: profundidad por defecto = FÁCIL (2 niveles)
    MinimaxTablero(int profundidad = (int)NivelDificultad::FACIL);

    // CAMBIA LA DIFICULTAD DINÁMICAMENTE DESDE EL MENÚ
    void setDificultad(NivelDificultad nivel) {
        _profundidad = static_cast<int>(nivel);
    }

    // DEVUELVE LA DIFICULTAD ACTUAL
    NivelDificultad getDificultad() const {
        return static_cast<NivelDificultad>(_profundidad);
    }  // ← llave cierra getDificultad, NO contiene nada más

    // MÉTODO PRINCIPAL DE LA IA
    MovimientoIA calcularMejorMovimiento(Tablero& tablero);
};