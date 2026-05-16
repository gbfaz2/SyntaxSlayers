#include "Coordinador.h"
#include "freeglut.h"
#include <ctime>
#include <cstdlib>

Coordinador::~Coordinador()
{
	delete pTablerogl;
	delete pTablero;
}

void Coordinador::inicializa()
{
	srand((unsigned)time(nullptr));
	pantallaIntro.reiniciar();
	menuPrincipal.reiniciar();
	estado = EstadoJuego::INTRO;
}

void Coordinador::dibuja()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (estado) {

    case EstadoJuego::INTRO:
        pantallaIntro.dibujar(anchoVentana, altoVentana);
        if (pantallaIntro.terminado()) {
            pantallaIntro.reiniciar();
            menuPrincipal.reiniciar();
            estado = EstadoJuego::MENU;
        }
        break;

    case EstadoJuego::MENU:
        menuPrincipal.dibujar(anchoVentana, altoVentana);
        if (menuPrincipal.terminado()) {
            EstadoJuego siguiente = menuPrincipal.siguienteEstado();
            configuracion = menuPrincipal.getConfiguracion();

            if (siguiente == EstadoJuego::FINAL) exit(0);

            if (siguiente == EstadoJuego::DESTINO) {
                pantallaDestino.reiniciar(configuracion);
                if (!pTablero) {
                    pTablero = new Tablero();
                    pTablerogl = new Tablerogl(pTablero);
                    pTablerogl->init();
                }
            }
            estado = siguiente;
        }
        break;

    case EstadoJuego::DESTINO:
        pantallaDestino.dibujar(anchoVentana, altoVentana);
        if (pantallaDestino.terminado())
            estado = EstadoJuego::TABLERO;
        break;

    case EstadoJuego::TABLERO:
        if (pTablerogl) pTablerogl->Dibuja();
        break;

    case EstadoJuego::ARENA:
        // TODO: arena de combate
        break;

    case EstadoJuego::RANKING:
        // TODO: pantalla de ranking
        break;

    case EstadoJuego::FINAL:
        // TODO: pantalla de fin de partida
        break;

    default: break;
    }

    glutSwapBuffers();
}

void Coordinador::tecla(unsigned char key)
{
    switch (estado) {
    case EstadoJuego::INTRO:
        pantallaIntro.saltar();
        break;
    case EstadoJuego::MENU:
        menuPrincipal.teclaPulsada(key);
        break;
    case EstadoJuego::DESTINO:
        pantallaDestino.avanzar();
        break;
    case EstadoJuego::TABLERO:
        if (key == 27) { menuPrincipal.reiniciar(); estado = EstadoJuego::MENU; break; }
        if (pTablerogl) pTablerogl->KeyDown(key);
        break;
    default:
        if (key == 27) { menuPrincipal.reiniciar(); estado = EstadoJuego::MENU; }
        break;
    }
    glutPostRedisplay();
}

void Coordinador::tecla_especial(int key)
{
    switch (estado) {
    case EstadoJuego::MENU:
        menuPrincipal.teclaEspecial(key);
        break;
    case EstadoJuego::TABLERO:
        if (pTablerogl) pTablerogl->SpecialKey(key);
        break;
    default: break;
    }
    glutPostRedisplay();
}

void Coordinador::mueve(double /*dt*/)
{
    // La animación la manejan las propias pantallas por fotograma (glutPostRedisplay en OnTimer)
}

void Coordinador::raton(int boton, int state, int x, int y)
{
    bool pulsado = (state == GLUT_DOWN);
    if (!pulsado) return;

    switch (estado) {
    case EstadoJuego::INTRO:
        pantallaIntro.saltar();
        break;
    case EstadoJuego::DESTINO:
        pantallaDestino.avanzar();
        break;
    case EstadoJuego::MENU:
        if (boton == GLUT_LEFT_BUTTON)
            menuPrincipal.ratonPulsado(x, y, anchoVentana, altoVentana);
        break;
    case EstadoJuego::TABLERO:
        if (pTablerogl) {
            int button;
            if (boton == GLUT_LEFT_BUTTON)       button = MOUSE_LEFT_BUTTON;
            else if (boton == GLUT_RIGHT_BUTTON) button = MOUSE_RIGHT_BUTTON;
            else                                 button = MOUSE_MIDDLE_BUTTON;

            int specialKey = glutGetModifiers();
            bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
            bool shiftKey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

            pTablerogl->MouseButton(x, y, button, true, shiftKey, ctrlKey);
        }
        break;
    default: break;
    }
    glutPostRedisplay();
}

/*void Coordinador::mueve(double dt)
{
	// VACÍO POR AHORA
}*/

void Coordinador::ratonMovido(int x, int y)
{
    if (estado == EstadoJuego::MENU)
        menuPrincipal.ratonMovido(x, y, anchoVentana, altoVentana);
    glutPostRedisplay();
}

void Coordinador::redimensionar(int ancho, int alto) {
    anchoVentana = ancho;
    altoVentana = (alto == 0) ? 1 : alto;
    glViewport(0, 0, anchoVentana, altoVentana);
}

/*void Coordinador::raton(int button, int state, int x, int y)
{
	// VACÍO POR AHORA
}*/