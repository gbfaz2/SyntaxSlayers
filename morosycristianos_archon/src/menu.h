// menu.h
// Autor: Ines Alcérreca Sánchez
#pragma once
#include "gamestate.h"
#include <string>

// ─── Texto con glut bitmap fonts ────────────────────────────
// Funcion auxiliar compartida por Intro y Menu
void drawTexto(const std::string& texto, float x, float y,
               float r, float g, float b, void* fuente = nullptr);

void drawRect(float x, float y, float w, float h,
              float r, float g, float b, float a = 1.0f);

void drawRectBorde(float x, float y, float w, float h,
                   float r, float g, float b, float grosor = 2.0f);

void entrar2D(int w, int h);
void salir2D();

// ═══════════════════════════════════════════════════════════
//  E_INTRO
// ═══════════════════════════════════════════════════════════
class IntroScreen {
public:
    IntroScreen();
    void draw(int w, int h);
    void skip();
    bool wantsTransition() const { return m_done; }
    void reset();
private:
    int  m_frame;
    bool m_done;
    static const int FRAMES = 250; // ~4 seg a 60fps
};

// ═══════════════════════════════════════════════════════════
//  E_MENU
// ═══════════════════════════════════════════════════════════
class MainMenu {
public:
    MainMenu();
    void draw(int w, int h);
    void keyDown(unsigned char key);
    void specialKey(int key);
    void mouseMove(int x, int y, int w, int h);
    void mouseClick(int x, int y, int w, int h);

    bool       wantsTransition() const { return m_done; }
    GameState  nextState()       const { return m_nextState; }
    GameConfig getConfig()       const { return m_cfg; }
    void reset();

private:
    int        m_paso;   // 0=modo, 1=bando, 2=confirmar
    int        m_sel;
    int        m_frame;
    bool       m_done;
    GameState  m_nextState;
    GameConfig m_cfg;

    void drawFondo(int w, int h);
    void drawTitulo(int w, int h);
    void drawPaso0(int w, int h);
    void drawPaso1(int w, int h);
    void drawPaso2(int w, int h);
    void drawPie(int w, int h);
    void drawOpcion(const std::string& txt, float x, float y,
                    float bw, float bh, bool sel);
    void confirmar();
    int  maxSel() const { return (m_paso == 0) ? 4 : 2; }
};
