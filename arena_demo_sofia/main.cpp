#include "Arena.h"
#include "ArenaRenderer.h"
#include "InputState.h"
#include "freeglut.h"
#include <chrono> // Para medir el tiempo entre frames (dt)

// Punto de la entrada del prorama, se inicializa GLUT, se crean la arena y el InputState
// y se registran los callbacks

// CONTROLES:
// P1 (Cristiano): W A S D para mover, F para atacar
// P2 (Andalusi): Flechas para mover, L para atacar
// R: reiniciar combate
// ESC: salir

// Estado global, necesario para los callbacks de GLUT
Arena g_arena;
InputState g_input;
std::chrono::steady_clock::time_point g_ultimoFrame;

// Arrays de teclas pulsadas (movimiento continuo)
bool g_teclasNormales[256] = { false };
bool g_teclasEspeciales[256] = { false };

// Leemos el estado del teclado y reellenamos el InputState
void leerTeclado()
{
    // P1 - Cristiano (WASD)
    g_input.p1.delante = g_teclasNormales['w'] || g_teclasNormales['W'];
    g_input.p1.atras = g_teclasNormales['s'] || g_teclasNormales['S'];
    g_input.p1.izquierda = g_teclasNormales['a'] || g_teclasNormales['A'];
    g_input.p1.derecha = g_teclasNormales['d'] || g_teclasNormales['D'];

    // P2 - Andalusi (Flechas)
    g_input.p2.delante = g_teclasEspeciales[GLUT_KEY_UP];
    g_input.p2.atras = g_teclasEspeciales[GLUT_KEY_DOWN];
    g_input.p2.izquierda = g_teclasEspeciales[GLUT_KEY_LEFT];
    g_input.p2.derecha = g_teclasEspeciales[GLUT_KEY_RIGHT];
    // atacar se setea en onKeyDown, no aqui
}

// Callbacks de GLUT

void onDisplay()
{
    ArenaRenderer::dibujar(g_arena);
}

void onReshape(int ancho, int alto)
{
    ArenaRenderer::configurarVista(ancho, alto);
}

void onKeyDown(unsigned char tecla, int, int)
{
    g_teclasNormales[tecla] = true; // g_ indica que es una variable global
    if (tecla == 'f' || tecla == 'F') g_input.p1.atacar = true;
    if (tecla == 'l' || tecla == 'L') g_input.p2.atacar = true;
    if (tecla == 'r' || tecla == 'R') g_arena.reiniciar();
    if (tecla == 27) exit(0); // ESC = salir
}

void onKeyUp(unsigned char tecla, int, int) // x, y
{
    g_teclasNormales[tecla] = false;
}

void onSpecialDown(int tecla, int, int) // x, y
{
    if (tecla >= 0 && tecla < 256) g_teclasEspeciales[tecla] = true;
}

void onSpecialUp(int tecla, int, int)
{
    if (tecla >= 0 && tecla < 256) g_teclasEspeciales[tecla] = false;
}

// Bucle principal: GLUT llama a esto cada 16ms (60 FPS)
void onTimer(int) // v
{
    // Calcular delta time
    auto ahora = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(ahora - g_ultimoFrame).count();
    g_ultimoFrame = ahora;
    if (dt > 0.05f) dt = 0.05f; // Limitar dt para evitar saltos si la ventana se pausa

    leerTeclado();
    g_arena.actualizar(dt, g_input);

    glutPostRedisplay(); // Pedir a GLUT que llame a onDisplay
    glutTimerFunc(16, onTimer, 0); // Programar el siguiente frame
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Archon Warfare - Arena de combate (demo)");

    // Registrar callbacks
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialFunc(onSpecialDown);
    glutSpecialUpFunc(onSpecialUp);

    // Arrancar el bucle
    g_ultimoFrame = std::chrono::steady_clock::now();
    glutTimerFunc(16, onTimer, 0);

    glutMainLoop();
    return 0;
}