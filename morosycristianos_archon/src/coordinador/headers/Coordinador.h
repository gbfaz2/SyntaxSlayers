#pragma once
#include "Arena.h"
#include "InputState.h"

class Coordinador
{
	// DEFINIMOS LOS ESTADOS AQUÍ MISMO PARA NO DEPENDER DE OTROS ARCHIVOS
	enum Estado { INTRO, MENU, TABLERO, ARENA, PAUSA, RANKING, GAMEOVER };
	Estado estado{ INTRO }; // ESTADO INICIAL

	Arena _arena;
	InputState _input;

public:
	Coordinador() = default; // CONSTRUCTOR
	virtual ~Coordinador() = default; // DESTRUCTOR

	void inicializa(); // SETUP INICIAL
	void tecla(unsigned char key); // GESTIÓN TECLADO
	void tecla_especial(int key); // GESTIÓN FLECHAS
	void mueve(double dt); // ACTUALIZA LÓGICA
	void dibuja(); // PINTA SEGÚN ESTADO
	void raton(int button, int state, int x, int y); // GESTIÓN RATÓN

	void tecla_up(unsigned char key); // Tecla soltada
	void tecla_especial_up(int key); // Flecha soltada
};