#pragma once

class Coordinador
{
	// DEFINIMOS LOS ESTADOS AQUÍ MISMO PARA NO DEPENDER DE OTROS ARCHIVOS
	enum Estado { INTRO, MENU, TABLERO, ARENA, PAUSA, RANKING, GAMEOVER };
	Estado estado{ INTRO }; // ESTADO INICIAL

public:
	Coordinador() = default; // CONSTRUCTOR
	virtual ~Coordinador() = default; // DESTRUCTOR

	void inicializa(); // SETUP INICIAL
	void tecla(unsigned char key); // GESTIÓN TECLADO
	void tecla_especial(int key); // GESTIÓN FLECHAS
	void mueve(double dt); // ACTUALIZA LÓGICA
	void dibuja(); // PINTA SEGÚN ESTADO
	void raton(int button, int state, int x, int y); // GESTIÓN RATÓN
};