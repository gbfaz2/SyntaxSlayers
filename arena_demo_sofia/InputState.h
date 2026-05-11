#pragma once

// Capa de abstraccion entre la fuente de input 
// (teclado, mando, IA,...) y la logica de la arena

// La arena no preguntara si la tecla esta pulsada, sino que preguntara
// "el jugador ha pedido moverse a la derecha?" y esta clase se encargara de
// traducir esa pregunta a la fuente de input correspondiente

// Esto quiere decir que si en el futuro añadimos el mando de la play o la IA, solo cambiara
// quien rellena este struct, pero la arena seguira intacta, preguntando lo mismo.

// Utilizo struct y no class porque aqui no tengo logica, encapsulacion, herencia,...
// solo quiero un contenedor de datos, y con struct me ahorro escribir public: y los getters/setters

struct EstadoJugador {
	bool delante{ false };
	bool atras{ false };
	bool izquierda{ false };
	bool derecha{ false };
	bool atacar{ false }; // true si el jugador ha pedido atacar, false si no
};

// Estado completo de los dos jugadores
struct InputState{
	EstadoJugador p1; // Cristiano (WASD + F)
	EstadoJugador p2; // Andalusi (Flechas + L)

	// Limpiamos los pulsos de ataque cada frame, para que no se queden "pillados" si el jugador pulsa una vez y luego suelta la tecla
	void consumirAtaques() {
		p1.atacar = false;
		p2.atacar = false;
	}
};