#include "Combatiente.h"
#include <algorithm> // para std::max (asegurar que la vida no es nunca negativa)
#include <cmath>

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

void Combatiente::pedirMovimiento(Direccion d) 
{
	// Acumulamos la direccion pedida
	switch (d)
	{
	case Direccion::Delante: _dzPedido -= 1.0f; break; // -Z hacia el fondo
	case Direccion::Atras: _dzPedido += 1.0f; break; // +Z hacia la camara
	case Direccion::Izquierda: _dxPedido -= 1.0f; break; // -X hacia la izquierda
	case Direccion::Derecha: _dxPedido += 1.0f; break; // +X hacia la derecha
	}
}

void Combatiente::actualizar(float dt) 
{
	// Aplico el movimiento pedido por el jugador
	if (_dxPedido != 0.0f || _dzPedido != 0.0f) 
	{
		// Normalizamos el movimiento para que no sea mas rapido en diagonal
		float longitud = std::sqrt(_dxPedido * _dxPedido + _dzPedido * _dzPedido);
		float nx = _dxPedido / longitud;
		float nz = _dzPedido / longitud;

		_x += nx * _velocidad * dt;
		_z += nz * _velocidad * dt;
	}

	// Reseteamos el movimiento pedido para el siguiente frame
	_dxPedido = 0.0f;
	_dzPedido = 0.0f;

	// Avanzar timers de ataque
	_tiempoDesdeUltimoAtaque += dt;
	if (_tiempoAnimacionAtaque > 0.0f) 
	{
		_tiempoAnimacionAtaque -= dt;
		if (_tiempoAnimacionAtaque < 0.0f)
			_tiempoAnimacionAtaque = 0.0f; // Aseguramos que no baje de 0
	}
}

bool Combatiente::intentarAtacar() 
{
	if (_tiempoDesdeUltimoAtaque < _cooldownAtaque)
		return false; // El cooldown no ha pasado, no se puede atacar

	// Reseteamos el timer de ataque y empezamos la animacion de ataque
	_tiempoDesdeUltimoAtaque = 0.0f;
	_tiempoAnimacionAtaque = 0.18f; // por ejemplo, la animacion dura 0.18 segundos
	return true;
}

Combatiente::Combatiente(float x, float z, float r, float g, float b, const Pieza& pieza)
	: _x{ x }, _y{ 0.0f }, _z{ z },
	_r{ r }, _g{ g }, _b{ b },
	_nombre{ pieza.getNombre() }
{
	// Usamos los stats reales de la pieza del tablero
	// La vida de Pieza es int (0-100), la convertimos a float
	_vida = static_cast<float>(pieza.getVida());  // no existe aun, lo añadimos luego
	_vidaMax = static_cast<float>(pieza.getVidaMax()); // tampoco existe aun

	// La fuerza de Pieza se convierte en daño del combatiente
	_danoAtaque = static_cast<float>(pieza.getFuerza());

	// La recarga de Pieza se convierte en cooldown (normalizamos de 0-100 a 0-2 segundos)
	// recarga 20 (baja) -> cooldown 0.2s (ataca rapido)
	// recarga 90 (alta) -> cooldown 0.9s (ataca lento)
	_cooldownAtaque = pieza.getRecarga() / 100.0f;
}