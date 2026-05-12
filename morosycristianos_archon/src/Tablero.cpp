//Tablero.cpp: implementación del tablero. Primero voy a hacer el tablero vacío 
#include "Tablero.h"
#include <iostream>
using std::cout, std::endl;

void Tablero::iniCasillas()//Asigna el tipo de terreno a cada casilla (zona local, dinámica, rival) y los 5 puntos de poder sobreescriben su casilla
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//asignamos el tipo base según la columna
			if (col <= 2) tablero[fila][col].tipo = Casilla_local;
			else if (col >= 6) tablero[fila][col].tipo = Casilla_rival;
			else tablero[fila][col].tipo = Casilla_dinamica;

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
