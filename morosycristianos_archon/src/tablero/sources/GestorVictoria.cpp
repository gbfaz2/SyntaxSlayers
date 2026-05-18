#include "GestorVictoria.h"
#include <iostream>


// MÉTODO PRINCIPAL 
// Comprueba las 3 condiciones en orden de prioridad
ResultadoVictoria GestorVictoria::comprobarVictoria(const Tablero& tablero) const
{
    ResultadoVictoria resultado = comprobarDominio(tablero); //comprueba el dominio lo primero

    //Si hay ganador (!=SIN_GANADOR) por dominio, devuelve el resultado 
    if (resultado != ResultadoVictoria::SIN_GANADOR) {
        std::cout << "[GestorVictoria] Victoria por DOMINIO!" << std::endl;
        return resultado; //devuelve el resultado obtenido con comprobardominio
    }

    //Comrpueba aniquilación, si hay ganador (!=SIN_GANADOR) devuelve el resultado  
    resultado = comprobarAniquilacion(tablero);
    if (resultado != ResultadoVictoria::SIN_GANADOR) {
        std::cout << "[GestorVictoria] Victoria por ANIQUILACION!" << std::endl;
        return resultado;  
    }

    //Comprueba asedio, si hayg anador (!=SIN_GANADOR) devuelve el resultado
    resultado = comprobarAsedio(tablero);
    if (resultado!= ResultadoVictoria::SIN_GANADOR) {
        std::cout << "[GestorVictoria] Victoria por ASEDIO FINAL!" << std::endl;
        return resultado;
    }
    
    //finalmente si el resultado de resultado es sin ganador (único caso no definido) entonces retorna eso
    return ResultadoVictoria::SIN_GANADOR;
}

// CONDICIÓN 1 —> DOMINIO
// Recorre todo el tablero buscando casillas de tipo Casilla_poder
// y cuenta cuántas tiene cada bando
ResultadoVictoria GestorVictoria::comprobarDominio(const Tablero& tablero) const
{
    //inicializacion puntos de cada bando
    int puntosLocal = 0;
    int puntosRival = 0;

    //bucle para recorrer todo el tablero
    for (int fila = 0; fila < Tablero::N; fila++) {
        for (int col = 0; col < Tablero::N; col++) {

            //si casilla distitna a la de poder, pasa de ella y sigue el bucle
            if (tablero.getCasilla(fila, col).tipo != Casilla_poder)
                continue;

            //Si casilla=casilla poder, escanea la casilla
            BandoPieza bando = tablero.getCasilla(fila, col).bando;

            //anota puntos segun bando recibido
            if (bando == bando_local) puntosLocal++;
            if (bando == bando_rival) puntosRival++;
        }
    }

    //cuando uno de los dos bandos llegue a 5, ganador
    if (puntosLocal == 5) return ResultadoVictoria::GANA_LOCAL;
    if (puntosRival == 5) return ResultadoVictoria::GANA_RIVAL;

    //por seguridad, devuelve siempre sin ganador 
    return ResultadoVictoria::SIN_GANADOR;
}

// CONDICIÓN 2 — ANIQUILACIÓN
// Un bando gana si el rival se queda sin piezas en el tablero
ResultadoVictoria GestorVictoria::comprobarAniquilacion(const Tablero& tablero) const
{
    //cuenta piezas con el metodo auxiliar
    int piezasLocal = contarPiezas(tablero, bando_local);
    int piezasRival = contarPiezas(tablero, bando_rival);

    //ambos bandos se quedan sin piezas a la vez
    if (piezasLocal == 0 && piezasRival == 0)
        return ResultadoVictoria::EMPATE;

    //gana rival
    if (piezasLocal == 0)
        return ResultadoVictoria::GANA_RIVAL;

    //gana local
    if (piezasRival == 0)
        return ResultadoVictoria::GANA_LOCAL;

    return ResultadoVictoria::SIN_GANADOR;
}


// CONDICIÓN 3 — ASEDIO FINAL
// Un bando gana si el rival solo tiene 1 pieza y está completamente rodeada de enemigos
ResultadoVictoria GestorVictoria::comprobarAsedio(const Tablero& tablero) const
{
    //cuenta las piezas de cada bando
    int piezasLocal = contarPiezas(tablero, bando_local);
    int piezasRival = contarPiezas(tablero, bando_rival);

    // Caso ningúno tiene piezas: no hay asedio posible
    if (piezasLocal != 1 && piezasRival != 1)
        return ResultadoVictoria::SIN_GANADOR;

    // Caso especial: los dos tienen 1 pieza a la vez
    if (piezasLocal == 1 && piezasRival == 1) {
        
        //declaras variables e inicializas
        bool localAcorralado = false;
        bool rivalAcorralado = false;

        //bucle que recorre tablero
        for (int fila = 0; fila < Tablero::N; fila++) {
            for (int col = 0; col < Tablero::N; col++) {
                
                //escanea bando casilla actual
                BandoPieza b = tablero.getCasilla(fila, col).bando;
                
                //si bando esta acorralado, entonces acorralado=true
                if (b == bando_local && estaAcorralada(tablero, fila, col))
                    localAcorralado = true;
                if (b == bando_rival && estaAcorralada(tablero, fila, col))
                    rivalAcorralado = true;
            }
        }

        //declaraciones victoria con condiciones del bucle
        if (localAcorralado && rivalAcorralado)
            return ResultadoVictoria::EMPATE;       // los dos acorralados a la vez
        if (localAcorralado)
            return ResultadoVictoria::GANA_RIVAL;   // local acorralado → gana rival
        if (rivalAcorralado)
            return ResultadoVictoria::GANA_LOCAL;   // rival acorralado → gana local

        return ResultadoVictoria::SIN_GANADOR;      // ninguno acorralado
    }

    // Caso normal: solo uno de los dos tiene 1 pieza

    //Combinaciones restantes posibles:
    //piezasLocal == 1 && piezasRival != 1  → local es el débil
    //piezasLocal != 1 && piezasRival == 1  → rival es el débil
    //bando debil aquel que solo tenga 1 pieza

    BandoPieza bandoDebil = (piezasLocal == 1) ? bando_local : bando_rival;

    //recorre todo el tablero
    for (int fila = 0; fila < Tablero::N; fila++) {
        for (int col = 0; col < Tablero::N; col++) {
            //si la casilla pertenece al bando debil
            if (tablero.getCasilla(fila, col).bando == bandoDebil) {
                //si dicha ficha esta acorralada, ya cumple las dos condiciones
                if (estaAcorralada(tablero, fila, col)) {
                    //resultado victoria segun bando debil establecido
                    return (bandoDebil == bando_local) ?
                        ResultadoVictoria::GANA_RIVAL :
                        ResultadoVictoria::GANA_LOCAL;
                }
            }
        }
    }

    //seguridad
    return ResultadoVictoria::SIN_GANADOR;
}

//metodo auxiliar: cuenta piezas de un bando en el tablero
int GestorVictoria::contarPiezas(const Tablero& tablero, BandoPieza bando) const
{
    //variable de cuenta
    int count = 0;
    
    //bucle que recorre todo el tablero
    for (int fila = 0; fila < Tablero::N; fila++)
        for (int col = 0; col < Tablero::N; col++)
            //si el bando de la casilla = bando metido en el metodo
            if (tablero.getCasilla(fila, col).bando == bando)
                count++;
    return count;
}

// metodo auxiliar: comprueba si la pieza en (fila,col) está rodeada de enemigos
// Mira las 4 casillas adyacentes (arriba, abajo, izquierda, derecha)
// Si todas tienen enemigo o están fuera del tablero → acorralada
bool GestorVictoria::estaAcorralada(const Tablero& tablero, int fila, int col) const
{

    //variable bandopropio de la casilla actual
    BandoPieza bandoPropio = tablero.getCasilla(fila, col).bando;

    //array que guarda las direcciones que vamos a usar para buscar
    int dirs[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    //bucle sumando el desplazamiento a la casilla actual
    for (int d = 0; d < 4; d++) {
        int f = fila + dirs[d][0];
        int c = col + dirs[d][1];


        // Fuera del tablero: cuenta como bloqueada
        if (f < 0 || f >= Tablero::N || c < 0 || c >= Tablero::N)
            continue;

        // Casilla libre o aliada: no está acorralada, libre
        BandoPieza bandoCasilla = tablero.getCasilla(f, c).bando;
        if (bandoCasilla == bando_nada || bandoCasilla == bandoPropio)
            return false;
    }

    return true;
}