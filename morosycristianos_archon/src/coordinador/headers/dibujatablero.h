
// DIBUJATABLERO — MOTOR GRÁFICO EXCLUSIVO PARA RENDERIZAR EL TABLERO Y SUS PIEZAS

#pragma once
#include "dibuja.h"
#include "tablerogl.h"
#include "dibujapersonajes.h"


class DibujaTablero : public Dibuja {
   
    friend class Tablerogl;
    static dibujapersonajes _dibujador;


    // CAPAS BASE DEL TABLERO
    static void tablero_fondo(const Tablerogl& t);
    static void tablero_casillas(const Tablerogl& t);
    static void tablero_casilla_individual(const Tablerogl& t, int fila, int col);
    static void tablero_color_casilla(const Tablerogl& t, int fila, int col);
    static void tablero_marco(const Tablerogl& t);

    // DECORACIÓN Y HUD (SÍMBOLOS, CURSORES, SELECCIÓN)
    static void tablero_simbolos(const Tablerogl& t);
    static void tablero_cruz(float cx, float cy, float size);
    static void tablero_luna(float cx, float cy, float size);
    static void tablero_punto_poder(float cx, float cy, float size);

    static void tablero_cursores(const Tablerogl& t);
    static void tablero_seleccion(const Tablerogl& t);
    static void tablero_movimientos_validos(const Tablerogl& t);
    static void tablero_casilla_resaltada(const Tablerogl& t, int fila, int col, float r, float g, float b, float lw, float z);

    // RENDERIZADO 3D DE LAS PIEZAS 
    static void tablero_piezas(Tablerogl& t);
    static void tablero_pieza_individual(Tablerogl& t, int fil, int col);
    //CONTADORES DE TIEMPO Y CARTEL DE MOVIMIENTO INVALIDO
    static void tablero_contadores(const Tablerogl& t);
    static void tablero_mensaje_invalido(const Tablerogl& t);
    //PANEL DE CARACTERÍSTICAS DE LAS PIEZAS
    static void tablero_panel_pieza(const Tablerogl& t);
    static void tablero_barra(int x, int y, int ancho, int alto, float valor, float maximo, float r, float g, float b);
    //PANEL DE HECHIZOS (SOLO PARA REYES)
    static void tablero_panel_hechizos(const Tablerogl& t);
    static void tablero_panel_infiltrado(const Tablerogl& t);
    int  _contadorMiliciano{ 0 };


public:
    // INICIALIZACIÓN DE OPENGL PARA EL TABLERO
    static void tablero_init();
    
    // ORQUESTADORES PRINCIPALES 
    static void tablero_dibujar(Tablerogl& t); // DIBUJA EL TABLERO COMPLETO DURANTE LA PARTIDA
    static void tablero_victoria(const Tablerogl& t); // DIBUJA LA PANTALLA FINAL DE GANADOR

    static void tablero_guardando(Tablerogl& t, int ancho, int alto, float tiempoRestante);
};