#pragma once
#include "estadojuego.h"
#include "menu.h"
#include "pantallabatalla.h"
#include "tablerogl.h"

class Coordinador; // AVISO: ESTA CLASE EXISTE, SE DEFINE EN OTRO SITIO

// GESTIONA TODO EL INPUT DEL JUEGO SEGUN EL ESTADO ACTUAL
class GestorInput
{
    int _ancho{ 800 }; // ANCHO VENTANA
    int _alto{ 600 };  // ALTO VENTANA
    Tablerogl* _tablerogl{ nullptr }; // PUNTERO AL TABLERO GL

public:
    GestorInput() = default;

    void setVentana(int ancho, int alto) { _ancho = ancho; _alto = alto; } // ACTUALIZA TAMAÑO VENTANA
    void setTablerogl(Tablerogl* t) { _tablerogl = t; } // ASIGNA TABLEROGL
    Coordinador* _coordinador{ nullptr }; // PUNTERO AL COORDINADOR
    void setCoordinador(Coordinador* c) { _coordinador = c; } // SETTER

    // TECLA NORMAL
    void teclaMenu(unsigned char key, EstadoJuego& estado,
        PantallaIntro& intro, MenuPrincipal& menu, PantallaDestino& destino);
    void teclaTablero(unsigned char key, EstadoJuego& estado);
    void teclaArena(unsigned char key);


    // TECLA ESPECIAL
    void teclaEspecialMenu(int key, EstadoJuego& estado, MenuPrincipal& menu);
    void teclaEspecialTablero(int key);
    void teclaUpArena(unsigned char key);
    void teclaEspecialArena(int key);
    void teclaEspecialUpArena(int key);


    // RATON
    void ratonMenu(int boton, int state, int x, int y, EstadoJuego& estado,
        PantallaIntro& intro, MenuPrincipal& menu, PantallaDestino& destino);
    void ratonMovidoMenu(int x, int y, EstadoJuego& estado, MenuPrincipal& menu);
    void ratonTablero(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey);
};