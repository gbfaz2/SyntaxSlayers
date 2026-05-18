
// Autor: Ines Alcérreca Sánchez
// Pantalla de destino historico: fondo animado por batalla

#define NOMINMAX 
#include "pantallabatalla.h"
#include "menu.h"
#include "freeglut.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Necesita winmm.lib (se enlaza automaticamente con #pragma comment)


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

    // IMAGENES DE LAS BATALLAS
    const char* rutas[] = {
    "bin\\imagenes\\GUADALETE.png",
    "bin\\imagenes\\ALARCOS.png",
    "bin\\imagenes\\NAVAS_TOLOSA.png",
    "bin\\imagenes\\GRANADA.png"
    };
    m_textura = ETSIDI::getTexture(rutas[(int)cfg.batalla]);

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

void PantallaDestino::dibujar(int ancho, int alto) {
    m_fotograma++;
    actualizar(ancho, alto);
    entrar2D(ancho, alto);
    dibujarFondo(ancho, alto);
    dibujarEfectos(ancho, alto);
    dibujarParticulas();
    dibujarTextos(ancho, alto);
    dibujarContinuar(ancho, alto);
    salir2D();
}

// PARTICULAS

void PantallaDestino::actualizar(int ancho, int alto) {
    if (m_fotograma > 60 && m_letraVisible < m_totalLetras)
        if (m_fotograma % 2 == 0) m_letraVisible++;

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

    if ((int)m_particulas.size() < 250) emitir(ancho, alto);
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

void PantallaDestino::dibujarParticulas() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    for (const auto& p : m_particulas) {
        glPointSize(p.tam);
        glColor4f(p.r, p.g, p.b, p.alfa);
        glBegin(GL_POINTS);
        glVertex2f(p.x, p.y);
        glEnd();
    }
    glDisable(GL_POINT_SMOOTH);
    glPointSize(1.0f);
    glDisable(GL_BLEND);
}

// FONDO DEGRADADO

void PantallaDestino::coloresFondo(float& r1, float& g1, float& b1,
    float& r2, float& g2, float& b2) const {
    switch (m_cfg.batalla) {
    case Batalla::GUADALETE:    // desierto: naranja oscuro → marron
        r1 = 0.55f; g1 = 0.27f; b1 = 0.04f;
        r2 = 0.16f; g2 = 0.07f; b2 = 0.01f; break;
    case Batalla::ALARCOS:      // noche de batalla: negro → carmesi
        r1 = 0.04f; g1 = 0.01f; b1 = 0.01f;
        r2 = 0.36f; g2 = 0.04f; b2 = 0.02f; break;
    case Batalla::NAVAS_TOLOSA: // amanecer en Sierra Morena: azul → verde-azul
        r1 = 0.04f; g1 = 0.06f; b1 = 0.22f;
        r2 = 0.10f; g2 = 0.26f; b2 = 0.22f; break;
    case Batalla::GRANADA:      // atardecer: purpura → naranja calido
        r1 = 0.12f; g1 = 0.04f; b1 = 0.22f;
        r2 = 0.44f; g2 = 0.18f; b2 = 0.04f; break;
    }
}

void PantallaDestino::dibujarFondo(int ancho, int alto) {
    // Imagen de fondo 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textura.id);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, alto);
    glTexCoord2f(1, 0); glVertex2f(ancho, alto);
    glTexCoord2f(1, 1); glVertex2f(ancho, 0);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Capa oscura semitransparente para que el texto sea legible
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(ancho, 0);
    glVertex2f(ancho, alto);
    glVertex2f(0, alto);
    glEnd();
    glDisable(GL_BLEND);
}

// EFECTOS ESPECIALES

void PantallaDestino::dibujarEfectos(int ancho, int alto) {
    const float PI = 3.14159f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (m_cfg.batalla) {

    case Batalla::GUADALETE:
        glLineWidth(1.5f);
        for (int j = 0; j < 5; j++) {
            float yBase = alto * 0.10f + j * 16.0f;
            float fase = m_fotograma * 0.04f + j * 1.3f;
            glColor4f(0.85f, 0.55f, 0.10f, 0.12f);
            glBegin(GL_LINE_STRIP);
            for (int x = 0; x <= ancho; x += 10) {
                float y = yBase + sinf(x * 0.018f + fase) * 6.0f;
                glVertex2f((float)x, y);
            }
            glEnd();
        }
        glLineWidth(1.0f);
        break;

    case Batalla::ALARCOS:
        break;

    case Batalla::GRANADA:
        break;

    case Batalla::NAVAS_TOLOSA: {
        float cx = ancho * 0.5f;
        float ty = (float)alto + 10.0f;
        int nRayos = 10;
        for (int r = 0; r < nRayos; r++) {
            float t = (r / (float)(nRayos - 1)) - 0.5f;
            float angA = t * 1.4f;
            float angB = (t + 1.0f / (nRayos - 1)) * 1.4f;
            float pulse = (sinf(m_fotograma * 0.03f + r * 0.7f) + 1.0f) * 0.5f;
            glColor4f(1.0f, 0.95f, 0.70f, 0.05f + pulse * 0.07f);
            float len = ancho * 1.5f;
            glBegin(GL_TRIANGLES);
            glVertex2f(cx, ty);
            glVertex2f(cx + sinf(angA) * len, ty - cosf(angA) * len);
            glVertex2f(cx + sinf(angB) * len, ty - cosf(angB) * len);
            glEnd();
        }
        break;
    }
    }

    glDisable(GL_BLEND);
}


void PantallaDestino::dibujarTextos(int ancho, int alto) {
    float alfa = alfaEntrada();
    float brillo = (sinf(m_fotograma * 0.04f) + 1.0f) * 0.10f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Titulo (screen y=85 = openGL y=alto-85)
    ETSIDI::setFont("bin\\fuentes\\ARIALNBI.ttf", 24);
    ETSIDI::setTextColor((0.85f + brillo) * alfa,
        (0.70f + brillo * 0.5f) * alfa,
        0.10f * alfa, alfa);
    ETSIDI::printxy(nombreBatalla(m_cfg.batalla), ancho / 2 - 150, alto - 85);

    // Linea decorativa (sigue siendo OpenGL, no es texto)
    glColor4f(0.85f * alfa, 0.68f * alfa, 0.10f * alfa, 0.65f * alfa);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(ancho / 2.0f - 220.0f, alto - 112.0f);
    glVertex2f(ancho / 2.0f + 220.0f, alto - 112.0f);
    glEnd();
    glLineWidth(1.0f);

    // Contexto historico (maquina de escribir)
    ETSIDI::setFont("bin\\fuentes\\ARIALNBI.ttf", 18);
    int letrasRestantes = m_letraVisible;
    for (int i = 0; i < (int)m_lineas.size(); i++) {
        if (letrasRestantes <= 0) break;
        int lineLen = (int)m_lineas[i].size();
        int mostrar = std::min(letrasRestantes, lineLen);
        letrasRestantes -= lineLen;
        std::string visible = m_lineas[i].substr(0, mostrar);
        ETSIDI::setTextColor(0.92f, 0.88f, 0.80f, alfa);
        ETSIDI::printxy(visible.c_str(), 140, alto - 160 - i * 26);
    }

    glDisable(GL_BLEND);
}

void PantallaDestino::dibujarContinuar(int ancho, int alto) {
    if (!textoCompleto()) return;
    float parpadeo = (sinf(m_fotograma * 0.08f) + 1.0f) * 0.5f;
    ETSIDI::setFont("bin\\fuentes\\ARIALNBI.ttf", 12);
    ETSIDI::setTextColor(0.80f, 0.78f, 0.65f, 0.35f + parpadeo * 0.65f);
    ETSIDI::printxy("Pulsa cualquier tecla para comenzar la batalla",
        ancho / 2 - 230, 30);
}
