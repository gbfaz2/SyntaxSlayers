#include "dibuja.h"
#include "freeglut.h"

int Dibuja::_anchoVentana = 1024; // O el tamaño por defecto que prefieras
int Dibuja::_altoVentana = 768;

// UTILIDADES OPENGL

void Dibuja::util_entrar2D(int ancho, int alto) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ancho, 0, alto);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void Dibuja::util_salir2D() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}