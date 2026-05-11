#include "Combatiente.h"
#include <algorithm> // para std::max (asegurar que la vida no es nunca negativa)

Combatiente::Combatiente(float x, float y, float z, float r, float g, float b)
	: _x{ x }, _y{ y }, _z{ z }, _r{ r }, _g{ g }, _b{ b }
{

}

void Combatiente::posicion(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}

void Combatiente::recibirDano(float cantidad)
{
	_vida = std::max(0.0f, _vida - cantidad); // resto vida, pero no dejo que baje de 0
}