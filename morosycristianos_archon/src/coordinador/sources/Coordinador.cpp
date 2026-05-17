#include "Coordinador.h"
#include "freeglut.h"

void Coordinador::inicializa()
{
	estado = ARENA; // ARRANCA EN ARENA PARA PRUEBAS
	ArenaRenderer::configurarVista(1024, 768); // CONFIGURA CAMARA Y PROYECCION
}

void Coordinador::dibuja()
{
	switch (estado) {
	case ARENA:
		ArenaRenderer::dibujar(_arena); // PINTA EL FRAME COMPLETO
		break;
	default:
		glClearColor(0.1f, 0.1f, 0.1f, 1); // GRIS OSCURO PARA OTROS ESTADOS
		glClear(GL_COLOR_BUFFER_BIT);
		break;
	}
}

void Coordinador::tecla(unsigned char key)
{
	if (key == 27) exit(0); // ESC: CIERRA

	if (estado == ARENA) {
		// CONTROLES P1: WASD + F
		if (key == 'w' || key == 'W') _input.p1.delante = true;
		if (key == 's' || key == 'S') _input.p1.atras = true;
		if (key == 'a' || key == 'A') _input.p1.izquierda = true;
		if (key == 'd' || key == 'D') _input.p1.derecha = true;
		if (key == 'f' || key == 'F') _input.p1.atacar = true;
	}
}

void Coordinador::tecla_up(unsigned char key)
{
	if (estado == ARENA) {
		// SUELTA TECLAS P1
		if (key == 'w' || key == 'W') _input.p1.delante = false;
		if (key == 's' || key == 'S') _input.p1.atras = false;
		if (key == 'a' || key == 'A') _input.p1.izquierda = false;
		if (key == 'd' || key == 'D') _input.p1.derecha = false;
	}
}

void Coordinador::tecla_especial(int key)
{
	// VACIO: P2 LO CONTROLA LA IA
}

void Coordinador::tecla_especial_up(int key)
{
	// VACIO: P2 LO CONTROLA LA IA
}

void Coordinador::mueve(double dt)
{
	if (estado == ARENA)
		_arena.actualizar((float)dt, _input); // AVANZA LA LOGICA
}

void Coordinador::raton(int button, int state, int x, int y)
{
	// VACIO POR AHORA
}