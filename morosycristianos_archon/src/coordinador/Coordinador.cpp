#include "Coordinador.h"
#include "freeglut.h"

// ─────────────────────────────────────────────────────────────
void Coordinador::inicializa()
{
    // Comentario: Hemos quitado tablero.init() y arena.init() porque no existen.
    // Cuando María y Sofía te pasen sus clases, las pondremos aquí.
    estado = MENU;
}

// ─────────────────────────────────────────────────────────────
void Coordinador::dibuja() const
{
    // Comentario: Ponemos fondos de colores para saber que el estado cambia
    switch (estado)
    {
    case MENU:
        glClearColor(0.5, 0, 0, 1); // Rojo oscuro (vibe inicial)
        // Aquí Inés pintará el menú más tarde
        break;

    case TABLERO:
        glClearColor(0, 0.5, 0, 1); // Verde (donde irá el tablero de María)
        break;

    case ARENA:
        glClearColor(0, 0, 0.5, 1); // Azul (donde irá tu arena y la de Sofía)
        break;

    case VICTORIA:
        glClearColor(1, 1, 0, 1); // Amarillo (¡ganaste!)
        break;
    }

    glClear(GL_COLOR_BUFFER_BIT); // Limpia la pantalla con el color del estado
}

// ─────────────────────────────────────────────────────────────
void Coordinador::tecla(unsigned char key)
{
    switch (estado)
    {
    case MENU:
        if (key == 'e' || key == 'E') {
            estado = TABLERO; // Comentario: ¡Funciona! Pasamos al verde
        }
        break;

    case TABLERO:
        // Simulamos una colisión para probar tu Arena
        if (key == 'a' || key == 'A') {
            estado = ARENA; // Comentario: Pasamos al azul
        }
        break;

    case ARENA:
        if (key == 'v' || key == 'V') {
            estado = VICTORIA;
        }
        break;

    case VICTORIA:
        if (key == 'c' || key == 'C') estado = MENU;
        break;
    }
}

// En Coordinador.cpp añade esto al final si no está:

void Coordinador::tecla_especial(int key) {
    // Comentario: Vacío por ahora hasta que necesitemos las flechas del teclado
}

void Coordinador::raton(int btn, int state, int x, int y) {
    // Comentario: Vacío hasta que usemos el mouse
}

void Coordinador::mueve(double dt) {
    // Comentario: Aquí irá la lógica de movimiento de tu IA
}