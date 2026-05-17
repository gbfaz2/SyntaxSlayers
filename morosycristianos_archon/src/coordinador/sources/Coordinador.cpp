#include "Coordinador.h"
#include "freeglut.h"

void Coordinador::inicializa()
{
	estado = INTRO; // ARRANCA EN INTRO
}

void Coordinador::dibuja()
{
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
	case ARENA:
		glClearColor(0, 0, 0.5, 1); // AZUL
		break;
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
}

void Coordinador::tecla_especial(int key)
{
	// VACÍO POR AHORA
}

void Coordinador::mueve(double dt)
{
	// VACÍO POR AHORA
}

void Coordinador::raton(int button, int state, int x, int y)
{
	// VACÍO POR AHORA
}