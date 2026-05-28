#include "MinimaxTablero.h"
#include <iostream>
#include <climits>
#include <algorithm>

// ============================================================
// CONSTRUCTOR
// Recibe la profundidad de búsqueda (cuántos turnos por delante mira la IA)
// Profundidad 1 = solo mira su siguiente movimiento
// Profundidad 2 = mira su movimiento Y tu respuesta
// Profundidad 3 = su movimiento, tu respuesta, y su siguiente movimiento
// ============================================================
MinimaxTablero::MinimaxTablero(int profundidad)
    : _profundidad(profundidad)
{
}

// ============================================================
// GENERA MOVIMIENTOS VÁLIDOS PARA UN BANDO
// 
// PARA CADA PIEZA DEL BANDO, CALCULA ADÓNDE PUEDE MOVERSE.
// USA SOLO 4 DIRECCIONES (ARRIBA, ABAJO, IZQUIERDA, DERECHA)
// SIN DIAGONALES — ES UNA SIMPLIFICACIÓN PARA QUE LA IA SEA RÁPIDA.
// 
// POR QUÉ LA IA PUEDE SER TONTA AQUÍ:
// - NO USA DIAGONALES: las piezas voladoras y de teleporte en el juego
//   real sí pueden moverse en diagonal, pero la IA no las considera.
//   Esto significa que ignora muchos movimientos tácticos importantes.
// - RADIO FIJO POR TIPO: no lee el radio real del objeto Pieza, usa
//   valores hardcodeados. Si cambias los stats de una pieza, la IA no
//   se entera.
// ============================================================
std::vector<MovimientoIA> MinimaxTablero::generarMovimientos(
    const Tablero& tablero, BandoPieza bando) const
{
    std::vector<MovimientoIA> movimientos;

<<<<<<< Updated upstream
=======
    // SOLO 4 DIRECCIONES: DERECHA, IZQUIERDA, ABAJO, ARRIBA
    const int dirs[4][2] = { {0,1},{0,-1},{1,0},{-1,0} };

>>>>>>> Stashed changes
    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);

            // SALTAMOS SI NO ES PIEZA DE NUESTRO BANDO
            if (cas.bando != bando || cas.obj == nullptr) continue;

<<<<<<< Updated upstream
            auto validas = tablero.casillasValidas(f, c);
            for (const auto& dest : validas) {

                // OBTENEMOS EL BANDO DE LA CASILLA DESTINO
                BandoPieza bandoDestino = tablero.getCasilla(dest.fila, dest.col).bando;

                // ACEPTAMOS LA CASILLA SI ESTÁ VACÍA O SI HAY UN ENEMIGO (DIFERENTE A MI BANDO)
                if (bandoDestino != bando) {
                    movimientos.push_back({ f, c, dest.fila, dest.col });
=======
            // RADIO DE MOVIMIENTO FIJO POR TIPO DE PIEZA
            // (SIMPLIFICACIÓN: no llama a cas.obj->getRadioMov() para evitar
            //  problemas con los punteros compartidos entre copia y original)
            int radio = 1;
            switch (cas.pieza) {
            case pieza_esfera:     radio = 3; break; // REY/EMIR
            case pieza_dodecaedro: radio = 8; break; // INFILTRADO (casi todo el tablero)
            case pieza_icosaedro:  radio = 4; break; // ALMOGAVAR
            case pieza_tetraedro:  radio = 3; break; // CABALLERIA LIGERA
            case pieza_cubog:      radio = 2; break; // INFANTERIA PESADA
            case pieza_cono:       radio = 3; break; // CABALLERIA PESADA
            case pieza_cilindro:   radio = 3; break; // BALLESTERO
            case pieza_cubo_p:     radio = 3; break; // MILICIANO
            default:               radio = 1; break;
            }

            // PARA CADA DIRECCIÓN, AVANZA CASILLA A CASILLA HASTA EL RADIO MÁXIMO
            for (auto& d : dirs) {
                for (int paso = 1; paso <= radio; paso++) {
                    int tf = f + d[0] * paso;
                    int tc = c + d[1] * paso;

                    // FUERA DEL TABLERO: PARA EN ESTA DIRECCIÓN
                    if (tf < 0 || tf >= Tablero::N || tc < 0 || tc >= Tablero::N) break;

                    BandoPieza bandoDst = tablero.getCasilla(tf, tc).bando;

                    // ALIADO EN EL CAMINO: BLOQUEADO, PARA EN ESTA DIRECCIÓN
                    if (bandoDst == bando) break;

                    // CASILLA VÁLIDA (LIBRE O ENEMIGO): AÑADE EL MOVIMIENTO
                    movimientos.push_back({ f, c, tf, tc });

                    // ENEMIGO EN EL CAMINO: PUEDE CAPTURAR PERO NO SEGUIR MÁS ALLÁ
                    if (bandoDst != bando_nada) break;
>>>>>>> Stashed changes
                }
            }
        }
    }
    return movimientos;
}

// ============================================================
// APLICA UN MOVIMIENTO SOBRE EL TABLERO (COPIA) Y DEVUELVE EL ESTADO ANTERIOR
// 
// ESTO ES COMO "HACER UN MOVIMIENTO EN EL TABLERO DE SIMULACIÓN".
// GUARDAMOS LO QUE HABÍA EN LA CASILLA DESTINO ANTES DE PISARLA,
// PARA PODER DESHACER EL MOVIMIENTO DESPUÉS.
// 
// MUY IMPORTANTE: NO LLAMA A setPosicion() SOBRE LOS OBJETOS PIEZA.
// SI LO LLAMARA, MOVERÍA LAS PIEZAS VISUALMENTE EN EL TABLERO REAL.
// ============================================================
EstadoCasilla MinimaxTablero::aplicarMovimiento(
    Tablero& tablero, const MovimientoIA& mov)
{
    EstadoCasilla estadoAnterior;
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino);
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);

    // GUARDAMOS TODO LO QUE HABÍA EN EL DESTINO (puede ser vacío o una pieza enemiga)
    estadoAnterior.fila = mov.filaDestino;
    estadoAnterior.col = mov.colDestino;
    estadoAnterior.pieza = dest.pieza;
    estadoAnterior.bando = dest.bando;
    estadoAnterior.obj = dest.obj;   // GUARDAMOS EL PUNTERO DE LA PIEZA CAPTURADA

    // MOVEMOS LA PIEZA DEL ORIGEN AL DESTINO (SOLO DATOS LÓGICOS, SIN GRÁFICOS)
    dest.pieza = origen.pieza;
    dest.bando = origen.bando;
    dest.obj = origen.obj;
    // NO LLAMAMOS setPosicion — ESO MOVERÍA LA PIEZA VISUALMENTE EN EL JUEGO REAL

    // VACIAMOS EL ORIGEN
    origen.pieza = pieza_nada;
    origen.bando = bando_nada;
    origen.obj = nullptr;

    return estadoAnterior;
}

// ============================================================
// DESHACE UN MOVIMIENTO RESTAURANDO EL ESTADO GUARDADO
// 
// ES EL OPUESTO DE aplicarMovimiento().
// DESPUÉS DE EVALUAR UN MOVIMIENTO SIMULADO, LO DESHACEMOS
// PARA PROBAR EL SIGUIENTE. ASÍ EL TABLERO SIEMPRE VUELVE
// AL ESTADO ORIGINAL ANTES DE EXPLORAR OTRA RAMA.
// ============================================================
void MinimaxTablero::deshacerMovimiento(
    Tablero& tablero, const MovimientoIA& mov,
    const EstadoCasilla& estadoAnterior)
{
    Casilla& dest = tablero.getCasilla(mov.filaDestino, mov.colDestino);
    Casilla& origen = tablero.getCasilla(mov.filaOrigen, mov.colOrigen);

    // DEVOLVEMOS LA PIEZA AL ORIGEN
    origen.pieza = dest.pieza;
    origen.bando = dest.bando;
    origen.obj = dest.obj;
    // NO LLAMAMOS setPosicion — MISMA RAZÓN QUE EN aplicarMovimiento

    // RESTAURAMOS EL DESTINO A LO QUE HABÍA ANTES (vacío, o la pieza que fue capturada)
    dest.pieza = estadoAnterior.pieza;
    dest.bando = estadoAnterior.bando;
    dest.obj = estadoAnterior.obj;
}

// ============================================================
// COMPRUEBA SI EL TABLERO ESTÁ EN ESTADO DE VICTORIA
// 
// SE USA COMO CONDICIÓN DE PARADA EN EL MINIMAX:
// SI ALGUIEN HA GANADO, NO TIENE SENTIDO SEGUIR EXPLORANDO.
// 
// COMPRUEBA DOS CONDICIONES:
// 1. ANIQUILACIÓN: un bando se quedó sin piezas
// 2. DOMINIO: un bando controla los 5 puntos de poder
// ============================================================
bool MinimaxTablero::hayVictoria(const Tablero& tablero) const
{
    int piezasLocal = 0, piezasRival = 0;
    int poderLocal = 0, poderRival = 0;

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);

            if (cas.bando == bando_local) {
                piezasLocal++;
                if (cas.tipo == Casilla_poder) poderLocal++;
            }
            else if (cas.bando == bando_rival) {
                piezasRival++;
                if (cas.tipo == Casilla_poder) poderRival++;
            }
        }
    }

    // VICTORIA POR ANIQUILACIÓN
    if (piezasLocal == 0 || piezasRival == 0) return true;

    // VICTORIA POR DOMINIO DE PUNTOS DE PODER
    if (poderLocal == 5 || poderRival == 5) return true;

    return false;
}

// ============================================================
// FUNCIÓN DE EVALUACIÓN — EL CEREBRO DE LA IA
// 
// CUANDO EL MINIMAX LLEGA A LA PROFUNDIDAD MÁXIMA O A UNA VICTORIA,
// LLAMA A ESTA FUNCIÓN PARA SABER SI LA POSICIÓN ES BUENA O MALA.
// 
// DEVUELVE UN NÚMERO:
//   POSITIVO = BUENO PARA LA IA (bando_rival)
//   NEGATIVO = BUENO PARA EL JUGADOR (bando_local)
//   CUANTO MÁS EXTREMO, MÁS VENTAJA TIENE ESE BANDO
// 
// POR QUÉ LA IA PUEDE SER TONTA AQUÍ:
// - SOLO EVALÚA MATERIAL Y POSICIÓN, NO ESTRATEGIA COMPLEJA.
// - NO DETECTA SI UNA PIEZA ESTÁ EN PELIGRO INMEDIATO.
// - EL BONUS DE "AVANZAR" (val += col * 2) HACE QUE LA IA SIEMPRE
//   QUIERA MOVERSE HACIA LA IZQUIERDA, AUNQUE NO SEA LO MEJOR.
// ============================================================
int MinimaxTablero::evaluar(const Tablero& tablero) const
{
    int puntuacion = 0;

    // VALOR DE CADA TIPO DE PIEZA (CUÁNTO "VALE" TENERLA)
    auto valorPieza = [](TipoPieza tipo) -> int {
        switch (tipo) {
        case pieza_esfera:     return 1000; // REY/EMIR: perderlo = perder la partida
        case pieza_dodecaedro: return   90; // INFILTRADO
        case pieza_icosaedro:  return   85; // ALMOGAVAR
        case pieza_tetraedro:  return   60; // CABALLERIA LIGERA
        case pieza_cubog:      return   70; // INFANTERIA PESADA
        case pieza_cono:       return   75; // CABALLERIA PESADA
        case pieza_cilindro:   return   60; // BALLESTERO
        case pieza_cubo_p:     return   30; // MILICIANO: la pieza más débil
        default:               return    0;
        }
        };

    // POSICIONES DE LOS 5 PUNTOS DE PODER DEL TABLERO
    const int poderPos[5][2] = { {0,4},{4,0},{4,4},{4,8},{8,4} };

    for (int f = 0; f < Tablero::N; f++) {
        for (int c = 0; c < Tablero::N; c++) {
            const Casilla& cas = tablero.getCasilla(f, c);
            if (cas.pieza == pieza_nada) continue;

            int val = valorPieza(cas.pieza); // VALOR BASE DE LA PIEZA

            // AJUSTAMOS EL VALOR SEGÚN LA VIDA RESTANTE
            // UNA PIEZA AL 50% DE VIDA VALE LA MITAD
            if (cas.obj != nullptr && cas.obj->getVidaMax() > 0) {
                val = (val * cas.obj->getVida()) / cas.obj->getVidaMax();
                val += cas.obj->getFuerza(); // PIEZAS MÁS FUERTES VALEN MÁS
            }

            // BONUS SI ESTÁ EN UN PUNTO DE PODER
            if (cas.tipo == Casilla_poder) val += 50;

            // BONUS POR CERCANÍA A PUNTOS DE PODER (DISTANCIA MANHATTAN)
            // UNA PIEZA A 1 CASILLA DE UN PUNTO DE PODER RECIBE +9
            // UNA PIEZA A 5 CASILLAS RECIBE +5
            // UNA PIEZA A 10 O MÁS CASILLAS NO RECIBE NADA
            for (auto& pp : poderPos) {
                int dist = abs(f - pp[0]) + abs(c - pp[1]);
                int bonus = std::max(0, 10 - dist);
                if (cas.bando == bando_rival) puntuacion += bonus; // BUENO PARA IA
                if (cas.bando == bando_local) puntuacion -= bonus; // MALO PARA IA
            }

            // BONUS PARA LA IA POR AVANZAR HACIA EL JUGADOR (COLUMNAS BAJAS)
            // ESTO HACE QUE LA IA TIENDA A MOVERSE HACIA LA IZQUIERDA
            if (cas.bando == bando_rival) {
                val += (Tablero::N - 1 - c) * 2; // MÁS BONUS CUANTO MÁS A LA IZQ
                puntuacion += val;
            }
            if (cas.bando == bando_local) {
                puntuacion -= val; // LAS PIEZAS DEL JUGADOR RESTAN A LA IA
            }
        }
    }

    return puntuacion;
}

// ============================================================
// ALGORITMO MINIMAX CON PODA ALPHA-BETA
// 
// ESTE ES EL CORAZÓN DE LA IA. FUNCIONA ASÍ:
// 
// IMAGINA UN ÁRBOL DONDE CADA NODO ES UN ESTADO DEL TABLERO:
//   - NIVEL 0 (RAÍZ): POSICIÓN ACTUAL
//   - NIVEL 1: TODOS LOS MOVIMIENTOS POSIBLES DE LA IA
//   - NIVEL 2: TODAS LAS RESPUESTAS POSIBLES DEL JUGADOR
//   - NIVEL 3: TODOS LOS MOVIMIENTOS SIGUIENTES DE LA IA
//   - ... Y ASÍ HASTA LA PROFUNDIDAD MÁXIMA
// 
// EN LOS NIVELES PARES (MAXIMIZAR=TRUE): LA IA ELIGE EL MOVIMIENTO
//   QUE LE DA LA PUNTUACIÓN MÁS ALTA (la IA juega lo mejor posible)
// 
// EN LOS NIVELES IMPARES (MAXIMIZAR=FALSE): EL JUGADOR ELIGE EL
//   MOVIMIENTO QUE LE DA LA PUNTUACIÓN MÁS BAJA PARA LA IA
//   (ASUMIMOS QUE EL JUGADOR TAMBIÉN JUEGA LO MEJOR POSIBLE)
// 
// PODA ALPHA-BETA:
//   ALPHA = LA MEJOR PUNTUACIÓN QUE LA IA YA SABE QUE PUEDE CONSEGUIR
//   BETA  = LA MEJOR PUNTUACIÓN QUE EL JUGADOR YA SABE QUE PUEDE CONSEGUIR
//   SI BETA <= ALPHA: NO HACE FALTA EXPLORAR MÁS ESTA RAMA
//   (EL JUGADOR NUNCA ELEGIRÍA IR POR AQUÍ PORQUE YA TIENE ALGO MEJOR)
//   ESTO PUEDE PODAR HASTA EL 70% DEL ÁRBOL EN CASOS FAVORABLES.
// 
// POR QUÉ LA IA PUEDE SER TONTA:
// - CON PROFUNDIDAD 1 SOLO VE SU SIGUIENTE JUGADA, NO LAS CONSECUENCIAS
// - CON PROFUNDIDAD 2 VE TU RESPUESTA PERO NO SU SIGUIENTE JUGADA
// - LA FUNCIÓN evaluar() SOLO MIRA MATERIAL Y POSICIÓN, NO AMENAZAS
// ============================================================
int MinimaxTablero::minimax(Tablero& tablero, int profundidad,
    bool maximizar, int alpha, int beta)
{
    // CASO BASE: PROFUNDIDAD AGOTADA O HAY UN GANADOR
    // EVALUAMOS LA POSICIÓN Y DEVOLVEMOS EL NÚMERO
    if (profundidad == 0 || hayVictoria(tablero))
        return evaluar(tablero);

    // GENERAMOS TODOS LOS MOVIMIENTOS POSIBLES PARA EL BANDO ACTUAL
    BandoPieza bando = maximizar ? bando_rival : bando_local;
    auto movimientos = generarMovimientos(tablero, bando);

    // SI NO HAY MOVIMIENTOS POSIBLES, EVALUAMOS LA POSICIÓN ACTUAL
    if (movimientos.empty()) return evaluar(tablero);

    // VALOR DE PIEZAS PARA ORDENAR MOVIMIENTOS (VER ABAJO)
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

    // OPTIMIZACIÓN: ORDENAMOS LOS MOVIMIENTOS PARA PROBAR PRIMERO LAS CAPTURAS
    // SI PROBAMOS PRIMERO LOS MOVIMIENTOS MÁS PROMETEDORES, LA PODA ALPHA-BETA
    // PUEDE ELIMINAR MÁS RAMAS Y EL ALGORITMO ES MÁS RÁPIDO
    std::sort(movimientos.begin(), movimientos.end(),
        [&tablero, &valorPieza](const MovimientoIA& a, const MovimientoIA& b) {
            int valA = valorPieza(tablero.getCasilla(a.filaDestino, a.colDestino).pieza);
            int valB = valorPieza(tablero.getCasilla(b.filaDestino, b.colDestino).pieza);
            return valA > valB; // CAPTURAS MÁS VALIOSAS PRIMERO
        });

    PilaHistorial<EstadoCasilla> historial;

    if (maximizar) {
        // TURNO DE LA IA: BUSCA EL MOVIMIENTO QUE MAXIMIZA SU PUNTUACIÓN
        int mejorValor = INT_MIN; // EMPEZAMOS EN -INFINITO

        for (const auto& mov : movimientos) {
            // SIMULAMOS EL MOVIMIENTO EN LA COPIA
            EstadoCasilla estado = aplicarMovimiento(tablero, mov);
            historial.guardar(estado);

            // LLAMADA RECURSIVA: AHORA ES EL TURNO DEL JUGADOR (maximizar=false)
            int valor = minimax(tablero, profundidad - 1, false, alpha, beta);

            // DESHACEMOS EL MOVIMIENTO PARA PROBAR EL SIGUIENTE
            deshacerMovimiento(tablero, mov, historial.recuperar());

            // BONUS EXTRA SI ESTE MOVIMIENTO CAPTURÓ UNA PIEZA ENEMIGA
            // (INCENTIVA A LA IA A COMER PIEZAS)
            if (estado.pieza != pieza_nada && estado.bando == bando_local) {
                valor += valorPieza(estado.pieza) * 2;
            }

            mejorValor = std::max(mejorValor, valor);
            alpha = std::max(alpha, valor);

            // PODA BETA: EL JUGADOR YA TIENE UNA OPCIÓN MEJOR, NO SEGUIMOS
            if (beta <= alpha) break;
        }
        return mejorValor;
    }
    else {
        // TURNO DEL JUGADOR: ASUMIMOS QUE JUGARÁ LO MEJOR POSIBLE CONTRA LA IA
        // BUSCA EL MOVIMIENTO QUE MINIMIZA LA PUNTUACIÓN DE LA IA
        int mejorValor = INT_MAX; // EMPEZAMOS EN +INFINITO

        for (const auto& mov : movimientos) {
            EstadoCasilla estado = aplicarMovimiento(tablero, mov);
            historial.guardar(estado);

            // LLAMADA RECURSIVA: AHORA ES EL TURNO DE LA IA (maximizar=true)
            int valor = minimax(tablero, profundidad - 1, true, alpha, beta);

            deshacerMovimiento(tablero, mov, historial.recuperar());

            // PENALIZACIÓN SI EL JUGADOR CAPTURA UNA PIEZA DE LA IA
            // (HACE QUE LA IA INTENTE PROTEGER SUS PIEZAS)
            if (estado.pieza != pieza_nada && estado.bando == bando_rival) {
                valor -= valorPieza(estado.pieza) / 2;
            }

            mejorValor = std::min(mejorValor, valor);
            beta = std::min(beta, valor);

            // PODA ALPHA: LA IA YA TIENE UNA OPCIÓN MEJOR, NO SEGUIMOS
            if (beta <= alpha) break;
        }
        return mejorValor;
    }
}

// ============================================================
// MÉTODO PRINCIPAL: CALCULA EL MEJOR MOVIMIENTO PARA LA IA
// 
// ES EL PUNTO DE ENTRADA QUE LLAMA EL COORDINADOR.
// TRABAJA SOBRE UNA COPIA DEL TABLERO PARA NO TOCAR EL ORIGINAL.
// 
// PROCESO:
// 1. CREA UNA COPIA DEL TABLERO ACTUAL
// 2. GENERA TODOS LOS MOVIMIENTOS POSIBLES PARA LA IA
// 3. PARA CADA MOVIMIENTO, LLAMA A minimax() PARA VER CUÁNTO VALE
// 4. DEVUELVE EL MOVIMIENTO CON LA PUNTUACIÓN MÁS ALTA
// ============================================================
MovimientoIA MinimaxTablero::calcularMejorMovimiento(Tablero& tablero)
{
<<<<<<< Updated upstream
    // SILENCIA PRINTS DURANTE EL CÁLCULO
=======
    // SILENCIAMOS LOS PRINTS DEL TABLERO DURANTE EL CÁLCULO
    // (EVITA QUE SE LLENE LA CONSOLA CON "Movido: (x,y) -> (x,y)")
>>>>>>> Stashed changes
    std::streambuf* bufOriginal = std::cout.rdbuf(nullptr);

    MovimientoIA mejorMov;
    int mejorValor = INT_MIN;

<<<<<<< Updated upstream
    auto movimientos = generarMovimientos(tablero, bando_rival);

    for (const auto& mov : movimientos) {
        EstadoCasilla estado = aplicarMovimiento(tablero, mov);
        int valor = minimax(tablero, _profundidad - 1, false, INT_MIN, INT_MAX);
        deshacerMovimiento(tablero, mov, estado);
=======
    // COPIA DEL TABLERO: EL MINIMAX TRABAJA AQUÍ, EL TABLERO REAL NO SE TOCA
    // LA COPIA COMPARTE LOS PUNTEROS DE PIEZAS PERO NO LES LLAMA setPosicion()
    Tablero copia = tablero;

    auto movimientos = generarMovimientos(copia, bando_rival);

    for (const auto& mov : movimientos) {
        // PROBAMOS EL MOVIMIENTO EN LA COPIA
        EstadoCasilla estado = aplicarMovimiento(copia, mov);

        // EVALUAMOS TODAS LAS RESPUESTAS POSIBLES DEL JUGADOR (profundidad - 1)
        // MAXIMIZAR=FALSE PORQUE AHORA TOCA EL TURNO DEL JUGADOR
        int valor = minimax(copia, _profundidad - 1, false, INT_MIN, INT_MAX);

        // DESHACEMOS PARA PROBAR EL SIGUIENTE
        deshacerMovimiento(copia, mov, estado);
>>>>>>> Stashed changes

        // SI ESTE MOVIMIENTO ES MEJOR QUE TODOS LOS ANTERIORES, LO GUARDAMOS
        if (valor > mejorValor) {
            mejorValor = valor;
            mejorMov = mov;
        }
    }

<<<<<<< Updated upstream
    // RESTAURA PRINTS
=======
    // RESTAURAMOS LOS PRINTS Y MOSTRAMOS EL MOVIMIENTO ELEGIDO
>>>>>>> Stashed changes
    std::cout.rdbuf(bufOriginal);
    std::cout << "[IA] Movimiento: ("
        << mejorMov.filaOrigen << "," << mejorMov.colOrigen << ") -> ("
        << mejorMov.filaDestino << "," << mejorMov.colDestino << ")\n";

    return mejorMov;
}