
// Pantalla de destino historico: fondo animado por batalla

#define NOMINMAX 
#include "pantallabatalla.h"
#include "menu.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

static float fr() { return (float)rand() / RAND_MAX; }

void PantallaDestino::reiniciar(const ConfigPartida& cfg) {
    m_cfg = cfg;
    m_fotograma = 0;
    m_terminado = false;
    m_letraVisible = 0;
    m_particulas.clear();
    m_particulas.reserve(400);

    m_lineas = contextoBatalla(cfg.batalla);
    m_totalLetras = 0;
    for (const auto& l : m_lineas) m_totalLetras += (int)l.size();
}

void PantallaDestino::avanzar() {
    if (!textoCompleto()) {
        m_letraVisible = m_totalLetras;  // mostrar todo de golpe
        return;
    }
    m_terminado = true;
}

bool PantallaDestino::textoCompleto() const {
    return m_letraVisible >= m_totalLetras;
}

float PantallaDestino::alfaEntrada() const {
    return (m_fotograma < 60) ? m_fotograma / 60.0f : 1.0f;
}

// ESTA FUNCION SUSTITUYE AL ANTIGUO 'dibujar()' EN LA PARTE LÓGICA
void PantallaDestino::actualizar(int ancho, int alto) {
    m_fotograma++; // AVANZA EL TIEMPO

    if (m_fotograma > 60 && m_letraVisible < m_totalLetras)
        if (m_fotograma % 2 == 0) m_letraVisible++; // EFECTO MAQUINA DE ESCRIBIR

    for (auto& p : m_particulas) {
        p.x += p.vx;
        p.y += p.vy;
        p.vida -= 1.0f;
        p.alfa = (p.vida / p.vidaMax) * 0.80f;
    }

    auto muerta = [&](const Particula& p) {
        return p.vida <= 0 || p.x < -20 || p.x > ancho + 20
            || p.y < -20 || p.y > alto + 20;
        };

    m_particulas.erase(
        std::remove_if(m_particulas.begin(), m_particulas.end(), muerta),
        m_particulas.end());

    if ((int)m_particulas.size() < 250) emitir(ancho, alto); // GENERA NUEVAS SI FALTAN
}

void PantallaDestino::colorParticula(float& r, float& g, float& b, float v) const {
    switch (m_cfg.batalla) {
    case Batalla::GUADALETE:    // arena dorada
        r = 0.82f + v * 0.12f; g = 0.55f + v * 0.25f; b = 0.05f + v * 0.15f; break;
    case Batalla::ALARCOS:      // brasas rojas
        r = 0.90f + v * 0.10f; g = 0.25f + v * 0.35f; b = 0.01f;           break;
    case Batalla::NAVAS_TOLOSA: // polvo de luz
        r = 0.65f + v * 0.35f; g = 0.80f + v * 0.20f; b = 1.0f;            break;
    case Batalla::GRANADA:      // petalos dorados/rosados
        r = 0.90f + v * 0.10f; g = 0.60f + v * 0.20f; b = 0.15f + v * 0.45f; break;
    }
}

void PantallaDestino::emitir(int ancho, int alto) {
    for (int i = 0; i < 2; i++) {
        Particula p;
        float v = fr();
        colorParticula(p.r, p.g, p.b, v);
        p.vidaMax = 90.0f + fr() * 110.0f;
        p.vida = p.vidaMax;
        p.alfa = 0.7f;

        switch (m_cfg.batalla) {
        case Batalla::GUADALETE:    // arena soplada de izquierda a derecha
            p.x = -5.0f;
            p.y = fr() * alto;
            p.vx = 1.5f + fr() * 2.5f;
            p.vy = (fr() - 0.5f) * 0.6f;
            p.tam = 1.5f + fr() * 2.5f;
            break;
        case Batalla::ALARCOS:      // brasas ascendentes desde abajo
            p.x = ancho * (0.1f + fr() * 0.8f);
            p.y = fr() * alto * 0.25f;
            p.vx = (fr() - 0.5f) * 1.5f;
            p.vy = 1.0f + fr() * 2.5f;
            p.tam = 1.0f + fr() * 3.5f;
            break;
        case Batalla::NAVAS_TOLOSA: // polvo de luz cayendo desde arriba
            p.x = fr() * ancho;
            p.y = alto + 5.0f;
            p.vx = (fr() - 0.5f) * 0.8f;
            p.vy = -(0.5f + fr() * 1.2f);
            p.tam = 1.0f + fr() * 2.5f;
            break;
        case Batalla::GRANADA:      // petalos cayendo suavemente
            p.x = fr() * ancho;
            p.y = alto + 5.0f;
            p.vx = (fr() - 0.5f) * 1.0f;
            p.vy = -(0.3f + fr() * 1.0f);
            p.tam = 2.0f + fr() * 3.0f;
            break;
        }
        m_particulas.push_back(p);
    }
}
