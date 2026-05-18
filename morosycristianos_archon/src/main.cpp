#include "freeglut.h"
#include "coordinador.h"
#include <chrono>
using namespace std::chrono;


Coordinador juego;


void OnDraw();
void OnTimer(int value);
void OnKeyboardDown(unsigned char key, int x, int y);
void OnSpecialKeyboardDown(int key, int x, int y);
void OnMouseClick(int button, int state, int x, int y);
void OnPassiveMotion(int x, int y);
void OnReshape(int ancho, int alto);
void OnKeyboardUp(unsigned char key, int x, int y);
void OnSpecialKeyboardUp(int key, int x, int y);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1024, 768);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Moros y Cristianos");
    glutSetWindowTitle("Moros y Cristianos");

    
    /*glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1024.0 / 768.0, 0.1, 150.0);*/

    juego.inicializa();

    glutDisplayFunc(OnDraw);
    glutTimerFunc(25, OnTimer, 0);
    glutKeyboardFunc(OnKeyboardDown);
    glutSpecialFunc(OnSpecialKeyboardDown);
    glutMouseFunc(OnMouseClick);
    glutPassiveMotionFunc(OnPassiveMotion);
    glutReshapeFunc(OnReshape);
    glutKeyboardUpFunc(OnKeyboardUp);
    glutSpecialUpFunc(OnSpecialKeyboardUp); 

    glutMainLoop();
    return 0;
}


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
    if (key == 0) exit(0);   // ESC siempre cierra
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
void OnPassiveMotion(int x, int y)
{
    juego.ratonMovido(x, y);
    glutPostRedisplay();
}

void OnReshape(int ancho, int alto)
{
    juego.redimensionar(ancho, alto);
    glutPostRedisplay();
}
void OnTimer(int /*VALUE*/)
{
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    double dt = duration<double>(now - last).count();
    last = now;

    juego.mueve(dt);

    glutPostRedisplay();
    glutTimerFunc(25, OnTimer, 0);
} 

void OnKeyboardUp(unsigned char key, int /*x*/, int /*y*/)
{
    juego.tecla_up(key);
    glutPostRedisplay();
}

void OnSpecialKeyboardUp(int key, int /*x*/, int /*y*/)
{
    juego.tecla_especial_up(key);
    glutPostRedisplay();
}