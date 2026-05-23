

// Autor: Ines Alcérreca Sánchez
// Pantallas E_INTRO y E_MENU 

#pragma once
#include "estadojuego.h"
#include <string>
#include "dibuja.h"

class DibujaMenu; // DECLARACION ANTICIPADA PARA LA AMISTAD

//  PantallaIntro  (E_INTRO)
class PantallaIntro {
public:
    PantallaIntro();

    void saltar();

    bool terminado() const { return m_terminado; }
    void reiniciar();

private:
    int  m_fotograma;
    bool m_terminado;

    static const int DURACION = 250; // fotogramas (aprox 4 segundos a 60 fps)

    friend class DibujaMenu; // PERMITE A DIBUJAMENU LEER VARIABLES PRIVADAS

};


//  MenuPrincipal  (E_MENU)
class MenuPrincipal {
public:
    MenuPrincipal();

    void dibujar(int ancho, int alto);

    bool          terminado()    const { return m_terminado; }
    EstadoJuego   siguienteEstado() const { return m_siguiente; }
    ConfigPartida getConfiguracion() const { return m_cfg; }

    void reiniciar();

    friend class GestorInput; // ACCESO TOTAL AL GESTOR DE INPUT
    friend class DibujaMenu; // PERMITE A DIBUJAMENU LEER VARIABLES PRIVADAS

private:
    int           m_paso;       // 0=modo, 1=bando, 2=batalla, 3=confirmar
    int           m_seleccion;  // opcion resaltada
    int           m_fotograma;
    bool          m_terminado;
    EstadoJuego   m_siguiente;
    ConfigPartida m_cfg;

    void confirmar();
    int maxOpciones() const { return (m_paso == 1) ? 2 : 4; }
};
