#pragma once
#include "estadojuego.h"
#include "menu.h"
#include "pantallabatalla.h"
#include "tablero.h"
#include "tablerogl.h"
#include "Arena.h"
#include "ArenaRenderer.h"
#include "InputState.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"
#include "GestorVictoria.h"
#include "GestorHechizos.h"
#include "GestorInput.h"

class Coordinador
{
	EstadoJuego     estado{ EstadoJuego::INTRO }; // ESTADO INICIAL

	int             anchoVentana{ 800 };
	int             altoVentana{ 600 };

	PantallaIntro   pantallaIntro;
	MenuPrincipal   menuPrincipal;
	PantallaDestino pantallaDestino;
	ConfigPartida   configuracion;
	GestorInput gestorInput; // GESTOR UNICO DE INPUT
	
	Tablero* pTablero{ nullptr };
	Tablerogl* pTablerogl{ nullptr };

	Arena           _arena;   // LOGICA DEL COMBATE
	InputState      _input;   // ESTADO DEL TECLADO

	GestorMovimiento gestorMovimiento;
	GestorTurnos     gestorTurnos;
	GestorVictoria   gestorVictoria;
	GestorHechizos* pGestorHechizos{ nullptr };

public:
	Coordinador() = default;
	~Coordinador();

	void inicializa();
	void dibuja();
	void tecla(unsigned char key);
	void tecla_up(unsigned char key); // TECLA SOLTADA
	void tecla_especial(int key);
	void tecla_especial_up(int key); // FLECHA SOLTADA
	void mueve(double dt);
	void raton(int button, int state, int x, int y);
	void ratonMovido(int x, int y);
	void redimensionar(int ancho, int alto);
	void reiniciarTablero(); // REINICIA EL TABLERO AL ESTADO INICIAL
};