#pragma once

// ─────────────────────────────────────────────────────────────
// Coordinador: único objeto global. Recibe todos los callbacks
// de GLUT y los redirige a la escena activa según el estado.
// Sigue el mismo patrón que el Coordinador del lab de PANG.
// ─────────────────────────────────────────────────────────────
class Coordinador
{
    // ── Máquina de estados ────────────────────────────────────
    enum Estado {
        MENU,           // pantalla de inicio
        TABLERO,        // fase estratégica 9x9
        TRANSICION,     // colisión detectada → entrando en arena
        ARENA,          // combate 1v1 tiempo real
        FIN_COMBATE,    // resultado decidido → volvemos al tablero
        PAUSA,          // juego congelado (tecla P)
        VICTORIA,       // condición de victoria cumplida
        DERROTA         // game over
    };

    Estado estado { MENU };

    

    // ── Datos de contexto entre escenas ──────────────────────
    // Cuando el tablero detecta colisión, rellena estos campos
    // y el Coordinador los pasa a la arena.
    int piezaAtacante { -1 };   // id de la pieza atacante
    int piezaDefensora { -1 };  // id de la pieza defensora

public:
    Coordinador() = default;
    virtual ~Coordinador() = default;

    void inicializa();                      // llamado una vez desde main
    void tecla(unsigned char key);          // callback teclado normal
    void tecla_especial(int key);           // callback flechas / teclas especiales
    void raton(int btn, int state, int x, int y); // callback ratón
    void mueve(double dt);                  // callback timer (lógica + física)
    void dibuja() const;                    // callback display

private:
    void _entrar_arena();    // configura la arena con las piezas que colisionaron
    void _volver_tablero();  // aplica resultado del combate al tablero
};
