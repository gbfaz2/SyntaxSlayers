// menu.cpp
// Autor: Ines Alcérreca Sánchez
// Solo usa OpenGL + freeglut, igual que el resto del proyecto

#include "menu.h"
#include "freeglut.h"
#include <cmath>
#include <string>

// ─────────────────────────────────────────────────────────────
//  Helpers globales
// ─────────────────────────────────────────────────────────────

void entrar2D(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);   // (0,0) = esquina inferior-izquierda
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void salir2D() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Dibuja texto con fuente bitmap de GLUT en posicion (x,y) OpenGL
void drawTexto(const std::string& texto, float x, float y,
               float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : texto)
        glutBitmapCharacter(fuente, c);
}

// Dibuja texto centrado horizontalmente
static void drawTextoCentrado(const std::string& texto, float cx, float y,
                               float r, float g, float b, void* fuente = nullptr) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    int ancho = glutBitmapLength((unsigned char*)fuente,
                                 (const unsigned char*)texto.c_str());
    drawTexto(texto, cx - ancho / 2.0f, y, r, g, b, fuente);
}

void drawRect(float x, float y, float w, float h,
              float r, float g, float b, float a) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
    glDisable(GL_BLEND);
}

void drawRectBorde(float x, float y, float w, float h,
                   float r, float g, float b, float grosor) {
    glLineWidth(grosor);
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
    glLineWidth(1.0f);
}

// ═════════════════════════════════════════════════════════════
//  IntroScreen
// ═════════════════════════════════════════════════════════════

IntroScreen::IntroScreen() { reset(); }

void IntroScreen::reset() { m_frame = 0; m_done = false; }

void IntroScreen::skip() { m_done = true; }

void IntroScreen::draw(int w, int h) {
    m_frame++;
    if (m_frame >= FRAMES) { m_done = true; return; }

    // Alpha: fade-in primer cuarto, fade-out ultimo cuarto
    float t = m_frame / (float)FRAMES;
    float alpha = 1.0f;
    if (t < 0.25f)       alpha = t / 0.25f;
    else if (t > 0.75f)  alpha = (1.0f - t) / 0.25f;

    entrar2D(w, h);

    // Fondo negro
    drawRect(0, 0, (float)w, (float)h, 0, 0, 0, 1.0f);

    // Titulo
    glColor4f(0.85f * alpha, 0.70f * alpha, 0.10f * alpha, alpha);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::string titulo = "MOROS Y CRISTIANOS";
    int tw = glutBitmapLength((unsigned char*)GLUT_BITMAP_TIMES_ROMAN_24,
                              (const unsigned char*)titulo.c_str());
    glRasterPos2f(w / 2.0f - tw / 2.0f, h / 2.0f + 30);
    for (char c : titulo)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

    // Subtitulo
    glColor4f(0.85f * alpha, 0.85f * alpha, 0.85f * alpha, alpha);
    std::string sub = "La Reconquista";
    int sw = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_18,
                              (const unsigned char*)sub.c_str());
    glRasterPos2f(w / 2.0f - sw / 2.0f, h / 2.0f);
    for (char c : sub)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    // "Pulsa cualquier tecla" (parpadea en la segunda mitad)
    if (m_frame > FRAMES / 2) {
        float parpadeo = (sinf(m_frame * 0.15f) + 1.0f) * 0.5f * alpha;
        glColor4f(0.7f, 0.7f, 0.7f, parpadeo);
        std::string skip = "Pulsa cualquier tecla para continuar";
        int pk = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_12,
                                  (const unsigned char*)skip.c_str());
        glRasterPos2f(w / 2.0f - pk / 2.0f, h / 4.0f);
        for (char c : skip)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glDisable(GL_BLEND);
    salir2D();
}

// ═════════════════════════════════════════════════════════════
//  MainMenu
// ═════════════════════════════════════════════════════════════

MainMenu::MainMenu() { reset(); }

void MainMenu::reset() {
    m_paso = 0; m_sel = 0; m_frame = 0;
    m_done = false;
    m_nextState = GameState::E_MENU;
    m_cfg = GameConfig();
}

// ─── draw principal ──────────────────────────────────────────
void MainMenu::draw(int w, int h) {
    m_frame++;
    entrar2D(w, h);
    drawFondo(w, h);
    drawTitulo(w, h);
    switch (m_paso) {
        case 0: drawPaso0(w, h); break;
        case 1: drawPaso1(w, h); break;
        case 2: drawPaso2(w, h); break;
    }
    drawPie(w, h);
    salir2D();
}

void MainMenu::drawFondo(int w, int h) {
    // Degradado: azul oscuro arriba -> rojo oscuro abajo
    glBegin(GL_QUADS);
        glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(0, h);
        glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(w, h);
        glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(w, 0);
        glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(0, 0);
    glEnd();
    // Linea dorada central
    glLineWidth(2.0f);
    glColor3f(0.85f, 0.70f, 0.10f);
    glBegin(GL_LINES);
        glVertex2f(w * 0.05f, h * 0.75f);
        glVertex2f(w * 0.95f, h * 0.75f);
    glEnd();
    glLineWidth(1.0f);
}

void MainMenu::drawTitulo(int w, int h) {
    float br = (sinf(m_frame * 0.04f) + 1.0f) * 0.12f;
    drawTextoCentrado("MOROS Y CRISTIANOS", w / 2.0f, h - 70,
                      0.85f + br, 0.70f + br * 0.5f, 0.10f,
                      GLUT_BITMAP_TIMES_ROMAN_24);
    drawTextoCentrado("- La Reconquista -", w / 2.0f, h - 100,
                      0.75f, 0.75f, 0.75f,
                      GLUT_BITMAP_HELVETICA_12);
}

void MainMenu::drawPaso0(int w, int h) {
    drawTextoCentrado("Selecciona el modo de juego:", w / 2.0f, h - 135,
                      0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);

    float bw = 280, bh = 44, gap = 14;
    float sx = w / 2.0f - bw / 2.0f;
    float sy = h / 2.0f + 60;

    const char* opts[] = { "  Jugador vs Jugador",
                            "  Jugador vs IA",
                            "  Ranking",
                            "  Salir" };
    for (int i = 0; i < 4; i++)
        drawOpcion(opts[i], sx, sy - i * (bh + gap), bw, bh, m_sel == i);
}

void MainMenu::drawPaso1(int w, int h) {
    std::string modotxt = (m_cfg.mode == GameMode::PVP) ?
                          "Modo: Jugador vs Jugador" : "Modo: Jugador vs IA";
    drawTextoCentrado(modotxt, w / 2.0f, h - 135,
                      0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
    drawTextoCentrado("Elige tu bando:", w / 2.0f, h - 155,
                      0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_12);

    float bw = 240, bh = 55, gap = 30;
    float sy = h / 2.0f + 10;
    drawOpcion("  CRISTIANO  ", w / 2.0f - bw - gap / 2.0f, sy, bw, bh, m_sel == 0);
    drawOpcion("  MUSULMAN ",  w / 2.0f + gap / 2.0f,       sy, bw, bh, m_sel == 1);
}

void MainMenu::drawPaso2(int w, int h) {
    std::string modo  = (m_cfg.mode  == GameMode::PVP) ? "Jugador vs Jugador" : "Jugador vs IA";
    std::string bando = (m_cfg.bando == Bando::CRISTIANO) ? "Cristiano" : "Musulman";
    drawTextoCentrado("Modo:  " + modo,  w / 2.0f, h / 2.0f + 70,
                      0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    drawTextoCentrado("Bando: " + bando, w / 2.0f, h / 2.0f + 40,
                      0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float bw = 180, bh = 44, gap = 30;
    float cy = h / 2.0f - 20;
    drawOpcion("  JUGAR",   w / 2.0f - bw - gap / 2.0f, cy, bw, bh, m_sel == 0);
    drawOpcion("  VOLVER",  w / 2.0f + gap / 2.0f,       cy, bw, bh, m_sel == 1);
}

void MainMenu::drawPie(int w, int h) {
    drawTextoCentrado("[Flechas/Mouse] Navegar    [ENTER/Click] Seleccionar    [ESC] Volver",
                      w / 2.0f, 12, 0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
}

void MainMenu::drawOpcion(const std::string& txt, float x, float y,
                           float bw, float bh, bool sel) {
    if (sel)
        drawRect(x, y, bw, bh, 0.85f, 0.70f, 0.10f, 0.25f);
    else
        drawRect(x, y, bw, bh, 0.08f, 0.08f, 0.08f, 0.50f);

    if (sel)
        drawRectBorde(x, y, bw, bh, 0.95f, 0.80f, 0.10f, 2.0f);
    else
        drawRectBorde(x, y, bw, bh, 0.35f, 0.35f, 0.35f, 1.0f);

    if (sel)
        drawTexto(txt, x + 12, y + bh / 2.0f - 6, 1.0f, 1.0f, 0.55f,
                  GLUT_BITMAP_HELVETICA_18);
    else
        drawTexto(txt, x + 12, y + bh / 2.0f - 6, 0.80f, 0.80f, 0.80f,
                  GLUT_BITMAP_HELVETICA_18);
}

// ─── input ───────────────────────────────────────────────────
void MainMenu::keyDown(unsigned char key) {
    if (key == 27) {
        if (m_paso > 0) { m_paso--; m_sel = 0; }
        else { m_nextState = GameState::E_INTRO; m_done = true; }
        return;
    }
    if (key == 13 || key == ' ') { confirmar(); return; }
    if (key == 'w' || key == 'W') m_sel = (m_sel - 1 + maxSel()) % maxSel();
    if (key == 's' || key == 'S') m_sel = (m_sel + 1) % maxSel();
}

void MainMenu::specialKey(int key) {
    if (key == GLUT_KEY_UP)    m_sel = (m_sel - 1 + maxSel()) % maxSel();
    if (key == GLUT_KEY_DOWN)  m_sel = (m_sel + 1) % maxSel();
    if (key == GLUT_KEY_LEFT  && m_paso == 1) m_sel = 0;
    if (key == GLUT_KEY_RIGHT && m_paso == 1) m_sel = 1;
}

void MainMenu::mouseMove(int mx, int my, int w, int h) {
    int gy = h - my;  // invertir Y
    auto enCaja = [&](float x, float y, float bw, float bh) {
        return mx >= x && mx <= x + bw && gy >= y && gy <= y + bh;
    };
    if (m_paso == 0) {
        float bw = 280, bh = 44, gap = 14;
        float sx = w / 2.0f - bw / 2.0f, sy = h / 2.0f + 60;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (bh + gap), bw, bh)) m_sel = i;
    } else if (m_paso == 1) {
        float bw = 240, bh = 55, gap = 30, sy = h / 2.0f + 10;
        if (enCaja(w / 2.0f - bw - gap / 2.0f, sy, bw, bh)) m_sel = 0;
        if (enCaja(w / 2.0f + gap / 2.0f,       sy, bw, bh)) m_sel = 1;
    } else if (m_paso == 2) {
        float bw = 180, bh = 44, gap = 30, cy = h / 2.0f - 20;
        if (enCaja(w / 2.0f - bw - gap / 2.0f, cy, bw, bh)) m_sel = 0;
        if (enCaja(w / 2.0f + gap / 2.0f,       cy, bw, bh)) m_sel = 1;
    }
}

void MainMenu::mouseClick(int mx, int my, int w, int h) {
    mouseMove(mx, my, w, h);
    confirmar();
}

void MainMenu::confirmar() {
    if (m_paso == 0) {
        switch (m_sel) {
            case 0: m_cfg.mode = GameMode::PVP; m_paso = 1; m_sel = 0; break;
            case 1: m_cfg.mode = GameMode::PVC; m_paso = 1; m_sel = 0; break;
            case 2: m_nextState = GameState::E_RANKING;  m_done = true; break;
            case 3: m_nextState = GameState::E_GAMEOVER; m_done = true; break;
        }
    } else if (m_paso == 1) {
        m_cfg.bando = (m_sel == 0) ? Bando::CRISTIANO : Bando::MUSULMAN;
        m_paso = 2; m_sel = 0;
    } else if (m_paso == 2) {
        if (m_sel == 0) { m_nextState = GameState::E_TABLERO; m_done = true; }
        else            { m_paso = 1; m_sel = 0; }
    }
}
