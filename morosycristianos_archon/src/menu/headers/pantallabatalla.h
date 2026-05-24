// Autor: Ines Alcérreca Sánchez
// Pantalla DESTINO (SOLO LÓGICA, CERO GRÁFICOS, CERO TEXTURAS)

#pragma once
#include "estadojuego.h"
#include <vector>
#include <string>

class DibujaMenu; // DECLARACION ANTICIPADA PARA LA AMISTAD CON EL MOTOR GRAFICO

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

    void emitir(int ancho, int alto); // CALCULA NACIMIENTO DE PARTICULAS
    void colorParticula(float& r, float& g, float& b, float variacion) const; // ASIGNA RGB LOGICO

    float alfaEntrada() const;
    bool  textoCompleto() const;

public:
    PantallaDestino() = default;

    void reiniciar(const ConfigPartida& cfg);
    void actualizar(int ancho, int alto); // GESTIONA LA LOGICA POR FRAME
    void avanzar();

    bool terminado() const { return m_terminado; }

    friend class DibujaMenu; // PERMITE A DIBUJAMENU LEER LA INFO PRIVADA
};