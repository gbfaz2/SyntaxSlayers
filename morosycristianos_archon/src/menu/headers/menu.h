

// Autor: Ines Alcérreca Sánchez
// Pantallas E_INTRO y E_MENU 

#pragma once
#include "estadojuego.h"
#include <string>


void dibujarTexto(const std::string& texto, float x, float y,
    float r, float g, float b, void* fuente = nullptr);

void dibujarTextoCentrado(const std::string& texto, float cx, float y,
    float r, float g, float b, void* fuente = nullptr);

void dibujarRectangulo(float x, float y, float ancho, float alto,
    float r, float g, float b, float alfa = 1.0f);

void dibujarBorde(float x, float y, float ancho, float alto,
    float r, float g, float b, float grosor = 2.0f);

void entrar2D(int ancho, int alto);
void salir2D();


//  PantallaIntro  (E_INTRO)
class PantallaIntro {
public:
    PantallaIntro();

    void dibujar(int ancho, int alto);
    void saltar();

    bool terminado() const { return m_terminado; }
    void reiniciar();

private:
    int  m_fotograma;
    bool m_terminado;

    static const int DURACION = 250; // fotogramas (aprox 4 segundos a 60 fps)
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

private:
    int           m_paso;       // 0=modo, 1=bando, 2=batalla, 3=confirmar
    int           m_seleccion;  // opcion resaltada
    int           m_fotograma;
    bool          m_terminado;
    EstadoJuego   m_siguiente;
    ConfigPartida m_cfg;

    void dibujarFondo(int ancho, int alto);
    void dibujarTitulo(int ancho, int alto);
    void dibujarPaso0(int ancho, int alto);   // seleccion de modo
    void dibujarPaso1(int ancho, int alto);   // seleccion de bando
    void dibujarPaso2(int ancho, int alto);   // seleccion de batalla
    void dibujarPaso3(int ancho, int alto);   // confirmacion
    void dibujarPie(int ancho, int alto);

    void dibujarOpcion(const std::string& texto, float x, float y,
        float ancho, float alto, bool seleccionada);

    void confirmar();
    int maxOpciones() const { return (m_paso == 1) ? 2 : 4; }
};
