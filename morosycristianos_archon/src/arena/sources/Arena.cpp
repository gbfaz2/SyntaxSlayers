#include "Arena.h"
#include <cmath> // para std::abs en resolverAtaque
#include <iostream>

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
	// SI EL COMBATE YA HA TERMINADO, NO HACEMOS NADA
	if (_resultado != ResultadoCombate::EnCurso) return;

	// 1. ORIENTA CADA COMBATIENTE HACIA SU ENEMIGO ANTES DE NADA
	auto orientarHacia = [](Combatiente& yo, const Combatiente& enemigo) {
		float dx = enemigo.x() - yo.x();
		float dz = enemigo.z() - yo.z();
		float dist = std::sqrt(dx * dx + dz * dz);
		if (dist > 0.001f)
			yo.setMirando(dx / dist, dz / dist);
		};
	orientarHacia(_p1, _p2);
	orientarHacia(_p2, _p1);

	// 2. LA IA RELLENA EL INPUT DE P2 ANTES DE PROCESAR ATAQUES
	if (_modo == ModoJuego::JVIA)
		_ia.actualizar(_p2, _p1, input.p2, dt);

	// 3. RESUELVE ATAQUES CON LA ORIENTACION YA ACTUALIZADA
	if (input.p1.atacar) resolverAtaque(_p1, _p2);
	if (input.p2.atacar) resolverAtaque(_p2, _p1);

	// 4. PROCESA MOVIMIENTO
	if (input.p1.delante)   _p1.pedirMovimiento(Direccion::Delante);
	if (input.p1.atras)     _p1.pedirMovimiento(Direccion::Atras);
	if (input.p1.izquierda) _p1.pedirMovimiento(Direccion::Izquierda);
	if (input.p1.derecha)   _p1.pedirMovimiento(Direccion::Derecha);

	if (input.p2.delante)   _p2.pedirMovimiento(Direccion::Delante);
	if (input.p2.atras)     _p2.pedirMovimiento(Direccion::Atras);
	if (input.p2.izquierda) _p2.pedirMovimiento(Direccion::Izquierda);
	if (input.p2.derecha)   _p2.pedirMovimiento(Direccion::Derecha);

	// 5. CONSUME PULSOS DE ATAQUE PARA QUE NO SE REPITAN
	input.consumirAtaques();

	// 6. AVANZA LA SIMULACION (YA NO TOCA _mirandoX)
	_p1.actualizar(dt);
	_p2.actualizar(dt);

	// 7. LIMITA POSICION DENTRO DE LA ARENA
	limitarPosicion(_p1);
	limitarPosicion(_p2);

	// 8. EVITA QUE LOS CUBOS SE SOLAPEN
	resolverColision();

	// 9. COMPRUEBA CONDICION DE VICTORIA
	if (!_p1.vivo() && !_p2.vivo()) _resultado = ResultadoCombate::Empate;
	else if (!_p1.vivo())           _resultado = ResultadoCombate::GanaP2;
	else if (!_p2.vivo())           _resultado = ResultadoCombate::GanaP1;
}

void Arena::resolverAtaque(Combatiente& atacante, Combatiente& objetivo)
{
	if (!atacante.intentarAtacar()) return;

	float dx = objetivo.x() - atacante.x();
	float dz = objetivo.z() - atacante.z();
	float distancia = std::sqrt(dx * dx + dz * dz);

	if (distancia <= atacante.alcanceAtaque() + objetivo.lado() * 0.5f)
	{
		// SI EL ATACANTE EMPEZO A LA IZQUIERDA (x<0), SOLO PUEDE GOLPEAR A ALGUIEN QUE ESTE A SU DERECHA (dx>0)
		// SI EL ATACANTE EMPEZO A LA DERECHA (x>0), SOLO PUEDE GOLPEAR A ALGUIEN QUE ESTE A SU IZQUIERDA (dx<0)
		bool atacanteEmpezóIzquierda = (atacante.inicioX() < 0);
		bool objetivoEstaALaDerecha = (dx > 0);

		if (atacanteEmpezóIzquierda == objetivoEstaALaDerecha)
			objetivo.recibirDano(atacante.danoAtaque());
	}
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

void Arena::iniciarCombate(const Pieza& atacante, const Pieza& defensora, ModoJuego modo)
{
	_modo = modo;
	std::cout << "[Arena] iniciarCombate llamado!" << std::endl;

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

	
	_ia.configurar(_p2.alcanceAtaque());

	// Reseteamos el resultado
	_resultado = ResultadoCombate::EnCurso;

	std::cout << "[Arena] vida p1: " << _p1.vida() << " vida p2: " << _p2.vida() << std::endl;
	std::cout << "[Arena] resultado = EnCurso" << std::endl;
}

void Arena::resolverColision()
{
	// DIFERENCIA DE POSICION ENTRE P2 Y P1 EN CADA EJE
	float dx = _p2.x() - _p1.x();
	float dz = _p2.z() - _p1.z();

	// DISTANCIA REAL ENTRE LOS CENTROS DE LOS DOS CUBOS
	float distancia = std::sqrt(dx * dx + dz * dz);

	// DISTANCIA MINIMA PERMITIDA: SUMA DE LOS RADIOS DE AMBOS CUBOS
	float separacionMinima = (_p1.lado() + _p2.lado()) * 0.5f;

	// SOLO ACTUAMOS SI SE ESTAN SOLAPANDO Y LA DISTANCIA NO ES CERO (EVITA DIVISION POR CERO)
	if (distancia < separacionMinima && distancia > 0.001f)
	{
		// VECTOR NORMALIZADO QUE APUNTA DE P1 HACIA P2
		float nx = dx / distancia;
		float nz = dz / distancia;

		// MITAD DEL SOLAPAMIENTO: CADA UNO SE MUEVE LA MISMA CANTIDAD
		float overlap = (separacionMinima - distancia) * 0.5f;

		// P1 SE ALEJA EN DIRECCION CONTRARIA A P2
		_p1.posicion(_p1.x() - nx * overlap, _p1.y(), _p1.z() - nz * overlap);

		// P2 SE ALEJA EN DIRECCION CONTRARIA A P1
		_p2.posicion(_p2.x() + nx * overlap, _p2.y(), _p2.z() + nz * overlap);
	}
}