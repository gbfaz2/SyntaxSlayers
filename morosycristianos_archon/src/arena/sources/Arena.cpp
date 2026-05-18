#include "Arena.h"
#include <cmath> // para std::abs en resolverAtaque

// Constructor: crea la arena con los dos combatientes en sus posiciones iniciales
Arena::Arena()
{
	// No necesito hacer nada aqui, porque ya he inicializado los combatientes con sus posiciones y colores en la declaracion de la clase
}

void Arena::reiniciar()
{
	// Coloco de nuevo a los combatientes en sus posiciones iniciales
	_p1.posicion(-4.0f, 0.0f, 0.0f);
	_p2.posicion(4.0f, 0.0f, 0.0f);

	_resultado = ResultadoCombate::EnCurso; // Reinicio el resultado del combate
}

void Arena::actualizar(float dt, InputState& input)
{
	// Si el combate ya ha terminado, no hago nada
	if (_resultado != ResultadoCombate::EnCurso) return;

	if (_contraIA) _ia.actualizar(_p2, _p1, input.p2, dt); // SOLO ACTIVA LA IA SI ESTA EN MODO PVC

	// Proceso el input del jugador 1 (Cristiano)
	if (input.p1.delante) _p1.pedirMovimiento(Direccion::Delante);
	if (input.p1.atras) _p1.pedirMovimiento(Direccion::Atras);
	if (input.p1.izquierda) _p1.pedirMovimiento(Direccion::Izquierda);
	if (input.p1.derecha) _p1.pedirMovimiento(Direccion::Derecha);
	if (input.p1.atacar) resolverAtaque(_p1, _p2); // Si el jugador ha pedido atacar, intento resolver el ataque

	// Proceso el input del jugador 2 (Andalusi)
	if (input.p2.delante) _p2.pedirMovimiento(Direccion::Delante);
	if (input.p2.atras) _p2.pedirMovimiento(Direccion::Atras);
	if (input.p2.izquierda) _p2.pedirMovimiento(Direccion::Izquierda);
	if (input.p2.derecha) _p2.pedirMovimiento(Direccion::Derecha);
	if (input.p2.atacar) resolverAtaque(_p2, _p1);

	// Hay que consumir los pulsos de ataque para que no se repitan
	input.consumirAtaques();

	// Actualizo a los combatientes (aplico el movimiento, los timers,...)
	_p1.actualizar(dt);
	_p2.actualizar(dt);

	// Limito la posicion dentro de la arena
	limitarPosicion(_p1);
	limitarPosicion(_p2);

	// Compruebo la condicion de victoria
	if(!_p1.vivo() && !_p2.vivo()) _resultado = ResultadoCombate::Empate; // Si los dos han muerto, es empate
	else if (!_p1.vivo()) _resultado = ResultadoCombate::GanaP2; // Si p1 ha muerto, gana p2
	else if (!_p2.vivo()) _resultado = ResultadoCombate::GanaP1; // Si p2 ha muerto, gana p1
}

void Arena::resolverAtaque(Combatiente& atacante, Combatiente& objetivo) 
{
	// Intentamos atacrar. Si el cooldown no ha pasado, no hacemos nada (devuelve false)
	if (!atacante.intentarAtacar()) return;

	// Calculo la distancia entre el atacante y el objetivo en el plano XZ
	float dx = objetivo.x() - atacante.x();
	float dz = objetivo.z() - atacante.z();
	float distancia = std::sqrt(dx * dx + dz * dz);

	// Si el objetivo esta dentro del alcance del ataque, le restamos vida
	if (distancia <= atacante.alcanceAtaque() + objetivo.lado() * 0.5f)
		objetivo.recibirDano(atacante.danoAtaque());
}

void Arena::limitarPosicion(Combatiente& c) 
{
	// Evitar que el combatiente salga de la arena, limitando su posicion a las dimensiones del escenario
	float halfX = _ancho * 0.5f;
	float halfZ = _profundo * 0.5f;
	float r = c.lado() * 0.5f; // Radio del cubo (la mitad del lado), para que no se quede "pillado" en los bordes

	float x = c.x();
	float z = c.z();

	if (x < -halfX + r) x = -halfX + r;
	if (x > halfX - r) x = halfX - r;
	if (z < -halfZ + r) z = -halfZ + r;
	if (z > halfZ - r) z = halfZ - r;

	c.posicion(x, c.y(), z);
}

void Arena::iniciarCombate(const Pieza& atacante, const Pieza& defensora)
{
	// Creamos los combatientes con las stats reales de las piezas del tablero P1 (atacante) siempre empieza a la izquierda, color segun bando
	float r1 = atacante.getBando() == Bando::CRISTIANO ? 0.89f : 0.39f;
	float g1 = atacante.getBando() == Bando::CRISTIANO ? 0.29f : 0.60f;
	float b1 = atacante.getBando() == Bando::CRISTIANO ? 0.29f : 0.13f;

	// P2 (defensora) siempre empieza a la derecha, color segun bando
	float r2 = defensora.getBando() == Bando::CRISTIANO ? 0.89f : 0.39f;
	float g2 = defensora.getBando() == Bando::CRISTIANO ? 0.29f : 0.60f;
	float b2 = defensora.getBando() == Bando::CRISTIANO ? 0.29f : 0.13f;

	// Sustituimos los combatientes con los datos reales
	_p1 = Combatiente(-4.0f, 0.0f, r1, g1, b1, atacante);
	_p2 = Combatiente(4.0f, 0.0f, r2, g2, b2, defensora);

	// Reseteamos el resultado
	_resultado = ResultadoCombate::EnCurso;
}