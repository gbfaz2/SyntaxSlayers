// MOTOR GRÁFICO CENTRALIZADO — ORQUESTADOR PRINCIPAL
// DELEGA EL DIBUJO A: DibujaMenu, DibujaTablero, DibujaArena

#pragma once
#include "freeglut.h"

class Dibuja {
protected:
    static int _anchoVentana;
    static int _altoVentana;

public:
    // UTILIDADES OPENGL COMPARTIDAS POR TODAS LAS PANTALLAS 
    static void util_entrar2D(int ancho, int alto);
    static void util_salir2D();
};