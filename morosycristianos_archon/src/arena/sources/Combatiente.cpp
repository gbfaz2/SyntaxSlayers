#include "Combatiente.h"
#include <algorithm> // para std::max (asegurar que la vida no es nunca negativa)
#include <cmath>
#include "Arma.h"

Combatiente::Combatiente(float x, float y, float z, float r, float g, float b)
	: _x{ x }, _y{ y }, _z{ z }, _r{ r }, _g{ g }, _b{ b }
{
	_inicioX = x; // GUARDA EL LADO EN EL QUE EMPEZO
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
	if (_dxPedido != 0.0f || _dzPedido != 0.0f)
	{
		// NORMALIZA EL MOVIMIENTO PARA QUE NO SEA MAS RAPIDO EN DIAGONAL
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
	_vida = static_cast<float>(pieza.getVida()) * 1.0f;
	_vidaMax = static_cast<float>(pieza.getVidaMax()); // tampoco existe aun

	// La fuerza de Pieza se convierte en daño del combatiente
	//_danoAtaque = static_cast<float>(pieza.getFuerza());
	_danoAtaque = static_cast<float>(pieza.getFuerza()) * 0.15f;

	// Combinamos recarga y velAtaque para el cooldown real recarga alta = tarda en recargar, velAtaque alta = ataca mas rapido
	float recargaNorm = pieza.getRecarga() / 100.0f;
	float velNorm = pieza.getVelAtaque() / 200.0f;
	_cooldownAtaque = std::max(0.1f, recargaNorm - velNorm);

	// RadioMov → velocidad en la arena
	// radioMov 2 -> velocidad 3.6 u/s (lento)
	// radioMov 5 -> velocidad 6.0 u/s (rapido)
	_velocidad = 2.0f + pieza.getRadioMov() * 0.8f;

	// Alcance segun tipo de arma (dynamic_cast de Pieza a Arma)
	_alcanceAtaque = 1.5f; // por defecto cuerpo a cuerpo
	const Arma* arma = dynamic_cast<const Arma*>(&pieza);
	if (arma && arma->getTipoAtaque() == TipoAtaque::DISTANCIA)
		_alcanceAtaque = 3.5f;
}