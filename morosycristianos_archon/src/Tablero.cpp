//Tablero.cpp: implementación del tablero. Primero voy a hacer el tablero vacío 
#include "Tablero.h"
#include <iostream>
using std::cout, std::endl;
//Ahora también asigna una fase inicial distinta a cada casilla dinámica según su posición en el tablero
void Tablero::iniCasillas()//Asigna el tipo de terreno a cada casilla (zona local, dinámica, rival) y los 5 puntos de poder sobreescriben su casilla
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//asignamos el tipo base según la columna
			if (col <= 2) tablero[fila][col].tipo = Casilla_local;
			else if (col >= 6) tablero[fila][col].tipo = Casilla_rival;
			else tablero[fila][col].tipo = Casilla_dinamica;

			//distribuimos las 3 columnas centrales en fases equidistantes para crear el efecto de ola
			if (tablero[fila][col].tipo == Casilla_dinamica) {
				tablero[fila][col].fase = (col - 3) / 3.0f;//para la col 3 la fase será de 0.0, para la 4 de 0.33 y para la 5 será de 0.66
			}

			//Los puntos de poder sobreescriben el tipo base
			//Los compruebo al final para que tengan prioridad absoluta
			if (esPuntoPoder(fila, col))
				tablero[fila][col].tipo = Casilla_poder;
		}

	}
}

bool Tablero::esPuntoPoder(int fila, int col) const
{
	if (fila == 4 && col == 4) return true;
	if (fila == 0 && col == 4) return true;
	if (fila == 4 && col == 0) return true;
	if (fila == 8 && col == 4) return true;
	if (fila == 4 && col == 8) return true;
	return false;
}

Tablero::Tablero()
{
	iniCasillas();
	cout << "[Board] Tablero" << N << "x" << N << "creado." << endl;
}

void Tablero::update(double dt)
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//como solo queremos actualizar las casillas dinámicas decimos que si no es dinamica continue
			if (tablero[fila][col].tipo != Casilla_dinamica)continue;

			//avanzamos la fase
			tablero[fila][col].fase += (float)(dt * CYCLE_SPEED);

			//cuando llega al final de ciclo, vuelve al inicio
			if (tablero[fila][col].fase > 1.0f)
				tablero[fila][col].fase -= 1.0f;
		}
	}
}
