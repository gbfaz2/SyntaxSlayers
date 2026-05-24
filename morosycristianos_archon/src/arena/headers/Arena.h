#pragma once
#include "Combatiente.h"
#include "InputState.h"
#include "IAArena.h"
#include "estadojuego.h"

// Escenario de combate que contiene a los dos combatienyes y 
// la logica de quien le da a quien

// IMPORTANTE: esta clase no sabe nada de OpenGL ni de como se dibuja, solo
// sabe la logica de juego. Del dibujo se encargara ArenaRenderer.

// Posibles resultados de un combate
enum class ResultadoCombate {
	EnCurso,
	GanaP1,
	GanaP2,
	Empate,
};

class Arena 
{
	// Dimensiones del escenario
	float _ancho{ 14.0f };
	float _profundo{ 10.0f };

	Combatiente _p1{ -4.0f, 0.0f, 0.0f, 0.89f, 0.29f, 0.29f }; // Posicion inicial y color de p1 (rojo)
	Combatiente _p2{ 4.0f, 0.0f, 0.0f, 0.39f, 0.60f, 0.13f }; // Posicion inicial y color de p2 (verde)

	ResultadoCombate _resultado{ ResultadoCombate::EnCurso };

	void resolverAtaque(Combatiente& atacante, Combatiente& objetivo); // Resuelve un ataque de un combatiente a otro, comprobando el cooldown, el alcance y aplicando el daño
	void limitarPosicion(Combatiente& c); // Limita la posicion de un combatiente dentro de los bordes de la arena

	IAArena _ia; // CONTROLADOR IA DEL ENEMIGO
	ModoJuego _modo{ ModoJuego::JVIA }; // POR DEFECTO, EL MODO DE JUEGO ES JUGADOR VS IA, PERO SE PUEDE CAMBIAR A JVJ DESDE EL MENU

public:
	// Constructor: crea la arena con los dos combatientes en sus posiciones iniciales
	Arena();

	void actualizar(float dt, InputState& input); // avanza la simulacion un frame
	void reiniciar(); // reinicia el combate, volviendo a las posiciones iniciales y reseteando la vida de los combatientes
	void iniciarCombate(const Pieza& atacante, const Pieza& defensora, ModoJuego modo); // INCIA UN COMBATE CON LAS PIEZAS SELECCIONADAS EN EL MENU
																						// Y EL MODO DE JUEGO (JVJ O JVIA)
	
	// Getters (informacion para el renderer)
	const Combatiente& p1() const { return _p1; } // const, de esta manera el renderer puede leer el combatiente pero no modificarlo
	const Combatiente& p2() const { return _p2; } // & (referencia), devuelve el objeto sin copiarlo
	float ancho() const { return _ancho; }
	float profundo() const { return _profundo; }
	ResultadoCombate resultado() const { return _resultado; }
	void resolverColision(); // EVITA QUE LOS COMBATIENTES SE SOLAPEN
	
	bool iaActiva() const { return _modo == ModoJuego::JVIA; }
};