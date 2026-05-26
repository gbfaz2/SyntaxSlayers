

// Autor: Ines Alcérreca Sánchez
// Pantallas E_INTRO y E_MENU 

#pragma once
#include "estadojuego.h"
#include <string>
#include "dibuja.h"
#include "GestorPartida.h"

class DibujaMenu; // DECLARACION ANTICIPADA PARA LA AMISTAD

//  PantallaIntro  (E_INTRO)
class PantallaIntro {
    int  m_fotograma;
    bool m_terminado;

    static const int DURACION = 250; // fotogramas (aprox 4 segundos a 60 fps)

    friend class DibujaMenu; // PERMITE A DIBUJAMENU LEER VARIABLES PRIVADAS

public:
    PantallaIntro();

    void saltar();

    bool terminado() const { return m_terminado; }
    void reiniciar();
};


//  MenuPrincipal  (E_MENU)
class MenuPrincipal {
    int           m_paso;       // 0=modo, 1=nombreJ1, 2=bando, 3=nombreJ2, 4=batalla, 5=confirmar
    int           m_seleccion;  // opcion resaltada
    int           m_fotograma;
    bool          m_terminado;
    EstadoJuego   m_siguiente;
    ConfigPartida m_cfg;
    std::string   m_nombreActual;

    void confirmar();
	void teclaTexto(unsigned char key); // para escribir los nombres de los jugadores
    int maxOpciones() const {
        switch (m_paso) {
        case 0: return 5;  // modo
        case 2: return 2;  // bando
        case 4: return 4;  // batalla
        case 5: return 2;  // confirmar
        default: return 0; // pasos de texto (1 y 3)
        }
    }
public:
    MenuPrincipal();

    void dibujar(int ancho, int alto);

    bool          terminado()    const { return m_terminado; }
    EstadoJuego   siguienteEstado() const { return m_siguiente; }
    ConfigPartida getConfiguracion() const { return m_cfg; }

    void reiniciar();

    friend class GestorInput; // ACCESO TOTAL AL GESTOR DE INPUT
    friend class DibujaMenu; // PERMITE A DIBUJAMENU LEER VARIABLES PRIVADAS
};
