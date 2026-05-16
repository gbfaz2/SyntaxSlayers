#include "freeglut.h"
#include "coordinador.h"
#include <chrono>
using namespace std::chrono;

// ─────────────────────────────────────────────────────────────
// Único objeto global: el jefe de todo
// ─────────────────────────────────────────────────────────────
Coordinador juego;

// ── Prototipos de callbacks ───────────────────────────────────
void OnDraw();
void OnTimer(int value);
void OnKeyboardDown(unsigned char key, int x, int y);
void OnSpecialKeyboardDown(int key, int x, int y);
void OnMouseClick(int button, int state, int x, int y);

// ─────────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1024, 768);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Moros y Cristianos");

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1024.0 / 768.0, 0.1, 150.0);

    juego.inicializa();

    glutDisplayFunc(OnDraw);
    glutTimerFunc(25, OnTimer, 0);
    glutKeyboardFunc(OnKeyboardDown);
    glutSpecialFunc(OnSpecialKeyboardDown);
    glutMouseFunc(OnMouseClick);

    glutMainLoop();
    return 0;
}

// ── Callbacks ─────────────────────────────────────────────────
void OnDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    juego.dibuja();

    glutSwapBuffers();
}

void OnKeyboardDown(unsigned char key, int /*x*/, int /*y*/)
{
    if (key == 27) exit(0);   // ESC siempre cierra
    juego.tecla(key);
    glutPostRedisplay();
}

void OnSpecialKeyboardDown(int key, int /*x*/, int /*y*/)
{
    juego.tecla_especial(key);
    glutPostRedisplay();
}

void OnMouseClick(int button, int state, int x, int y)
{
    juego.raton(button, state, x, y);
    glutPostRedisplay();
}

void OnTimer(int /*value*/)
{
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    double dt = duration<double>(now - last).count();
    last = now;

    juego.mueve(dt);

    glutPostRedisplay();
    glutTimerFunc(25, OnTimer, 0);
}
