// ============================================================
// MinimaxTablero.cpp
// IMPLEMENTACIÓN DEL MOTOR DE IA PARA EL TABLERO
// Contiene el algoritmo Minimax con poda Alpha-Beta, la función
// de evaluación heurística, la generación de movimientos legales
// y los métodos para aplicar y deshacer movimientos simulados.
// ============================================================

#include "MinimaxTablero.h"
#include <iostream>  // PARA LOG DE DEPURACIÓN DEL MOVIMIENTO ELEGIDO
#include <climits>   // INT_MIN / INT_MAX PARA INICIALIZAR ALPHA-BETA
#include <algorithm> // std::sort Y std::max / std::min

// ============================================================
// CONSTRUCTOR
// ============================================================
MinimaxTablero::MinimaxTablero(int profundidad)
    : _profundidad(profundidad) // GUARDA LA PROFUNDIDAD DE BÚSQUEDA
{
}

// ============================================================
// GENERA TODOS LOS MOVIMIENTOS LEGALES DE UN BANDO
// RECORRE EL TABLERO Y POR CADA PIEZA DEL BANDO DADO EXPANDE
// HASTA SU RADIO EN LAS 4 DIRECCIONES, PARANDO EN ALIADOS O ENEMIGOS
// ============================================================
std::vector<MovimientoIA> MinimaxTablero::generarMovimientos(
    const Tablero& tablero, BandoPieza bando) const
{
    std::vector<MovimientoIA> movimientos; // LISTA DE MOVIMIENTOS RESULTANTE

    const int dirs[4][2] = { {0,1},{0,-1},{1,0},{-1,0} }; // 4 DIRECCIONES: DERECHA, IZQUIERDA, ABAJO, ARRIBA

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.bando != bando || cas.obj == nullptr) continue; // SALTA CASILLAS VACÍAS O DEL BANDO CONTRARIO

            // ASIGNA EL RADIO DE MOVIMIENTO SEGÚN EL TIPO DE PIEZA
            int radio = 1;
            switch (cas.pieza) {
            case pieza_lider:     radio = 3; break; // REY / EMIR
            case pieza_teleporte: radio = 8; break; // INFILTRADO
            case pieza_volador2:  radio = 4; break; // ALMOGAVAR
            case pieza_volador1:  radio = 3; break; // CABALLERÍA LIGERA
            case pieza_fuerte1:   radio = 2; break; // INFANTERÍA PESADA
            case pieza_fuerte2:   radio = 3; break; // CABALLERÍA PESADA
            case pieza_basica2:   radio = 3; break; // BALLESTERO
            case pieza_basica1:   radio = 3; break; // MILICIANO
            default:              radio = 1; break; // CASO GENÉRICO
            }

            // EXPANDE EN CADA DIRECCIÓN CASILLA A CASILLA HASTA EL RADIO
            for (auto& d : dirs) {
                for (int paso = 1; paso <= radio; paso++) {
                    int tf = f + d[0] * paso; // FILA DESTINO
                    int tc = c + d[1] * paso; // COLUMNA DESTINO

                    if (tf < 0 || tf >= Tablero::N || tc < 0 || tc >= Tablero::N) break; // FUERA DEL TABLERO

                    BandoPieza bandoDst = tablero.getCasilla(tf, tc).bando;
                    if (bandoDst == bando) break; // ALIADO — BLOQUEADO, PARA EN ESTA DIRECCIÓN

                    movimientos.push_back({ f, c, tf, tc }); // MOVIMIENTO VÁLIDO — LO AÑADE

                    if (bandoDst != bando_nada) break; // ENEMIGO — CAPTURA Y PARA EN ESTA DIRECCIÓN
                }
            }
        }
    }
    return movimientos; // DEVUELVE LA LISTA COMPLETA
}

// ============================================================
// APLICA UN MOVIMIENTO EN EL TABLERO Y DEVUELVE EL ESTADO ANTERIOR
// NO LLAMA A muevePieza PARA EVITAR EFECTOS SECUNDARIOS GRÁFICOS
// ============================================================
EstadoCasilla MinimaxTablero::aplicarMovimiento(
    Tablero& tablero, const MovimientoIA& mov)
{
    EstadoCasilla estadoAnterior;                                      // SNAPSHOT DE LA CASILLA DESTINO
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino); // CASILLA DESTINO
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);  // CASILLA ORIGEN

    // GUARDA EL ESTADO ACTUAL DEL DESTINO PARA PODER RESTAURARLO
    estadoAnterior.fila = mov.filaDestino;
    estadoAnterior.col = mov.colDestino;
    estadoAnterior.pieza = dest.pieza;
    estadoAnterior.bando = dest.bando;
    estadoAnterior.obj = dest.obj;

    // MUEVE LA PIEZA: COPIA DATOS DE ORIGEN A DESTINO
    dest.pieza = origen.pieza;
    dest.bando = origen.bando;
    dest.obj = origen.obj;
    if (dest.obj) dest.obj->setPosicion(mov.filaDestino, mov.colDestino); // ACTUALIZA POSICIÓN LÓGICA

    // LIMPIA LA CASILLA DE ORIGEN
    origen.pieza = pieza_nada;
    origen.bando = bando_nada;
    origen.obj = nullptr;

    return estadoAnterior; // DEVUELVE EL SNAPSHOT PARA DESHACER DESPUÉS
}

// ============================================================
// DESHACE UN MOVIMIENTO RESTAURANDO EL ESTADO GUARDADO
// DEVUELVE LA PIEZA ATACANTE A SU ORIGEN Y RESTAURA LA CAPTURADA
// ============================================================
void MinimaxTablero::deshacerMovimiento(
    Tablero& tablero, const MovimientoIA& mov,
    const EstadoCasilla& estadoAnterior)
{
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino); // CASILLA DESTINO
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);  // CASILLA ORIGEN

    // DEVUELVE LA PIEZA ATACANTE A SU POSICIÓN ORIGINAL
    origen.pieza = dest.pieza;
    origen.bando = dest.bando;
    origen.obj = dest.obj;
    if (origen.obj) origen.obj->setPosicion(mov.filaOrigen, mov.colOrigen); // RESTAURA POSICIÓN LÓGICA

    // RESTAURA LA CASILLA DESTINO AL ESTADO ANTERIOR (RECUPERA PIEZA CAPTURADA SI HABÍA)
    dest.pieza = estadoAnterior.pieza;
    dest.bando = estadoAnterior.bando;
    dest.obj = estadoAnterior.obj;
    if (dest.obj) dest.obj->setPosicion(mov.filaDestino, mov.colDestino); // RESTAURA POSICIÓN LÓGICA
}

// ============================================================
// COMPRUEBA SI EL TABLERO ESTÁ EN ESTADO TERMINAL (VICTORIA)
// DETECTA ANIQUILACIÓN TOTAL Y DOMINIO DE 5 PUNTOS DE PODER
// ============================================================
bool MinimaxTablero::hayVictoria(const Tablero& tablero) const
{
    int piezasLocal = 0, piezasRival = 0; // CONTADOR DE PIEZAS VIVAS POR BANDO
    int poderLocal = 0, poderRival = 0; // CONTADOR DE PUNTOS DE PODER CONTROLADOS

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);

            if (cas.bando == bando_local) {
                piezasLocal++;
                if (cas.tipo == Casilla_poder) poderLocal++; // PUNTO DE PODER CONTROLADO POR LOCAL
            }
            else if (cas.bando == bando_rival) {
                piezasRival++;
                if (cas.tipo == Casilla_poder) poderRival++; // PUNTO DE PODER CONTROLADO POR RIVAL
            }
        }
    }

    if (piezasLocal == 0 || piezasRival == 0) return true; // VICTORIA POR ANIQUILACIÓN
    if (poderLocal == 5 || poderRival == 5) return true; // VICTORIA POR DOMINIO DE BASTIONES

    return false; // PARTIDA AÚN EN CURSO
}

// ============================================================
// FUNCIÓN DE EVALUACIÓN HEURÍSTICA DEL TABLERO
// POSITIVO = BUENO PARA LA IA (bando_rival)
// NEGATIVO = BUENO PARA EL JUGADOR (bando_local)
// TIENE EN CUENTA: VALOR DE PIEZA, VIDA, FUERZA, PROXIMIDAD A
// PUNTOS DE PODER Y AVANCE EN EL TABLERO
// ============================================================
int MinimaxTablero::evaluar(const Tablero& tablero) const
{
    int puntuacion = 0; // ACUMULADOR DE LA PUNTUACIÓN TOTAL

    // VALOR BASE DE CADA TIPO DE PIEZA
    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_lider:     return 1000; // REY / EMIR — PIEZA MÁS VALIOSA
        case pieza_teleporte: return   90; // INFILTRADO
        case pieza_volador2:  return   85; // ALMOGAVAR
        case pieza_volador1:  return   60; // CABALLERÍA LIGERA
        case pieza_fuerte1:   return   70; // INFANTERÍA PESADA
        case pieza_fuerte2:   return   75; // CABALLERÍA PESADA
        case pieza_basica2:   return   60; // BALLESTERO
        case pieza_basica1:   return   30; // MILICIANO — PIEZA MÁS DÉBIL
        default:              return    0; // DESCONOCIDO
        }
        };

    const int poderPos[5][2] = { {0,4},{4,0},{4,4},{4,8},{8,4} }; // COORDENADAS DE LOS 5 BASTIONES

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.pieza == pieza_nada) continue; // CASILLA VACÍA — SALTA

            int val = valorPieza(cas.pieza); // VALOR BASE DE LA PIEZA

            if (cas.obj != nullptr && cas.obj->getVidaMax() > 0) {
                val = (val * cas.obj->getVida()) / cas.obj->getVidaMax(); // ESCALA SEGÚN VIDA RESTANTE
                val += cas.obj->getFuerza();                               // BONUS POR FUERZA DE ATAQUE
            }

            if (cas.tipo == Casilla_poder) val += 50; // BONUS EXTRA SI ESTÁ EN UN BASTIÓN

            // BONUS POR PROXIMIDAD A CADA BASTIÓN — MÁS CERCA = MÁS PUNTOS
            for (auto& pp : poderPos) {
                int dist = abs(f - pp[0]) + abs(c - pp[1]); // DISTANCIA MANHATTAN AL BASTIÓN
                int bonus = std::max(0, 10 - dist);           // MÁXIMO 10 PUNTOS, DECRECE CON LA DISTANCIA
                if (cas.bando == bando_rival) puntuacion += bonus; // SUMA PARA LA IA
                if (cas.bando == bando_local) puntuacion -= bonus; // RESTA PARA EL JUGADOR
            }

            if (cas.bando == bando_rival) {
                val += (Tablero::N - 1 - c) * 2; // BONUS POR AVANZAR HACIA EL LADO ENEMIGO
                puntuacion += val;                // SUMA AL TOTAL PARA LA IA
            }
            if (cas.bando == bando_local) {
                puntuacion -= val; // RESTA AL TOTAL PARA LA IA (BUENO PARA EL JUGADOR)
            }
        }
    }

    return puntuacion; // PUNTUACIÓN FINAL DEL ESTADO
}

// ============================================================
// NÚCLEO RECURSIVO DEL ALGORITMO MINIMAX CON PODA ALPHA-BETA
// maximizar=true  → TURNO DE LA IA    (bando_rival,  busca MAX)
// maximizar=false → TURNO DEL JUGADOR (bando_local,  busca MIN)
// ============================================================
int MinimaxTablero::minimax(Tablero& tablero, int profundidad,
    bool maximizar, int alpha, int beta)
{
    if (profundidad == 0 || hayVictoria(tablero))
        return evaluar(tablero); // NODO HOJA — DEVUELVE LA HEURÍSTICA

    BandoPieza bando = maximizar ? bando_rival : bando_local; // BANDO QUE MUEVE EN ESTE NIVEL
    auto movimientos = generarMovimientos(tablero, bando);    // GENERA MOVIMIENTOS LEGALES

    if (movimientos.empty()) return evaluar(tablero); // SIN MOVIMIENTOS — DEVUELVE HEURÍSTICA

    // VALOR BASE PARA ORDENAR CAPTURAS (IGUAL QUE EN evaluar)
    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_lider:     return 1000;
        case pieza_teleporte: return   90;
        case pieza_volador2:  return   85;
        case pieza_volador1:  return   60;
        case pieza_fuerte1:   return   70;
        case pieza_fuerte2:   return   75;
        case pieza_basica2:   return   60;
        case pieza_basica1:   return   30;
        default:              return    0;
        }
        };

    // ORDENA MOVIMIENTOS: PRIMERO LAS CAPTURAS MÁS VALIOSAS PARA MEJORAR LA PODA
    std::sort(movimientos.begin(), movimientos.end(),
        [&tablero, &valorPieza](const MovimientoIA& a, const MovimientoIA& b) {
            int valA = valorPieza(tablero.getCasilla(a.filaDestino, a.colDestino).pieza);
            int valB = valorPieza(tablero.getCasilla(b.filaDestino, b.colDestino).pieza);
            return valA > valB; // ORDEN DESCENDENTE POR VALOR DE CAPTURA
        });

    PilaHistorial<EstadoCasilla> historial; // PILA LOCAL PARA DESHACER MOVIMIENTOS

    if (maximizar) {
        int mejorValor = INT_MIN; // INICIALIZA EN EL PEOR CASO PARA MAXIMIZAR
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov); // SIMULA EL MOVIMIENTO
            historial.guardar(estado);
            int valor = minimax(tablero, profundidad - 1, false, alpha, beta); // LLAMA EN MODO MIN
            deshacerMovimiento(tablero, mov, historial.recuperar());           // RESTAURA

            // PREMIA CAPTURAS DE PIEZAS ENEMIGAS CON BONUS ADICIONAL
            if (estado.pieza != pieza_nada && estado.bando == bando_local)
                valor += valorPieza(estado.pieza) * 2;

            mejorValor = std::max(mejorValor, valor);
            alpha = std::max(alpha, valor);
            if (beta <= alpha) break; // PODA BETA — RAMA DESCARTADA
        }
        return mejorValor;
    }
    else {
        int mejorValor = INT_MAX; // INICIALIZA EN EL PEOR CASO PARA MINIMIZAR
        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov); // SIMULA EL MOVIMIENTO
            historial.guardar(estado);
            int valor = minimax(tablero, profundidad - 1, true, alpha, beta); // LLAMA EN MODO MAX
            deshacerMovimiento(tablero, mov, historial.recuperar());          // RESTAURA

            // PENALIZA EXPONER PIEZAS PROPIAS A CAPTURA
            if (estado.pieza != pieza_nada && estado.bando == bando_rival)
                valor -= valorPieza(estado.pieza) / 2;

            mejorValor = std::min(mejorValor, valor);
            beta = std::min(beta, valor);
            if (beta <= alpha) break; // PODA ALPHA — RAMA DESCARTADA
        }
        return mejorValor;
    }
}

// ============================================================
// MÉTODO PRINCIPAL — CALCULA EL MEJOR MOVIMIENTO PARA LA IA
// TRABAJA SOBRE UNA COPIA DEL TABLERO PARA NO ALTERAR EL JUEGO REAL
// ============================================================
MovimientoIA MinimaxTablero::calcularMejorMovimiento(Tablero& tablero)
{
    std::streambuf* bufOriginal = std::cout.rdbuf(nullptr); // SILENCIA COUT DURANTE EL CÁLCULO

    MovimientoIA mejorMov;           // MEJOR MOVIMIENTO ENCONTRADO
    int mejorValor = INT_MIN;        // INICIALIZA EN EL PEOR CASO

    Tablero copia = tablero;         // COPIA DE SEGURIDAD — EL TABLERO REAL NO SE TOCA

    auto movimientos = generarMovimientos(copia, bando_rival); // MOVIMIENTOS LEGALES DE LA IA

    for (const auto& mov : movimientos) {
        EstadoCasilla estado = aplicarMovimiento(copia, mov);                      // SIMULA
        int valor = minimax(copia, _profundidad - 1, false, INT_MIN, INT_MAX);     // EVALÚA
        deshacerMovimiento(copia, mov, estado);                                    // RESTAURA

        if (valor > mejorValor) {  // SI ES MEJOR QUE EL ACTUAL, LO GUARDA
            mejorValor = valor;
            mejorMov = mov;
        }
    }

    std::cout.rdbuf(bufOriginal); // RESTAURA COUT
    std::cout << "[IA] Movimiento: ("
        << mejorMov.filaOrigen << "," << mejorMov.colOrigen << ") -> ("
        << mejorMov.filaDestino << "," << mejorMov.colDestino << ")\n";

    return mejorMov; // DEVUELVE EL MEJOR MOVIMIENTO CALCULADO
}