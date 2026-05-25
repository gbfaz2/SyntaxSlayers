#include "MinimaxTablero.h"
#include <iostream>
#include <climits>

// CONSTRUCTOR
MinimaxTablero::MinimaxTablero(int profundidad)
    : _profundidad(profundidad)
{
}

// GENERA MOVIMIENTOS VÁLIDOS PARA UN BANDO
std::vector<MovimientoIA> MinimaxTablero::generarMovimientos(
    const Tablero& tablero, BandoPieza bando) const
{
    std::vector<MovimientoIA> movimientos;

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.bando != bando || cas.obj == nullptr) continue;

            auto validas = tablero.casillasValidas(f, c);
            for (const auto& dest : validas) {
                // SOLO CASILLAS VACÍAS — EVITA CORRUPCIÓN DEL TABLERO
                if (tablero.getCasilla(dest.fila, dest.col).bando == bando_nada) {
                    movimientos.push_back({ f, c, dest.fila, dest.col });
                }
            }
        }
    }
    return movimientos;
}

// APLICA UN MOVIMIENTO Y GUARDA ESTADO ANTERIOR
EstadoCasilla MinimaxTablero::aplicarMovimiento(
    Tablero& tablero, const MovimientoIA& mov)
{
    // GUARDA ESTADO DESTINO
    EstadoCasilla estadoAnterior;
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino);
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);

    estadoAnterior.fila = mov.filaDestino;
    estadoAnterior.col = mov.colDestino;
    estadoAnterior.pieza = dest.pieza;
    estadoAnterior.bando = dest.bando;
    estadoAnterior.obj = dest.obj;

    // MUEVE DIRECTAMENTE SIN LLAMAR A muevePieza (EVITA EFECTOS SECUNDARIOS)
    dest.pieza = origen.pieza;             // COPIA TIPO
    dest.bando = origen.bando;             // COPIA BANDO
    dest.obj = origen.obj;               // COPIA PUNTERO
    if (dest.obj) dest.obj->setPosicion(mov.filaDestino, mov.colDestino);

    // LIMPIA ORIGEN
    origen.pieza = pieza_nada;
    origen.bando = bando_nada;
    origen.obj = nullptr;

    return estadoAnterior;
}

// DESHACE UN MOVIMIENTO RESTAURANDO EL ESTADO GUARDADO
void MinimaxTablero::deshacerMovimiento(
    Tablero& tablero, const MovimientoIA& mov,
    const EstadoCasilla& estadoAnterior)
{
    // DEVUELVE LA PIEZA ATACANTE A SU ORIGEN
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino);
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);

    origen.pieza = dest.pieza;               // RESTAURA TIPO EN ORIGEN
    origen.bando = dest.bando;               // RESTAURA BANDO EN ORIGEN
    origen.obj = dest.obj;                 // RESTAURA PUNTERO EN ORIGEN
    if (origen.obj)
        origen.obj->setPosicion(mov.filaOrigen, mov.colOrigen); // RESTAURA POSICIÓN

    // RESTAURA LA CASILLA DESTINO AL ESTADO ANTERIOR (puede tener pieza capturada)
    dest.pieza = estadoAnterior.pieza;       // RESTAURA TIPO DESTINO
    dest.bando = estadoAnterior.bando;       // RESTAURA BANDO DESTINO
    dest.obj = estadoAnterior.obj;         // RESTAURA PUNTERO DESTINO
    if (dest.obj)
        dest.obj->setPosicion(mov.filaDestino, mov.colDestino); // RESTAURA POSICIÓN
}

// COMPRUEBA SI HAY VICTORIA
bool MinimaxTablero::hayVictoria(const Tablero& tablero) const
{
    // CUENTA PIEZAS DE CADA BANDO
    int piezasLocal = 0, piezasRival = 0;
    for (int f = 0; f < Tablero::N; f++)
        for (int c = 0; c < Tablero::N; c++) {
            if (tablero.getCasilla(f, c).bando == bando_local) piezasLocal++;
            if (tablero.getCasilla(f, c).bando == bando_rival) piezasRival++;
        }
    return piezasLocal == 0 || piezasRival == 0; // ALGUIEN SE QUEDÓ SIN PIEZAS
}

// FUNCIÓN DE EVALUACIÓN
// Positivo = bueno para la IA (bando_rival)
// Negativo = bueno para el jugador (bando_local)
int MinimaxTablero::evaluar(const Tablero& tablero) const
{
    int puntuacion = 0;

    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_esfera:     return 1000; // REY/EMIR
        case pieza_dodecaedro: return   90; // INFILTRADO
        case pieza_icosaedro:  return   85; // ALMOGAVAR
        case pieza_tetraedro:  return   60; // CABALLERIA LIGERA
        case pieza_cubog:      return   70; // INFANTERIA PESADA
        case pieza_cono:       return   75; // CABALLERIA PESADA
        case pieza_cilindro:   return   60; // BALLESTERO
        case pieza_cubo_p:     return   30; // MILICIANO
        default:               return    0;
        }
        };

    const int poderPos[5][2] = { {0,4},{4,0},{4,4},{4,8},{8,4} }; // PUNTOS DE PODER

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.pieza == pieza_nada) continue;

            int val = valorPieza(cas.pieza);                   // VALOR BASE

            if (cas.tipo == Casilla_poder) val += 50;          // BONUS PUNTO DE PODER

            // BONUS POR PROXIMIDAD A PUNTOS DE PODER
            for (auto& pp : poderPos) {
                int dist = abs(f - pp[0]) + abs(c - pp[1]);   // DISTANCIA MANHATTAN
                int bonus = std::max(0, 10 - dist);            // MÁS CERCA = MÁS BONUS
                if (cas.bando == bando_rival)  puntuacion += bonus; // BUENO PA IA
                if (cas.bando == bando_local)  puntuacion -= bonus; // MALO PA IA
            }

            if (cas.bando == bando_rival) {
                val += (Tablero::N - 1 - c) * 2;              // BONUS POR AVANZAR
                puntuacion += val;                             // SUMA PARA IA
            }
            if (cas.bando == bando_local) {
                puntuacion -= val;                             // RESTA PARA IA
            }
        }
    }

    return puntuacion;
}

// ALGORITMO MINIMAX CON PODA ALPHA-BETA
int MinimaxTablero::minimax(Tablero& tablero, int profundidad,
    bool maximizar, int alpha, int beta)
{
    // CASO BASE: profundidad 0 o victoria
    if (profundidad == 0 || hayVictoria(tablero))
        return evaluar(tablero);

    BandoPieza bando = maximizar ? bando_rival : bando_local; // BANDO QUE MUEVE
    auto movimientos = generarMovimientos(tablero, bando);    // GENERA MOVIMIENTOS

    if (movimientos.empty()) return evaluar(tablero);          // SIN MOVIMIENTOS

    PilaHistorial<EstadoCasilla> historial;                    // HISTORIAL DE ESTADOS

    if (maximizar) {
        int mejorValor = INT_MIN;                              // IA BUSCA MÁXIMO
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov); // APLICA
            historial.guardar(estado);                         // GUARDA EN HISTORIAL
            int valor = minimax(tablero, profundidad - 1, false, alpha, beta); // RECURSIVO
            deshacerMovimiento(tablero, mov, historial.recuperar()); // DESHACE
            mejorValor = std::max(mejorValor, valor);          // ACTUALIZA MEJOR
            alpha = std::max(alpha, valor);                    // ACTUALIZA ALPHA
            if (beta <= alpha) break;                          // PODA BETA
        }
        return mejorValor;
    }
    else {
        int mejorValor = INT_MAX;                              // JUGADOR BUSCA MÍNIMO
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov); // APLICA
            historial.guardar(estado);                         // GUARDA EN HISTORIAL
            int valor = minimax(tablero, profundidad - 1, true, alpha, beta); // RECURSIVO
            deshacerMovimiento(tablero, mov, historial.recuperar()); // DESHACE
            mejorValor = std::min(mejorValor, valor);          // ACTUALIZA MEJOR
            beta = std::min(beta, valor);                      // ACTUALIZA BETA
            if (beta <= alpha) break;                          // PODA ALPHA
        }
        return mejorValor;
    }
}

// MÉTODO PRINCIPAL: CALCULA EL MEJOR MOVIMIENTO PARA LA IA
MovimientoIA MinimaxTablero::calcularMejorMovimiento(Tablero& tablero)
{
    // SILENCIA PRINTS DURANTE EL CÁLCULO
    std::streambuf* bufOriginal = std::cout.rdbuf(nullptr);

    MovimientoIA mejorMov;
    int mejorValor = INT_MIN;

    auto movimientos = generarMovimientos(tablero, bando_rival);

    for (const auto& mov : movimientos) {
        EstadoCasilla estado = aplicarMovimiento(tablero, mov);
        int valor = minimax(tablero, _profundidad - 1, false, INT_MIN, INT_MAX);
        deshacerMovimiento(tablero, mov, estado);

        if (valor > mejorValor) {
            mejorValor = valor;
            mejorMov = mov;
        }
    }

    // RESTAURA PRINTS
    std::cout.rdbuf(bufOriginal);
    std::cout << "[IA] Movimiento: ("
        << mejorMov.filaOrigen << "," << mejorMov.colOrigen << ") -> ("
        << mejorMov.filaDestino << "," << mejorMov.colDestino << ")\n";

    return mejorMov;
}