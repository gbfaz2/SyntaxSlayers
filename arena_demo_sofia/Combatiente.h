#pragma once

// Unidad que pelea dentro de la arena de combate
// Representa a un soldado del bando Cristiano o Andalusi en la fase
// de combate cuerpo a cuerpo del juego.

// SISTEMA DE COORDENADAS
// -X: eje horizontal (izquierda<--->derecha)
// -Y: eje vertical (suelo = 0, cielo = positivo)
// -Z: eje horizontal (cerca<--->lejos de la camara)

// Esta clase es la versión "demo" inicial. Cuando este lista la
// jerarquia de clases de Alba, adaptaremos esta para
// integrarse con ella

class Combatiente
{
	// Posicion en la arena
	float _x{ 0.0f };
	float _y{ 0.0f };
	float _z{ 0.0f };

	// Vida
	float _vida{ 100.0f };
	float _vidaMax{ 100.0f };

	// Color del cubo
	float _r{ 1.0f };
	float _g{ 1.0f };
	float _b{ 1.0f };

	// Tamaño del cubo
	float _lado{ 1.0f };

public:
	// Constructor: crea un combatiente con la posicion y el color dados
	Combatiente(float x, float y, float z, float r, float g, float b); // y valdra 0 siempre a no ser que metamos voladores
};