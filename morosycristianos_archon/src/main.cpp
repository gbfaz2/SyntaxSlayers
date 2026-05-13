// main.cpp
// Autor: Ines Alcérreca Sánchez
// Integra el menu (Ines) con el tablero (Maria)

#include "freeglut.h"
#include "gamestate.h"
#include "menu.h"
#include "tablerogl.h"
#include "Tablero.h"

// ─── Ventana ─────────────────────────────────────────────────
int W = 800, H = 600;

// ─── Estado ──────────────────────────────────────────────────
GameState  g_state = GameState::E_INTRO;
GameConfig g_cfg;

// ─── Objetos de pantalla ─────────────────────────────────────
IntroScreen  g_intro;
MainMenu     g_menu;

// Tablero de Maria (se inicializa al entrar a E_TABLERO)
Tablero*     g_tablero  = nullptr;
Tablerogl*   g_tablerogl = nullptr;

// ─── Callbacks ───────────────────────────────────────────────
void OnDraw();
void OnKeyboard(unsigned char key, int x, int y);
void OnSpecial(int key, int x, int y);
void OnMouse(int button, int state, int x, int y);
void OnMouseMove(int x, int y);
void OnReshape(int w, int h);
void OnTimer(int v);

// ═════════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(W, H);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Moros y Cristianos - SyntaxSlayers");

    glutDisplayFunc(OnDraw);
    glutKeyboardFunc(OnKeyboard);
    glutSpecialFunc(OnSpecial);
    glutMouseFunc(OnMouse);
    glutPassiveMotionFunc(OnMouseMove);
    glutReshapeFunc(OnReshape);
    glutTimerFunc(16, OnTimer, 0);

    glClearColor(0, 0, 0, 1);
    glutMainLoop();
    return 0;
}

void OnTimer(int v) {
    glutPostRedisplay();
    glutTimerFunc(16, OnTimer, 0);
}

void OnReshape(int w, int h) {
    W = w; H = (h == 0 ? 1 : h);
    glViewport(0, 0, W, H);
}

// ═════════════════════════════════════════════════════════════
//  OnDraw
// ═════════════════════════════════════════════════════════════
void OnDraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (g_state) {

    case GameState::E_INTRO:
        g_intro.draw(W, H);
        if (g_intro.wantsTransition()) {
            g_intro.reset();
            g_menu.reset();
            g_state = GameState::E_MENU;
        }
        break;

    case GameState::E_MENU:
        g_menu.draw(W, H);
        if (g_menu.wantsTransition()) {
            GameState next = g_menu.nextState();
            g_cfg = g_menu.getConfig();
            if (next == GameState::E_GAMEOVER) exit(0);
            if (next == GameState::E_TABLERO) {
                // Inicializar tablero de Maria
                if (!g_tablero) {
                    g_tablero   = new Tablero();
                    g_tablerogl = new Tablerogl(g_tablero);
                    g_tablerogl->init();
                }
            }
            g_state = next;
        }
        break;

    case GameState::E_TABLERO:
        if (g_tablerogl) g_tablerogl->Dibuja();
        break;

    case GameState::E_ARENA:
        // TODO: arena
        break;

    case GameState::E_RANKING:
        // TODO: ranking (Alba)
        break;

    case GameState::E_GAMEOVER:
        // TODO: pantalla final (Ines)
        break;

    default: break;
    }

    glutSwapBuffers();
}

// ═════════════════════════════════════════════════════════════
//  Teclado
// ═════════════════════════════════════════════════════════════
void OnKeyboard(unsigned char key, int x, int y) {
    switch (g_state) {
    case GameState::E_INTRO:
        g_intro.skip();
        break;
    case GameState::E_MENU:
        g_menu.keyDown(key);
        break;
    case GameState::E_TABLERO:
        if (key == 27) { g_menu.reset(); g_state = GameState::E_MENU; break; }
        if (g_tablerogl) g_tablerogl->KeyDown(key);
        break;
    default:
        if (key == 27) { g_menu.reset(); g_state = GameState::E_MENU; }
        break;
    }
    glutPostRedisplay();
}

void OnSpecial(int key, int x, int y) {
    if (g_state == GameState::E_MENU) g_menu.specialKey(key);
    glutPostRedisplay();
}

// ═════════════════════════════════════════════════════════════
//  Raton
// ═════════════════════════════════════════════════════════════
void OnMouse(int button, int state, int x, int y) {
    bool down = (state == GLUT_DOWN);
    if (!down) return;
    switch (g_state) {
    case GameState::E_INTRO:
        g_intro.skip();
        break;
    case GameState::E_MENU:
        if (button == GLUT_LEFT_BUTTON) g_menu.mouseClick(x, y, W, H);
        break;
    case GameState::E_TABLERO:
        if (g_tablerogl) {
            bool ctrl  = (glutGetModifiers() & GLUT_ACTIVE_CTRL)  != 0;
            bool shift = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) != 0;
            int  btn   = (button == GLUT_LEFT_BUTTON) ? MOUSE_LEFT_BUTTON : MOUSE_RIGHT_BUTTON;
            g_tablerogl->MouseButton(x, y, btn, down, shift, ctrl);
        }
        break;
    default: break;
    }
    glutPostRedisplay();
}

void OnMouseMove(int x, int y) {
    if (g_state == GameState::E_MENU) g_menu.mouseMove(x, y, W, H);
    glutPostRedisplay();
}
