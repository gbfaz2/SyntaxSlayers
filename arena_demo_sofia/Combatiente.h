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

	// Añado getters. Los implemento aqui porque son muy cortos (funcion inline), asi si hay que añadir otro getter, solo toco este archivo
	// Devuelven informacion sobre el combatiente. Son const, porque no modifican el objeto
	// Posición
	float x() const { return _x; }
	float y() const { return _y; }
	float z() const { return _z; }

	// Vida
	float vida() const { return _vida; }
	float vidaMax() const { return _vidaMax; }

	// Color (devuelvo los tres por sepaoarado para que el renderer los pase a glColor3f directamente)
	float r() const { return _r; }
	float g() const { return _g; }
	float b() const { return _b; }

	// Tamaño
	float lado() const { return _lado; }
};