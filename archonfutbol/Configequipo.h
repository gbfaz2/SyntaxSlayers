#pragma once
#include <string>

using namespace std;

//identificador del equipo
enum TipoEquipo {BARSA, ATLETICO, MADRID};

//Color RGB para OpenGL
struct Color3f {
	float r, g, b;
	Color3f(float rr=1.f, float gg=1.f, float bb=1.f): r(rr), g(gg), b(bb){}
};

//Toda la información de un equipo en un sitio
struct ConfigEquipo {
	TipoEquipo id;
	string nombre;
	Color3f colorCasilla; //color de sus casillas en el tablero
	Color3f colorPieza; //color del circulo de sus piezas
};

//Devuelve la configuracion del equipo correspondiente segun su id
// Si hay que cambiar un color, solo se toca aquí
inline ConfigEquipo crearConfigEquipo(TipoEquipo id) {
	switch (id) {
	case BARSA:
		return { BARSA, "FC Barcelona", Color3f(0.07f,0.30f,0.73f), Color3f(0.04f,0.18f,0.55f) };
	case ATLETICO:
		return { ATLETICO, "Atletico de Madrid", Color3f(0.80f,0.06f,0.10f), Color3f(0.60f,0.04f,0.08f) };
	case MADRID: default:
		return { MADRID, "Real Madrid", Color3f(0.92f,0.92f,0.92f), Color3f(0.65f, 0.65f, 0.65f) };
	}
}
