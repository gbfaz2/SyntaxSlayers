#pragma once
#include "Tablero.h"

// GESTOR DE VICTORIA
// Comprueba en cada turno si se cumple alguna de las condiciones de victoria:
// 1. DOMINIO      -> un bando controla los 5 puntos de poder a la vez
// 2. ANIQUILACION -> un bando elimina todas las tropas enemigas
// 3. ASEDIO FINAL -> el rival solo tiene 1 pieza y está rodeada de enemigos

// Resultado posible tras comprobar victoria
enum class ResultadoVictoria {
    SIN_GANADOR,    // la partida continúa
    GANA_LOCAL,     // gana el bando local (cristiano)
    GANA_RIVAL,     // gana el bando rival (musulmán)
    EMPATE          // caso excepcional: los dos pierden a la vez
};

class GestorVictoria {
public:
    // Método público al que llamar desde el main al final de cada turno
    // Comprueba las 3 condiciones y devuelve el resultado en forma de ResultadoVictoria
    ResultadoVictoria comprobarVictoria(const Tablero& tablero) const;

private:
    //Métodos privados:
    //Comprueba DOMINIO
    ResultadoVictoria comprobarDominio(const Tablero& tablero) const;

    //Comprueba ANIQUILACIÓN
    ResultadoVictoria comprobarAniquilacion(const Tablero& tablero) const;

    //Comprueba ASEDIO FINAL
    ResultadoVictoria comprobarAsedio(const Tablero& tablero) const;

    //Método auxiliar para contar piezas
    int contarPiezas(const Tablero& tablero, BandoPieza bando) const;

    //Método auxiliar para comprobar si pieza en (fila,col) está completamente rodeada de enemigos
    bool estaAcorralada(const Tablero& tablero, int fila, int col) const;
};