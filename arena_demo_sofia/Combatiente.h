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

// Direcciones de movimiento del combtiente en la arena
enum class Direccion{
	Delante,
	Atras,
	Izquierda,
	Derecha
};

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

	// Velocidad de movimiento (unidades por segundo)
	float _velocidad{ 4.0f };

	// Movimiento actual pedido por el jugador (se aplica en actualizar())
	float _dxPedido{ 0.0f };
	float _dzPedido{ 0.0f };

	// Tiempo en segundos entre ataques (cooldown)
	float _cooldownAtaque{ 0.5f }; // puede atacar 2 veces por segundo

	// Tiempo acumulado desde el ultimo ataque (se resetea a 0 cada vez que ataca)
	float _tiempoDesdeUltimoAtaque{ 0.5f };

	// Tiempo que dura la animacion de ataque (en segundos)
	float _tiempoAnimacionAtaque{ 0.0f };

	// Alcance del ataque (en unidades)
	float _alcanceAtaque{ 1.5f };

	// Daño que inflige el ataque
	float _danoAtaque{ 12.0f };

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

	// Setters
	// Mueve al combatiente a una nueva posicion concreta del mundo
	void posicion(float x, float y, float z);

	// Resta vida al combatiente. Si la vida llega a 0, el combatiente muere, no puede bajar de 0(se marca como tal, pero no se borra del vector de combatientes)
	void recibirDano(float cantidad);
	bool vivo() const { return _vida > 0.0f; } // Devuelve true si el combatiente sigue vivo (vida > 0), false si ha muerto (vida <= 0)

	// Indica al combatiente que se mueva en una direccion concreta (delante, atras, izquierda, derecha)
	void pedirMovimiento(Direccion d);

	// Avanza la simulacion un frame (aplica el movimiento, los timers,...)
	void actualizar(float dt);

	float alcanceAtaque() const { return _alcanceAtaque; }
	float danoAtaque() const { return _danoAtaque; }
	bool atacando() const { return _tiempoAnimacionAtaque > 0.0f; } // Devuelve true si el combatiente esta en medio de la animacion de ataque, false si no
	bool intentarAtacar(); // Intenta iniciar un ataque. Si el cooldown ha pasado, inicia la animacion de ataque y resetea el timer, devolviendo true. Si el cooldown no ha pasado, devuelve false y no hace nada
};