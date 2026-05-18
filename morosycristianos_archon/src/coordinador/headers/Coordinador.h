#pragma once
#include "estadojuego.h"
#include "menu.h"
#include "pantallabatalla.h"
#include "tablero.h"
#include "tablerogl.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"
#include "GestorVictoria.h"

class Coordinador
{
	// DEFINIMOS LOS ESTADOS AQUÍ MISMO PARA NO DEPENDER DE OTROS ARCHIVOS
	EstadoJuego     estado{ EstadoJuego::INTRO };

	int             anchoVentana{ 800 };
	int             altoVentana{ 600 };

	PantallaIntro   pantallaIntro;
	MenuPrincipal   menuPrincipal;
	PantallaDestino pantallaDestino;
	ConfigPartida   configuracion;

	Tablero* pTablero{ nullptr };
	Tablerogl* pTablerogl{ nullptr };

	GestorMovimiento gestorMovimiento;
	GestorTurnos     gestorTurnos;
	GestorVictoria   gestorVictoria;


public:
	Coordinador() = default; // CONSTRUCTOR
	~Coordinador(); // DESTRUCTOR

	void inicializa(); // SETUP INICIAL
	void dibuja(); // PINTA SEGÚN ESTADO
	void tecla(unsigned char key); // GESTIÓN TECLADO
	void tecla_especial(int key); // GESTIÓN FLECHAS
	void mueve(double dt); // ACTUALIZA LÓGICA
	void raton(int button, int state, int x, int y); // GESTIÓN RATÓN
	void ratonMovido(int x, int y);
	void redimensionar(int ancho, int alto);
};