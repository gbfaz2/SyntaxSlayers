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
            if (cas.bando != bando || cas.obj == nullptr) continue; // SOLO PIEZAS DEL BANDO

            // CASILLAS VÁLIDAS PARA ESTA PIEZA
            auto validas = tablero.casillasValidas(f, c);
            for (const auto& dest : validas) {
                movimientos.push_back({ f, c, dest.fila, dest.col }); // AÑADE MOVIMIENTO
            }
        }
    }
    return movimientos;
}

// APLICA UN MOVIMIENTO Y GUARDA ESTADO ANTERIOR
EstadoCasilla MinimaxTablero::aplicarMovimiento(
    Tablero& tablero, const MovimientoIA& mov)
{
    // GUARDA ESTADO DE LA CASILLA DESTINO ANTES DE MODIFICARLA
    EstadoCasilla estadoAnterior;
    const Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino);
    estadoAnterior.fila = mov.filaDestino;
    estadoAnterior.col = mov.colDestino;
    estadoAnterior.pieza = dest.pieza;
    estadoAnterior.bando = dest.bando;
    estadoAnterior.obj = dest.obj;    // GUARDA PUNTERO SIN BORRAR

    // MUEVE LA PIEZA (muevePieza ya gestiona captura)
    tablero.muevePieza(mov.filaOrigen, mov.colOrigen,
        mov.filaDestino, mov.colDestino);
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

    // VALOR DE CADA TIPO DE PIEZA
    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_esfera:     return 1000; // REY/EMIR — MUY VALIOSO
        case pieza_dodecaedro: return  90;  // INFILTRADO
        case pieza_icosaedro:  return  85;  // ALMOGAVAR
        case pieza_tetraedro:  return  60;  // CABALLERIA LIGERA
        case pieza_cubog:      return  70;  // INFANTERIA PESADA
        case pieza_cono:       return  75;  // CABALLERIA PESADA
        case pieza_cilindro:   return  60;  // BALLESTERO
        case pieza_cubo_p:     return  30;  // MILICIANO
        default:               return   0;
        }
        };

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.pieza == pieza_nada) continue;

            int val = valorPieza(cas.pieza);

            // BONUS POR PUNTO DE PODER
            if (cas.tipo == Casilla_poder) val += 50;

            // POSITIVO PARA RIVAL, NEGATIVO PARA LOCAL
            if (cas.bando == bando_rival)  puntuacion += val;
            if (cas.bando == bando_local)  puntuacion -= val;
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
    MovimientoIA mejorMov;
    int mejorValor = INT_MIN;                                  // IA BUSCA MÁXIMO

    auto movimientos = generarMovimientos(tablero, bando_rival); // MOVIMIENTOS IA
    std::cout << "[IA] Calculando " << movimientos.size() << " movimientos...\n";

    for (const auto& mov : movimientos) {
        EstadoCasilla estado = aplicarMovimiento(tablero, mov); // APLICA
        int valor = minimax(tablero, _profundidad - 1, false,  // EVALÚA
            INT_MIN, INT_MAX);
        deshacerMovimiento(tablero, mov, estado);              // DESHACE

        if (valor > mejorValor) {                              // SI ES MEJOR
            mejorValor = valor;
            mejorMov = mov;                                  // GUARDA MEJOR MOV
        }
    }

    std::cout << "[IA] Mejor movimiento: ("
        << mejorMov.filaOrigen << "," << mejorMov.colOrigen << ") -> ("
        << mejorMov.filaDestino << "," << mejorMov.colDestino
        << ") puntuacion=" << mejorValor << "\n";

    return mejorMov;
}