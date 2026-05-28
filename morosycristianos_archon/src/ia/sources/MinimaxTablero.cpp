#include "MinimaxTablero.h"
#include <iostream>
#include <climits>
#include <algorithm>

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

    const int dirs[4][2] = { {0,1},{0,-1},{1,0},{-1,0} }; // SOLO 4 DIRECCIONES

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.bando != bando || cas.obj == nullptr) continue;

            int radio = 1;
            switch (cas.pieza) {
            case pieza_esfera:     radio = 3; break;
            case pieza_dodecaedro: radio = 8; break;
            case pieza_icosaedro:  radio = 4; break;
            case pieza_tetraedro:  radio = 3; break;
            case pieza_cubog:      radio = 2; break;
            case pieza_cono:       radio = 3; break;
            case pieza_cilindro:   radio = 3; break;
            case pieza_cubo_p:     radio = 3; break;
            default:               radio = 1; break;
            }

            // AVANZA EN CADA DIRECCIÓN PARANDO SI HAY UNA PIEZA EN EL CAMINO
            for (auto& d : dirs) {
                for (int paso = 1; paso <= radio; paso++) {
                    int tf = f + d[0] * paso;
                    int tc = c + d[1] * paso;
                    if (tf < 0 || tf >= Tablero::N || tc < 0 || tc >= Tablero::N) break;

                    BandoPieza bandoDst = tablero.getCasilla(tf, tc).bando;
                    if (bandoDst == bando) break; // ALIADO — BLOQUEADO, PARA

                    movimientos.push_back({ f, c, tf, tc });

                    if (bandoDst != bando_nada) break; // ENEMIGO — CAPTURA Y PARA
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
    int piezasLocal = 0, piezasRival = 0;
    int poderLocal = 0, poderRival = 0;

    // RECORREMOS TABLERO UNA SOLA VEZ PARA NO RALENTIZAR LA IA
    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {

            const Casilla& cas = tablero.getCasilla(f, c);

            if (cas.bando == bando_local) {
                piezasLocal++;
                // SUMAMOS SI ESTÁ EN UN PUNTO DE PODER
                if (cas.tipo == Casilla_poder) poderLocal++;
            }
            else if (cas.bando == bando_rival) {
                piezasRival++;
                // SUMAMOS SI ESTÁ EN UN PUNTO DE PODER
                if (cas.tipo == Casilla_poder) poderRival++;
            }
        }
    }

    // VICTORIA 1: ANIQUILACIÓN (CERO PIEZAS VIVAS)
    if (piezasLocal == 0 || piezasRival == 0) return true;

    // VICTORIA 2: DOMINIO (5 PUNTOS DE PODER CONTROLADOS)
    if (poderLocal == 5 || poderRival == 5) return true;

    // IGNORAMOS ASEDIO EN LA SIMULACIÓN PORQUE CALCULAR VECINOS CONSUME MUCHA CPU
    return false;
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

            int val = valorPieza(cas.pieza);  // VALOR BASE

            if (cas.obj != nullptr && cas.obj->getVidaMax() > 0) {

                // ESCALAMOS EL VALOR SEGÚN EL PORCENTAJE DE VIDA RESTANTE
                val = (val * cas.obj->getVida()) / cas.obj->getVidaMax();

                // SUMAMOS LA FUERZA COMO BONUS DE PELIGROSIDAD
                val += cas.obj->getFuerza();
			}

            if (cas.tipo == Casilla_poder) val += 50;  // BONUS PUNTO DE PODER

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
    if (profundidad == 0 || hayVictoria(tablero))
        return evaluar(tablero);

    BandoPieza bando = maximizar ? bando_rival : bando_local;
    auto movimientos = generarMovimientos(tablero, bando);

    if (movimientos.empty()) return evaluar(tablero);

    // LAMBDA LOCAL PARA SABER EL VALOR DE LAS PIEZAS (IGUAL QUE EN EVALUAR)
    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_esfera:     return 1000;
        case pieza_dodecaedro: return   90;
        case pieza_icosaedro:  return   85;
        case pieza_tetraedro:  return   60;
        case pieza_cubog:      return   70;
        case pieza_cono:       return   75;
        case pieza_cilindro:   return   60;
        case pieza_cubo_p:     return   30;
        default:               return    0;
        }
        };

    // 1. ORDENAR MOVIMIENTOS: PROBAR PRIMERO LAS CAPTURAS MÁS VALIOSAS (OPTIMIZA PODA)
    std::sort(movimientos.begin(), movimientos.end(), [&tablero, &valorPieza](const MovimientoIA& a, const MovimientoIA& b) {
        int valA = valorPieza(tablero.getCasilla(a.filaDestino, a.colDestino).pieza);
        int valB = valorPieza(tablero.getCasilla(b.filaDestino, b.colDestino).pieza);
        return valA > valB; // DE MAYOR A MENOR VALOR DE CAPTURA DESTINO
        });

    PilaHistorial<EstadoCasilla> historial;

    if (maximizar) {
        int mejorValor = INT_MIN;
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov);
            historial.guardar(estado);
            int valor = minimax(tablero, profundidad - 1, false, alpha, beta);
            deshacerMovimiento(tablero, mov, historial.recuperar());

            // 2. BONIFICACIÓN POR CAPTURAR PIEZAS ENEMIGAS
            // EL ESTADO GUARDÓ LA PIEZA QUE PISAMOS AL MOVER
            if (estado.pieza != pieza_nada && estado.bando == bando_local) {
                valor += valorPieza(estado.pieza) * 2; // PREMIA DEVORAR PIEZAS
            }

            mejorValor = std::max(mejorValor, valor);
            alpha = std::max(alpha, valor);
            if (beta <= alpha) break; // PODA BETA
        }
        return mejorValor;
    }
    else {
        int mejorValor = INT_MAX;
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov);
            historial.guardar(estado);
            int valor = minimax(tablero, profundidad - 1, true, alpha, beta);
            deshacerMovimiento(tablero, mov, historial.recuperar());

            // 3. PENALIZACIÓN POR EXPONER NUESTRAS PIEZAS AL RIVAL
            // SI EL RIVAL NOS COME ALGO EN SU TURNO SIMULADO, RESTAMOS PUNTOS
            if (estado.pieza != pieza_nada && estado.bando == bando_rival) {
                valor -= valorPieza(estado.pieza) / 2; // PENALIZA PERDER TROPAS
            }

            mejorValor = std::min(mejorValor, valor);
            beta = std::min(beta, valor);
            if (beta <= alpha) break; // PODA ALPHA
        }
        return mejorValor;
    }
}

// MÉTODO PRINCIPAL: CALCULA EL MEJOR MOVIMIENTO PARA LA IA
MovimientoIA MinimaxTablero::calcularMejorMovimiento(Tablero& tablero)
{
    std::streambuf* bufOriginal = std::cout.rdbuf(nullptr);

    MovimientoIA mejorMov;
    int mejorValor = INT_MIN;

    // TRABAJAMOS SOBRE UNA COPIA — EL TABLERO REAL NUNCA SE TOCA
    Tablero copia = tablero;

    auto movimientos = generarMovimientos(copia, bando_rival);

    for (const auto& mov : movimientos) {
        EstadoCasilla estado = aplicarMovimiento(copia, mov);
        int valor = minimax(copia, _profundidad - 1, false, INT_MIN, INT_MAX);
        deshacerMovimiento(copia, mov, estado);

        if (valor > mejorValor) {
            mejorValor = valor;
            mejorMov = mov;
        }
    }

    std::cout.rdbuf(bufOriginal);
    std::cout << "[IA] Movimiento: ("
        << mejorMov.filaOrigen << "," << mejorMov.colOrigen << ") -> ("
        << mejorMov.filaDestino << "," << mejorMov.colDestino << ")\n";

    return mejorMov;
}