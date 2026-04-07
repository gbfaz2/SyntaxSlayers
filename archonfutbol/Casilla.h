//este archivo sirve para definir los datos de cada casilla del tablero

#pragma once
#include <iostream>
using namespace std;

//el tipo de zona es permanente, no cambia nunca 
enum TipoZona {
	ZONA_LOCAL, //color fijo del equipo que se elija como local
	ZONA_VISITANTE, //color fijo del equipo que se elija como visitante
	DINAMICA, //casillas que cambian de color, oscilan entre los dos colores de local y visitante cada N turnos
	PUNTO_PODER //son las 5 zonas "VIP", que estarán en dorado
};

//el color visible en el momento en la casilla-> solo cambia en las dinamicas cuando oscila de color
enum ColorVisible {
	COLOR_LOCAL,
	COLOR_VISITANTE
};

//quien esta ocupando la casilla
enum TipoOcupante {
	NADIE,
	LOCAL,
	VISITANTE
};

//Ventaja de combate que da la casilla. ¡IMPORTANTE PARA SOFIA O GABRI QUE LUEGO NECESITAREIS LLAMAR A TABLERO::GETVENTAJA() AL INICIAR UN DUELO ENTRE PERSONAJES
enum VentajaCombate {
	VENTAJA,
	DESVENTAJA,
	NEUTRO
};

//creo la clase de casilla
class Casilla {
protected:
	TipoZona zona; //zona permanente (la asigna el Tablero al crear)
	ColorVisible colorActual; //lo que se ve ahora
	TipoOcupante ocupadaPor; //pieza encima
	bool seleccionada; //seleccionada con el raton
public:
	//constructor de casilla vacia, zona local y color local
	Casilla():zona(ZONA_LOCAL),colorActual(COLOR_LOCAL),ocupadaPor(NADIE),seleccionada(false){}
	//Getters
	TipoZona getZona() const { return zona; }
	ColorVisible getColorActual() const { return colorActual; }
	TipoOcupante getOcupadaPor() const { return ocupadaPor; }
	bool isSeleccionada() const { return seleccionada; }
	//Setters
	void setZona(TipoZona z) { zona = z; }
	void setColorActual(ColorVisible c) { colorActual = c ; }
	void setOcupadaPor(TipoOcupante o) { ocupadaPor = o; }
	void setSeleccionada(bool s) { seleccionada = s; }
	//para las casillas dinamicas invierte el color
	void invertirColor() {
		colorActual = (colorActual == COLOR_LOCAL) ? COLOR_VISITANTE : COLOR_LOCAL;
	}
};