// Autor: Ines Alcérreca Sánchez (Modificado por Gabri)
// Pantallas E_INTRO y E_MENU

#pragma once
#include "estadojuego.h"
#include <string>
#include "dibuja.h"
#include "GestorPartida.h"

class DibujaMenu; // DECLARACION ANTICIPADA PARA LA AMISTAD

class PantallaIntro {
    int  m_fotograma;
    bool m_terminado;
    static const int DURACION = 420; // ~7 segundos a 60fps
    friend class DibujaMenu;

public:
    PantallaIntro();
    void saltar();
    bool terminado() const { return m_terminado; }
    void reiniciar();
};

class MenuPrincipal {
    int m_paso;
    int m_seleccion;
    int m_fotograma;
    bool m_terminado;
    EstadoJuego m_siguiente;
    ConfigPartida m_cfg;
    std::string m_nombreJ1Actual;
    std::string m_nombreJ2Actual;
    int m_focoNombre;

    void confirmar();
    void teclaTextoJ1(unsigned char key);
    void teclaTextoJ2(unsigned char key);

    int maxOpciones() const {
        switch (m_paso) {
        case 0: return 5;
        case 1: return 2;
        case 2: return 0;
        case 3: return 4;
        case 4: return 2;
        default: return 0;
        }
    }

    static void menu_paso0(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso1(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso2(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso3(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso4(MenuPrincipal& m, int ancho, int alto);

public:
    MenuPrincipal();
    void dibujar(int ancho, int alto);
    bool          terminado()       const { return m_terminado; }
    EstadoJuego   siguienteEstado() const { return m_siguiente; }
    ConfigPartida getConfiguracion()const { return m_cfg; }
    void reiniciar();

    friend class GestorInput;
    friend class DibujaMenu;
};