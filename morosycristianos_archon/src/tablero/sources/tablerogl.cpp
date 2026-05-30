// Aquí se va a dibujar el tablero. 
// Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
// Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include "MinimaxTablero.h"
#include <cmath>

int Tablerogl::_anchoVentana = 1024;
int Tablerogl::_altoVentana = 768;

using namespace std;

Tablerogl::Tablerogl(Tablero* pb) :m_tablero(pb)
{
	N = pb->getSize(); //Siempre va a ser nueve, pero para asegurarnos mejor leerlo directamente de nuestra clase tablero
	ancho = 0.12f; //ancho de una casilla en unidades Opengl
	dist = 2.0f; //distancia que hay de la cámara al tablero, el centro del tablero es donde va a apuntar la cámara
	//x positivo crece hacia la derecha, y negativo crece hacia abajo
	centro_x = N * ancho / 2.0;
	centro_y = -N * ancho / 2.0;
	centro_z = 0.0;

	Filacursor[0] = 4; Colcursor[0] = 1; //Cursor local
	Filacursor[1] = 4; Colcursor[1] = 7; //Cursor rival

	xcasilla_sel = -1; //todavía no hay casilla seleccionada
	ycasilla_sel = -1;

	fromFila = fromCol = -1;
	fromBando = bando_nada;
	piezaSeleccionada = false; //no hay pieza seleccionada

	victoria_ = bando_nada; //la partida sigue en curso, nadie ha ganado

	leftButton = rightButton = midButton = false;
	controlKey = shiftKey = false;
}

void Tablerogl::trySelectorMove(BandoPieza bando)
{
	//Obtenemos las coordenadas a las que apunta el cursor o el ratón en este momento
	int idx = (bando == bando_local) ? 0 : 1;
	int currentFila = Filacursor[idx];
	int currentCol = Colcursor[idx];

	// Si NO hay pieza seleccionada, estamos en FASE DE SELECCIÓN
	if (!piezaSeleccionada) {

		// EN MODO JvsIA EL JUGADOR SOLO PUEDE MOVER EL BANDO LOCAL
		if (_modoJVIA && bando == bando_rival) return;

		// Comprobamos si la pieza en esa casilla pertenece al bando que tiene el turno
		if (gestorTurnos.esDelBandoActual(*m_tablero, currentFila, currentCol)) {
			// Es válida. Guardamos el origen y marcamos como seleccionada
			fromFila = currentFila;
			fromCol = currentCol;
			fromBando = bando;
			piezaSeleccionada = true;

			// SUGERENCIA: calcular el mejor movimiento para la pieza seleccionada
			// Usamos una IA ligera (profundidad 1) para que sea instantáneo
			_sugerenciaFila = -1;
			_sugerenciaCol = -1;

			auto casillas = m_tablero->casillasValidas(currentFila, currentCol);

			if (!casillas.empty()) {
				Tablero copia = *m_tablero;
				MinimaxTablero iaLigera(1);

				int mejorValor = (bando == bando_rival) ? INT_MIN : INT_MAX;
				for (const auto& pos : casillas) {
					MovimientoIA mov{ currentFila, currentCol, pos.fila, pos.col };

					EstadoCasilla estado = iaLigera.aplicarMovimiento(copia, mov);
					int valor = iaLigera.evaluar(copia);
					iaLigera.deshacerMovimiento(copia, mov, estado);

					bool esMejor = (bando == bando_rival) ? (valor > mejorValor) : (valor < mejorValor);
					if (esMejor) {
						mejorValor = valor;
						_sugerenciaFila = pos.fila;
						_sugerenciaCol = pos.col;
					}
				}
			}
		}
		// Si no es su turno o la casilla está vacía, no hace nada y sale
		return;
	}

	// Si YA HAY pieza seleccionada, estamos en FASE DE MOVIMIENTO
	else {

		Pieza* pieza = m_tablero->getCasilla(fromFila, fromCol).obj;

		if (!pieza) {
			piezaSeleccionada = false; // Por seguridad, si la pieza desapareció
			return;
		}

		// COMPROBACIÓN DE ASEDIO
		if (_gestorHechizos && _gestorHechizos->estaBloqueada(fromFila, fromCol)) {
			mostrarMensajeInvalido("Pieza bloqueada por Asedio!");
			piezaSeleccionada = false;
			fromFila = fromCol = -1;
			return;
		}

		//guardamos el origen ANTES
		int origenFila = fromFila;
		int origenCol = fromCol;

		// Intentamos mover la pieza desde el origen al destino (currentFila, currentCol)
		ResultadoMovimiento resultado = gestorMovimiento.resolverMovimiento(
			pieza, *m_tablero, currentFila, currentCol
		);

		if (resultado == ResultadoMovimiento::MOVIMIENTO_OK) {
			// Origen: donde estaba ANTES del movimiento
			float ox, oy;
			cell2center(origenFila, origenCol, ox, oy);

			// Destino: donde está AHORA lógicamente
			float dx, dy;
			cell2center(currentFila, currentCol, dx, dy);

			_animMov.pieza = pieza;
			_animMov.origenX = ox;  _animMov.origenY = oy;
			_animMov.destinoX = dx;  _animMov.destinoY = dy;
			_animMov.t = 0.0f;
			_animMov.activa = true;

			piezaSeleccionada = false;
			fromFila = fromCol = -1;
		}


		else if (resultado == ResultadoMovimiento::COMBATE) {

			// 1. CALCULAMOS COORDENADAS PARA LA ANIMACIÓN (IGUAL QUE EN MOVIMIENTO_OK)
			float ox, oy, dx, dy;
			cell2center(fromFila, fromCol, ox, oy);
			cell2center(currentFila, currentCol, dx, dy);

			_animMov.pieza = pieza;
			_animMov.origenX = ox;  _animMov.origenY = oy;
			_animMov.destinoX = dx;  _animMov.destinoY = dy;
			_animMov.t = 0.0f;
			_animMov.activa = true; // ARRANCAMOS LA ANIMACIÓN VISUAL HACIA EL ENEMIGO

			piezaSeleccionada = false;
			fromFila = fromCol = -1;

			// 2. PREPARAMOS EL COMBATE, PERO AÚN NO AVISAMOS AL COORDINADOR
			Pieza* atacante = gestorMovimiento.getUltimoAtacante();
			Pieza* defensora = gestorMovimiento.getUltimaDefensora();

			if (atacante && defensora) {
				_pAtacante = atacante;
				_pDefensora = defensora;
			}
		}
		// Si el movimiento es INVÁLIDO o BLOQUEADO, la pieza sigue seleccionada
		// esperando a que elijas un destino válido (o puedes cancelar la selección si prefieres).
		else {
			if (resultado == ResultadoMovimiento::BLOQUEADO_ALIADO)
				mostrarMensajeInvalido("Casilla bloqueada por aliado");
			else
				mostrarMensajeInvalido("Movimiento invalido");
		}
	}
}

void Tablerogl::cell2center(int casilla_x, int casilla_y, float& glx, float& gly)
{
	glx = casilla_y * ancho + ancho / 2.0f;
	gly = -casilla_x * ancho - ancho / 2.0f;
}

void Tablerogl::world2cell(double x, double y, int& casilla_x, int& casilla_y)
{
	casilla_x = (int)(fabs(y / ancho));
	casilla_y = (int)(x / ancho);
}

int Tablerogl::getVentajaTerrenoCombate() const
{
	// combate ocurre en la casilla de la pieza defensora
	if (_pDefensora) { 
		TipoCasilla tipo = m_tablero->getCasilla(_pDefensora->getFila(), _pDefensora->getColumna()).tipo;
		if (tipo == Casilla_local) return 1;
		if (tipo == Casilla_rival) return 2;
	}
	return 0;
}

void Tablerogl::limpiarCombate()
{
	_combatePendiente = false;
	_pAtacante = nullptr;
	_pDefensora = nullptr; 

}

void Tablerogl::redimensionar(int ancho, int alto) {
	_anchoVentana = ancho;
	_altoVentana = (alto == 0) ? 1 : alto;

	return;
}

void Tablerogl::aplicarCambiosDinamicos()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			Casilla& cas = m_tablero->getCasilla(fila, col);
			if (cas.tipo == Casilla_local) {
				cas.tipo = Casilla_rival;
			}
			else if (cas.tipo == Casilla_rival) {
				cas.tipo = Casilla_local;
			}
		}
	}
}

//update para la interpolación en el tablero
void Tablerogl::update(double dt)
{
	if (_animMov.activa) {
		_animMov.t += (float)dt * 2.0f; // VELOCIDAD: 2 
		if (_animMov.t >= 1.0f) {
			_animMov.t = 1.0f;
			_animMov.activa = false;

			// LA PIEZA YA HA LLEGADO VISUALMENTE A SU DESTINO. 
			// COMPROBAMOS SI HABÍA UN COMBATE EN ESPERA
			if (_pAtacante != nullptr && _pDefensora != nullptr) {
				_combatePendiente = true; // AHORA SÍ, QUE SALTE LA ARENA TRAS EL CHOQUE
			}
			else {
				gestorTurnos.terminarTurno(); // SI NO HAY PELEA, CAMBIA EL TURNO NORMALMENTE
			}
		}
	}

	//CURACIÓN DE LOS PUNTOS DE PODER
	_tiempoCuracionPoder += (float)dt;

	// Se ejecuta 1 vez cada segundo
	if (_tiempoCuracionPoder >= 1.0f) {
		_tiempoCuracionPoder = 0.0f; // Reiniciamos el contador

		// Recorremos todo el tablero buscando piezas en Puntos de Poder
		for (int fila = 0; fila < N; fila++) {
			for (int col = 0; col < N; col++) {
				Casilla& cas = m_tablero->getCasilla(fila, col);
				// Si la casilla es Punto de Poder y hay una pieza en ella
				if (cas.tipo == Casilla_poder && cas.obj != nullptr) {
					// Comprobamos si tiene menos vida de la máxima
					if (cas.obj->getVida() < cas.obj->getVidaMax()) {
						// La curamos con 1 punto de vida cada segundo
						cas.obj->curar(1);
					}
				}
			}
		}
	}
}