//aqui ya si que va la implementación de la lógica del tablero
#include "Tablero.h"

void Tablero::inicializarZonas() //(fila+col) par-> ZONA_LOCAL, impar-> ZONA VISITANTE
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			if ((fila + col) % 2 == 0) {
				tabla[fila][col].setZona(ZONA_LOCAL);
				tabla[fila][col].setColorActual(COLOR_LOCAL);
			}
			else {
				tabla[fila][col].setZona(ZONA_VISITANTE);
				tabla[fila][col].setColorActual(COLOR_VISITANTE);
			}
		}
	}
}

//Marcamos los puntos de poder, es decir : (4,4) centro, (0,4) arriba, (8,4) abajo, (4,0) izquierda, (4,8) derecha
void Tablero::marcarPuntosPoder()
{
	const int posiciones[5][2] = { {4,4},{0,4},{8,4},{4,0},{4,8} };
	for (auto& p : posiciones)
		tabla[p[0]][p[1]].setZona(PUNTO_PODER);
}

//Ponemos un tercio de las casillas normales en dinamicas, para lo que seguiremos el patron
void Tablero::marcarDinamicas()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			if (tabla[fila][col].getZona() == PUNTO_PODER) continue;
			if ((fila + col) % 3 == 0) {
				tabla[fila][col].setZona(DINAMICA);
				tabla[fila][col].setColorActual(COLOR_LOCAL);
			}
		}
	}
}


//Esto solo actuara cuando el turnoActual sea multiplo de cadaCuantosTurnos
void Tablero::avanzarOscilacion(int turnoActual, int cadaCuantosTurnos)
{
	if (turnoActual % cadaCuantosTurnos != 0) return;
	for (int fila = 0; fila < N; fila++)
		for (int col = 0; col < N; col++)
			if (tabla[fila][col].getZona() == DINAMICA)
				tabla[fila][col].invertirColor();
}

//punto_poder->neutro, color actual del equipo->ventaja, del rival->desventaja
VentajaCombate Tablero::getVentaja(int fila, int col, TipoOcupante quien) const
{
	if (tabla[fila][col].getZona() == PUNTO_PODER)return NEUTRO;
	bool esColorLocal = (tabla[fila][col].getColorActual() == COLOR_LOCAL);
	if (quien == LOCAL)
		return esColorLocal ? VENTAJA : DESVENTAJA;
	else 
		return esColorLocal ? DESVENTAJA : VENTAJA;
}

//controlar los cinco puntos de victoria es una de las condiciones de victoria
bool Tablero::controlCincoPuntos(TipoOcupante quien) const
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			if (tabla[fila][col].getZona() == PUNTO_PODER && tabla[fila][col].getOcupadaPor() != quien)
				return false;
		}
	}
	return true;
}


//establecer el ocupante que hay en la casilla
void Tablero::establecerOcupante(int fila, int col, TipoOcupante quien)
{
	if (enTablero(fila, col))
		tabla[fila][col].setOcupadaPor(quien);
}

//la seleccion de la casilla que se hace y la limpieza de la casilla que se ha seleccionado para poder volver a seleccionar
void Tablero::seleccionar(int fila, int col)
{
	limipiarSeleccion();
	if (enTablero(fila, col)) {
		tabla[fila][col].setSeleccionada(true);
		filaSelec = fila;
		colSelec = col;
	}
}

void Tablero::limipiarSeleccion()
{
	if (haySeleccion())
		tabla[filaSelec][colSelec].setSeleccionada(false);
	filaSelec = colSelec = -1;
}



