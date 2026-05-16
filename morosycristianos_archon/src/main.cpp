
// Autor: Ines Alcérreca Sánchez
// Punto de entrada del juego, gestión de estados y callbacks de GLUT

#include "freeglut.h"
#include <ctime>
#include "estadojuego.h"
#include "menu.h"
#include "pantallabatalla.h"
#include "tablerogl.h"
#include "Tablero.h"

//Tamaño de la ventana
int anchoVentana = 800;
int altoVentana  = 600;


EstadoJuego   estadoActual = EstadoJuego::INTRO;
ConfigPartida  configuracion;
PantallaIntro  pantallaIntro;
MenuPrincipal  menuPrincipal;
PantallaDestino pantallaDestino;
Tablero*    pTablero    = nullptr;
Tablerogl*  pTablerogl  = nullptr;

// Callbacks de GLUT
void OnDibujar();
void OnTeclado(unsigned char tecla, int x, int y);
void OnTeclaEspecial(int tecla, int x, int y);
void OnRaton(int boton, int estado, int x, int y);
void OnRatonMovido(int x, int y);
void OnRedimensionar(int ancho, int alto);
void OnTemporizador(int valor);

int main(int argc, char* argv[]) {
    srand((unsigned)time(nullptr));
    glutInit(&argc, argv);
    glutInitWindowSize(anchoVentana, altoVentana);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Moros y Cristianos - SyntaxSlayers");

    glutDisplayFunc(OnDibujar);
    glutKeyboardFunc(OnTeclado);
    glutSpecialFunc(OnTeclaEspecial);
    glutMouseFunc(OnRaton);
    glutPassiveMotionFunc(OnRatonMovido);
    glutReshapeFunc(OnRedimensionar);
	glutTimerFunc(16, OnTemporizador, 0);   // Fuerza redibujo cada 16 ms

    glClearColor(0, 0, 0, 1);
    glutMainLoop();
    return 0;
}

// Temporizador: fuerza redibujo cada frame
void OnTemporizador(int valor) {
    glutPostRedisplay();
    glutTimerFunc(16, OnTemporizador, 0);
}

// Redimensionado de ventana
void OnRedimensionar(int ancho, int alto) {
    anchoVentana = ancho;
    altoVentana  = (alto == 0) ? 1 : alto;
    glViewport(0, 0, anchoVentana, altoVentana);
}

// Dibujo principal: enruta según el estado actual
void OnDibujar() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (estadoActual) {

    // INTRO 
    case EstadoJuego::INTRO:
        pantallaIntro.dibujar(anchoVentana, altoVentana);
        if (pantallaIntro.terminado()) {
            pantallaIntro.reiniciar();
            menuPrincipal.reiniciar();
            estadoActual = EstadoJuego::MENU;
        }
        break;

    // MENU 
    case EstadoJuego::MENU:
        menuPrincipal.dibujar(anchoVentana, altoVentana);
        if (menuPrincipal.terminado()) {
            EstadoJuego siguiente = menuPrincipal.siguienteEstado();
            configuracion = menuPrincipal.getConfiguracion();

            if (siguiente == EstadoJuego::FINAL) exit(0);

            if (siguiente == EstadoJuego::DESTINO) {
                pantallaDestino.reiniciar(configuracion);
                // Inicializar tablero ya (se usara al salir de DESTINO)
                if (!pTablero) {
                    pTablero   = new Tablero();
                    pTablerogl = new Tablerogl(pTablero);
                    pTablerogl->init();
                }
            }
            estadoActual = siguiente;
        }
        break;

    case EstadoJuego::DESTINO:
        pantallaDestino.dibujar(anchoVentana, altoVentana);
        if (pantallaDestino.terminado())
            estadoActual = EstadoJuego::TABLERO;
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

// TECLADO
void OnTeclado(unsigned char tecla, int x, int y) {
    switch (estadoActual) {
    case EstadoJuego::INTRO:
        pantallaIntro.saltar();
        break;
    case EstadoJuego::MENU:
        menuPrincipal.teclaPulsada(tecla);
        break;
    case EstadoJuego::DESTINO:
        pantallaDestino.avanzar();
        break;
    case EstadoJuego::TABLERO:
        if (tecla == 27) { menuPrincipal.reiniciar(); estadoActual = EstadoJuego::MENU; break; }
        if (pTablerogl) pTablerogl->KeyDown(tecla);
        break;
    default:
        if (tecla == 27) { menuPrincipal.reiniciar(); estadoActual = EstadoJuego::MENU; }
        break;
    }
    glutPostRedisplay();
}

// TECLAS ESPECIALES (Flechas, etc)
void OnTeclaEspecial(int tecla, int x, int y) {
    switch (estadoActual) {
    case EstadoJuego::MENU:
        menuPrincipal.teclaEspecial(tecla);
        break;
    case EstadoJuego::TABLERO:
        if (pTablerogl) pTablerogl->SpecialKey(tecla);
        break;
    default: break;
    }
    glutPostRedisplay();
}

// RATON 
void OnRaton(int boton, int estado, int x, int y) {
    bool pulsado = (estado == GLUT_DOWN);
    if (!pulsado) return;

    switch (estadoActual) {
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
            bool down = (estado == GLUT_DOWN);

            //convertimos la constante de glut al enum nuestro
            int button;
            if (boton == GLUT_LEFT_BUTTON) button = MOUSE_LEFT_BUTTON;
            else if (boton == GLUT_RIGHT_BUTTON) button = MOUSE_RIGHT_BUTTON;
            else button = MOUSE_MIDDLE_BUTTON;

            //Detectamos ctrl y shift
            int specialKey = glutGetModifiers();
            bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
            bool shiftkey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

            pTablerogl->MouseButton(x, y, button, down, shiftkey, ctrlKey);
            glutPostRedisplay();
        }
        break;
    default: break;
    }
    glutPostRedisplay();
}

// RATON MOVIDO (sin pulsar botones)
void OnRatonMovido(int x, int y) {
    if (estadoActual == EstadoJuego::MENU)
        menuPrincipal.ratonMovido(x, y, anchoVentana, altoVentana);
    glutPostRedisplay();
}
