// ============================================================
// MinimaxTablero.h
// IA ESTRATÉGICA DEL TABLERO — ALGORITMO MINIMAX CON PODA ALPHA-BETA
// Calcula el mejor movimiento para la IA (bando_rival) y expone
// métodos públicos para el sistema de sugerencias al jugador humano.
// Incluye la plantilla PilaHistorial para deshacer movimientos simulados
// y los structs MovimientoIA y EstadoCasilla usados por toda la IA.
// ============================================================

#pragma once
#include "Tablero.h"       // ACCESO A CASILLAS, BANDOS Y TIPOS DE PIEZA
#include "estadojuego.h"   // NivelDificultad: FACIL, NORMAL, DIFICIL
#include <vector>          // LISTA DE MOVIMIENTOS GENERADOS
#include <climits>         // INT_MIN / INT_MAX PARA INICIALIZAR ALPHA-BETA

// ============================================================
// PLANTILLA GENÉRICA: PILA LIFO PARA GUARDAR Y RESTAURAR ESTADOS
// USADA PARA DESHACER MOVIMIENTOS SIMULADOS DENTRO DEL MINIMAX
// ============================================================
template<typename T>
class PilaHistorial {
    std::vector<T> _pila; // ALMACÉN INTERNO DE ESTADOS
public:
    void guardar(const T& estado) { _pila.push_back(estado); }  // APILA UN ESTADO
    T    recuperar() { T t = _pila.back(); _pila.pop_back(); return t; } // DESAPILA Y DEVUELVE
    bool vacia() const { return _pila.empty(); }     // TRUE SI NO HAY NADA GUARDADO
};

// ============================================================
// STRUCT: REPRESENTA UN MOVIMIENTO POSIBLE EN EL TABLERO
// ============================================================
struct MovimientoIA {
    int filaOrigen{ -1 }; // FILA DE LA PIEZA QUE SE MUEVE
    int colOrigen{ -1 }; // COLUMNA DE LA PIEZA QUE SE MUEVE
    int filaDestino{ -1 }; // FILA A LA QUE SE MUEVE
    int colDestino{ -1 }; // COLUMNA A LA QUE SE MUEVE
};

// ============================================================
// STRUCT: SNAPSHOT DE UNA CASILLA ANTES DE APLICAR UN MOVIMIENTO
// PERMITE RESTAURAR EL TABLERO AL ESTADO PREVIO (DESHACER)
// ============================================================
struct EstadoCasilla {
    int        fila{ -1 };         // FILA DE LA CASILLA GUARDADA
    int        col{ -1 };         // COLUMNA DE LA CASILLA GUARDADA
    TipoPieza  pieza{ pieza_nada }; // TIPO DE PIEZA QUE HABÍA
    BandoPieza bando{ bando_nada }; // BANDO AL QUE PERTENECÍA
    Pieza* obj{ nullptr };    // PUNTERO AL OBJETO PIEZA (PUEDE SER NULLPTR)
};

// ============================================================
// CLASE PRINCIPAL: MOTOR DE IA PARA EL TABLERO
// ============================================================
class MinimaxTablero {

    int _profundidad; // NIVELES DE BÚSQUEDA EN EL ÁRBOL DE JUEGO

    // NÚCLEO RECURSIVO DEL ALGORITMO — PRIVADO, SOLO USA calcularMejorMovimiento
    int minimax(Tablero& tablero, int profundidad, bool maximizar,
        int alpha, int beta);

    // COMPRUEBA SI EL ESTADO DEL TABLERO ES TERMINAL (VICTORIA DE ALGÚN BANDO)
    bool hayVictoria(const Tablero& tablero) const;

public:
    // CONSTRUCTOR — PROFUNDIDAD POR DEFECTO SEGÚN NivelDificultad::FACIL
    MinimaxTablero(int profundidad = (int)NivelDificultad::FACIL);

    // CAMBIA LA DIFICULTAD EN CALIENTE DESDE EL MENÚ
    void setDificultad(NivelDificultad nivel) {
        _profundidad = static_cast<int>(nivel); // CONVIERTE ENUM A ENTERO
    }

    // DEVUELVE LA DIFICULTAD ACTUALMENTE CONFIGURADA
    NivelDificultad getDificultad() const {
        return static_cast<NivelDificultad>(_profundidad); // CONVIERTE ENTERO A ENUM
    }

    // CALCULA Y DEVUELVE EL MEJOR MOVIMIENTO PARA LA IA (bando_rival)
    MovimientoIA calcularMejorMovimiento(Tablero& tablero);

    // ── MÉTODOS PÚBLICOS PARA EL SISTEMA DE SUGERENCIAS AL JUGADOR ──

    // GENERA TODOS LOS MOVIMIENTOS LEGALES DE UN BANDO
    std::vector<MovimientoIA> generarMovimientos(const Tablero& tablero, BandoPieza bando) const;

    // APLICA UN MOVIMIENTO EN EL TABLERO Y DEVUELVE EL ESTADO ANTERIOR (PARA DESHACER)
    EstadoCasilla aplicarMovimiento(Tablero& tablero, const MovimientoIA& mov);

    // RESTAURA EL TABLERO AL ESTADO PREVIO A UN MOVIMIENTO SIMULADO
    void deshacerMovimiento(Tablero& tablero, const MovimientoIA& mov, const EstadoCasilla& estado);

    // PUNTÚA EL ESTADO ACTUAL DEL TABLERO (POSITIVO = BUENO PARA IA, NEGATIVO = BUENO PARA JUGADOR)
    int evaluar(const Tablero& tablero) const;
};