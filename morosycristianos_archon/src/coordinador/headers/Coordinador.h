#pragma once
#include "estadojuego.h"
#include "menu.h"
#include "pantallabatalla.h"
#include "tablero.h"
#include "tablerogl.h"
#include "Arena.h"
#include "InputState.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"
#include "GestorVictoria.h"
#include "GestorHechizos.h"
#include "GestorInput.h"
#include "SpriteRey.h"
#include "GestorPartida.h"
#include "MinimaxTablero.h"

class Coordinador
{
	friend class GestorInput; // ACCESO TOTAL AL GESTOR DE INPUT
	
	EstadoJuego     estado{ EstadoJuego::INTRO }; // ESTADO INICIAL

	

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
	SpriteRey _spriteReyLocal;

	Pieza* _pAtacanteCombate{ nullptr };  // pieza atacante del combate actual
	Pieza* _pDefensoraCombate{ nullptr }; // pieza defensora del combate actual

	MinimaxTablero _minimax{ 3 };          // IA CON PROFUNDIDAD 3
	bool _iaCalculando{ false };           // EVITA CALCULAR VARIAS VECES POR FRAME
	bool _iaYaMovio{ false };  // CONTROLA QUE LA IA SOLO MUEVE UNA VEZ POR TURNO
	int  _turnoAnteriorIA{ -1 }; // TURNO EN EL QUE MOVIÓ LA IA POR ÚLTIMA VEZ
	float _tiempoEsperaIA{ 0.0f }; // TEMPORIZADOR ANTES DE QUE MUEVA LA IA

public:
	Coordinador() = default;
	~Coordinador();
    
	int      _anchoVentana{ 800 };
	int      _altoVentana{ 600 };
	
	float _tiempoGuardado{ 0.0f };

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

	std::string _rankingGanador{ "" };
	std::string _rankingBatalla{ "" };
	int _rankingTurnos{ 0 };
	int _rankingPiezasLocal{ 0 };
	int _rankingPiezasRival{ 0 };
};