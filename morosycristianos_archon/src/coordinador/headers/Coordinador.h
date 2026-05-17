#pragma once
#include "Arena.h"
#include "ArenaRenderer.h"
#include "InputState.h"

class Coordinador
{
	enum Estado { INTRO, MENU, TABLERO, ARENA, PAUSA, RANKING, GAMEOVER };
	Estado estado{ ARENA }; // ARRANCA DIRECTO EN ARENA PARA PRUEBAS

	Arena _arena;           // LOGICA DEL COMBATE
	InputState _input;      // ESTADO DEL TECLADO

public:
	Coordinador() = default;
	virtual ~Coordinador() = default;

	void inicializa();
	void tecla(unsigned char key);
	void tecla_up(unsigned char key);
	void tecla_especial(int key);
	void tecla_especial_up(int key);
	void mueve(double dt);
	void dibuja();
	void raton(int button, int state, int x, int y);
};