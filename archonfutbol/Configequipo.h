#pragma once
#include <string>

using namespace std;

//identificador del equipo
enum TipoEquipo {BARSA, ATLETICO, MADRID};

//Clase de configuración de color, que guarda el color en bits de 0 a 255 y lo convierte a float. Ponemos los datos privados para evitar los valores invalidos
class Color3f {
private:
	int r, g, b; //valores de 0 a 255
public:
	//Constructor con valores de 0 a 255
	Color3f(int rr = 255, int gg = 255, int bb = 255) : r(rr), g(gg), b(bb){}

	//Getters en bits (0-255): para mostrar o comparar colores
	int getR() const { return r; }
	int getG() const { return g; }
	int getB() const { return b; }

	//Getters en float: es solo para pasarle el color a OperGL ya que este trabaja con floats (hacemos conversión de ints a floats)
	float getRf() const { return r / 255.0f; }
	float getGf() const { return g / 255.0f; }
	float getBf() const { return b / 255.0f; }

	//devuelve una version mas oscura del color, esto será util para distinguir casillas DINAMICAS del tablero del resto
	Color3f oscurecer(float factor = 0.72f) const {
		return Color3f((int)(r*factor), (int)(g * factor), (int)(b * factor));
	}
};

class ConfigEquipo {
private:
	TipoEquipo id;
	string nombre;
	Color3f colorCasilla; //el color de sus casillas en el tablero
	Color3f colorPieza; //color del circulo de sus piezas
public:
	//Constructor por defecto 
	ConfigEquipo():id(MADRID), nombre(""), colorCasilla(255,255,255), colorPieza(200,200,200) {}

	//constructor completo (usado por el método estatico de crear)
	ConfigEquipo(TipoEquipo _id, string _nombre, Color3f _casilla, Color3f _pieza):id(_id), nombre(_nombre), colorCasilla(_casilla), colorPieza(_pieza){}

	//Los getters para obtener los datos privados de la clase del equipo
	TipoEquipo getId() const { return id; }
	string getNombre() const { return nombre; }
	Color3f getColorCasilla() const { return colorCasilla; }
	Color3f getColorPieza() const { return colorPieza; }

	//Metodo estatico de de fabricacion, para definir los colores de cada equipo: BARSA->AZUL, ATLETICO->ROJO, MADRID->BLANCO
	static ConfigEquipo crear(TipoEquipo id) {
		switch (id) {
		case BARSA:
			return ConfigEquipo(BARSA, "FC Barcelona", Color3f(18, 77, 186), Color3f(10, 46, 140));
		case ATLETICO:
			return ConfigEquipo(ATLETICO, "Atletico de Madrid", Color3f(204, 15, 26), Color3f(153, 10, 20));
		case MADRID:
			return ConfigEquipo(MADRID, "Real Madrid", Color3f(235, 235, 235), Color3f(166, 166, 166));
		}
	}
};