
// Autor: Ines Alcérreca Sánchez
// Pantalla DESTINO: fondo animado + contexto historico de la batalla sorteada

#pragma once
#include "estadojuego.h"
#include <vector>
#include <string>
#include "ETSIDI.h"

struct Particula {
    float x, y, vx, vy;
    float r, g, b, alfa;
    float vida, vidaMax, tam;
};

class PantallaDestino {
    ConfigPartida            m_cfg;
    int                      m_fotograma = 0;
    bool                     m_terminado = false;
    int                      m_letraVisible = 0;
    int                      m_totalLetras = 0;
    std::vector<Particula>   m_particulas;
    std::vector<std::string> m_lineas;

    void actualizar(int ancho, int alto);
    void emitir(int ancho, int alto);
    void dibujarFondo(int ancho, int alto);
    void dibujarEfectos(int ancho, int alto);
    void dibujarParticulas();
    void dibujarTextos(int ancho, int alto);
    void dibujarContinuar(int ancho, int alto);

    void  coloresFondo(float& r1, float& g1, float& b1,
        float& r2, float& g2, float& b2) const;
    void  colorParticula(float& r, float& g, float& b, float variacion) const;
    float alfaEntrada() const;
    bool  textoCompleto() const;

    ETSIDI::GLTexture m_textura = { 0, 0, 0 }; //para el fondo animado 

public:
    PantallaDestino() = default;
    void reiniciar(const ConfigPartida& cfg);
    void dibujar(int ancho, int alto);
    void avanzar();
    bool terminado() const { return m_terminado; }
};
