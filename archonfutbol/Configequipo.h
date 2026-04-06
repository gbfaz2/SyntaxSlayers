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
	Color3f(int rr = 255, int gg = 255, int bb = 255) : r(rr), g(gg), b(bb);

	//Getters en bits (0-255): para mostrar o comparar colores

};