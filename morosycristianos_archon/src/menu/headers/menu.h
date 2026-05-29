// Autor: Ines Alcérreca Sánchez
// Pantallas INTRO y MENU

#pragma once
#include "estadojuego.h"
#include <string>
#include "dibuja.h"
#include "GestorPartida.h"

class DibujaMenu; // DECLARACION ANTICIPADA

// ============================================================
// PANTALLA INTRO
// ============================================================

class PantallaIntro {
    int  m_fotograma;
    bool m_terminado;

    static const int DURACION = 420; // ~7 segundos a 60 fps

    friend class DibujaMenu;

public:
    PantallaIntro();
    void saltar();
    bool terminado() const { return m_terminado; }
    void reiniciar();
};

// ============================================================
// MENÚ PRINCIPAL
// ============================================================

class MenuPrincipal {
    // Paso actual del menú:
    // 0 = menú principal  (Cargar / Nueva / Ayuda / Ranking / Salir)
    // 1 = modo de juego   (JvJ / JvIA)
    // 2 = configuración   (nombre J1, bando, nombre J2 o dificultad)
    // 3 = batalla         (4 opciones)
    // 4 = confirmar       (JUGAR / VOLVER)
    int           m_paso;
    int           m_seleccion;
    int           m_fotograma;
    bool          m_terminado;
    EstadoJuego   m_siguiente;
    ConfigPartida m_cfg;

    // Texto que se está escribiendo en los campos de nombre
    std::string   m_nombreActual;   // compatibilidad (no se usa activamente)
    std::string   m_nombreJ1Actual;
    std::string   m_nombreJ2Actual;
    int           m_focoNombre;     // 0 = J1 tiene foco, 1 = J2 tiene foco

    void confirmar();
    void teclaTexto(unsigned char key);

    int maxOpciones() const {
        switch (m_paso) {
        case 0: return 5;  // Cargar / Nueva / Ayuda / Ranking / Salir
        case 1: return 2;  // JvJ / JvIA
        case 2: return 0;  // sin selector numérico (campos de texto + botón Continuar)
        case 3: return 4;  // batallas
        case 4: return 2;  // JUGAR / VOLVER
        default: return 0;
        }
    }

public:
    MenuPrincipal();

    bool          terminado()       const { return m_terminado; }
    EstadoJuego   siguienteEstado() const { return m_siguiente; }
    ConfigPartida getConfiguracion()const { return m_cfg; }

    void reiniciar();

    friend class GestorInput;
    friend class DibujaMenu;
};