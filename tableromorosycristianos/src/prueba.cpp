#include "ETSIDI.h"
#include "glut.h" // O el nombre que tenga tu cabecera de glut

void dibuja() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Aquí irá nuestro tablero más adelante
    glutSwapBuffers();
}

void buclePrincipal() {
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    // Inicialización de la ventana
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Batallas de Al-Andalus - Maria");

    // Registro de las funciones de dibujado y bucle
    glutDisplayFunc(dibuja);
    glutIdleFunc(buclePrincipal);

    // Reproducir un sonido de prueba si tienes uno, o simplemente iniciar el juego
    // ETSIDI::playMusica("sonido_fondo.mp3", true); 

    glutMainLoop();

    return 0;
}