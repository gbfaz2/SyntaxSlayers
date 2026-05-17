#include "Coordinador.h"
#include "freeglut.h"
#include "ArenaRenderer.h"

void Coordinador::inicializa()
{
	estado = INTRO; // ARRANCA EN INTRO
}

void Coordinador::dibuja()
{
	if (estado == ARENA) 
	{
		ArenaRenderer::dibujar(_arena);
		return;
	}
	// TEST DE COLORES: CADA ESTADO PINTA LA PANTALLA DE UN COLOR
	switch (estado) {
	case INTRO:
		glClearColor(1, 1, 1, 1); // BLANCO
		break;
	case MENU:
		glClearColor(1, 0.5, 0, 1); // NARANJA
		break;
	case TABLERO:
		glClearColor(0, 0.5, 0, 1); // VERDE
		break;
	/*case ARENA:
		ArenaRenderer::dibujar(_arena);
		//glClearColor(0, 0, 0.5, 1); // AZUL
		return;*/
	case PAUSA:
		glClearColor(0.5, 0.5, 0.5, 1); // GRIS
		break;
	case RANKING:
		glClearColor(1, 1, 0, 1); // AMARILLO
		break;
	case GAMEOVER:
		glClearColor(0.5, 0, 0, 1); // ROJO
		break;
	}

	glClear(GL_COLOR_BUFFER_BIT); // LIMPIA PANTALLA CON COLOR
}

void Coordinador::tecla(unsigned char key)
{
	if (key == 27) exit(0); // ESCAPE: CIERRA JUEGO

	// TEST RÁPIDO: CAMBIA DE ESTADO CON TECLAS PARA PROBAR LA MÁQUINA
	if (key == '1') estado = INTRO; // PULSA 1: INTRO
	if (key == '2') estado = MENU; // PULSA 2: MENU
	if (key == '3') estado = TABLERO; // PULSA 3: TABLERO
	if (key == '4') estado = ARENA; // PULSA 4: ARENA
	if (key == '5') estado = PAUSA; // PULSA 5: PAUSA
	if (key == '6') estado = RANKING; // PULSA 6: RANKING
	if (key == '7') estado = GAMEOVER; // PULSA 7: GAMEOVER

	switch (estado) 
	{
	case ARENA:
		if (key == 'w' || key == 'W') _input.p1.delante = true;
		if (key == 's' || key == 'S') _input.p1.atras = true;
		if (key == 'a' || key == 'A') _input.p1.izquierda = true;
		if (key == 'd' || key == 'D') _input.p1.derecha = true;
		if (key == 'f' || key == 'F') _input.p1.atacar = true;
		if (key == 'r' || key == 'R') _arena.reiniciar();
		if (key == 'l' || key == 'L') _input.p2.atacar = true;
		break;
	}
}

void Coordinador::tecla_especial(int key)
{
	if (estado == ARENA) 
	{
		if (key == GLUT_KEY_UP)    _input.p2.delante = true;
		if (key == GLUT_KEY_DOWN)  _input.p2.atras = true;
		if (key == GLUT_KEY_LEFT)  _input.p2.izquierda = true;
		if (key == GLUT_KEY_RIGHT) _input.p2.derecha = true;
	}
}

void Coordinador::mueve(double dt)
{
	if (estado == ARENA)
		_arena.actualizar((float)dt, _input);
	// VACÍO POR AHORA
}

void Coordinador::raton(int button, int state, int x, int y)
{
	// VACÍO POR AHORA
}

void Coordinador::tecla_up(unsigned char key)
{
	if (estado == ARENA) {
		if (key == 'w' || key == 'W') _input.p1.delante = false;
		if (key == 's' || key == 'S') _input.p1.atras = false;
		if (key == 'a' || key == 'A') _input.p1.izquierda = false;
		if (key == 'd' || key == 'D') _input.p1.derecha = false;
	}
}

void Coordinador::tecla_especial_up(int key)
{
	if (estado == ARENA) {
		if (key == GLUT_KEY_UP)    _input.p2.delante = false;
		if (key == GLUT_KEY_DOWN)  _input.p2.atras = false;
		if (key == GLUT_KEY_LEFT)  _input.p2.izquierda = false;
		if (key == GLUT_KEY_RIGHT) _input.p2.derecha = false;
	}
}