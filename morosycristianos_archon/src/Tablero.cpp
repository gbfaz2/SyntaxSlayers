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
			if (col == 4) tablero[fila][col].tipo = Casilla_dinamica;
			else if (col < 4) {
				tablero[fila][col].tipo = ((fila+col)%2 ==0) ? Casilla_local:Casilla_rival;
			}
			else {
				//hacemos como un espejo de lo que tenemos a la izq
				int espejocol = 8 - col;
				tablero[fila][col].tipo = ((fila + espejocol) % 2 == 0) ? Casilla_rival : Casilla_local;
			}

			//distribuimos las 3 columnas centrales(casillas dinámicas) usando (fila*N+col) para que cada casilla empiece en un punto ditinto del ciclo
			/*if (tablero[fila][col].tipo == Casilla_dinamica) {
				tablero[fila][col].fase = (float)((fila * N + col) % 10) / 10.0f;
			}*/

			//Los puntos de poder sobreescriben el tipo base pero mantienen el ciclo dinámico (también oscilan)
			//Los compruebo al final para que tengan prioridad absoluta
			if (esPuntoPoder(fila, col)) {
				tablero[fila][col].tipo = Casilla_poder;
				//tablero[fila][col].fase = 0.0f; //empiezan en ventaja local
			}
		}

	}
}

void Tablero::iniPiezas()//coloca las piezas en sus posiciones iniciales
{
	//BANDO LOCAL (columna 0 y 1)
	//en la columna 0 las figuras especiales y el líder
	lugar(4, 0, pieza_esfera, bando_local);//Rey cristiano
	lugar(3, 0, pieza_dodecaedro, bando_local);//Espía
	lugar(5, 0, pieza_icosaedro, bando_local);//Almogavar
	lugar(0, 0, pieza_tetraedro, bando_local);//Caballería ligera
	lugar(8, 0, pieza_tetraedro, bando_local);//caballería ligera
	lugar(1, 0, pieza_cubog, bando_local);//infantería pesada
	lugar(7, 0, pieza_cubog, bando_local);//infantería pesada
	lugar(2, 0, pieza_cono, bando_local);//caballería pesada
	lugar(6, 0, pieza_cono, bando_local);//caballería pesada
	// en la columna 1 los ballesteros (esquinas) y milicianos (centro)
	lugar(0, 1, pieza_cilindro, bando_local);//ballestero
	lugar(8, 1, pieza_cilindro, bando_local);//ballestero
	for (int fila = 1; fila <= 7; fila++) lugar(fila, 1, pieza_cubo_p, bando_local);//Milicianos

	//BANDO RIVAL (columna 7,8)
	//en la columna 8 las figuras especiales y el líder
	lugar(4, 8, pieza_esfera, bando_rival);//Emir
	lugar(3, 8, pieza_dodecaedro, bando_rival);//Asesino de élite
	lugar(5, 8, pieza_icosaedro, bando_rival);//Arquero a caballo
	lugar(0, 8, pieza_tetraedro, bando_rival);//Jinete Bereber
	lugar(8, 8, pieza_tetraedro, bando_rival);//Jinete Bereber
	lugar(1, 8, pieza_cubog, bando_rival);//guardia negra
	lugar(7, 8, pieza_cubog, bando_rival);//guardia negra
	lugar(2, 8, pieza_cono, bando_rival);//caballería acorazada
	lugar(6, 8, pieza_cono, bando_rival);//caballería acorazada
	// en la columna 7 los arqueros (esquinas) y soldados (centro)
	lugar(0, 7, pieza_cilindro, bando_rival);//arquero ghazí
	lugar(8, 7, pieza_cilindro, bando_rival);//arquero ghazí
	for (int fila = 1; fila <= 7; fila++) lugar(fila, 7, pieza_cubo_p, bando_rival);//soldados ghazís

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
	iniPiezas();
	cout << "[Board] Tablero" << N << "x" << N << "creado." << endl;
}

/*void Tablero::update(double dt)
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//solo las casillas que oscilan
			if (tablero[fila][col].tipo != Casilla_dinamica && tablero[fila][col].tipo !=Casilla_poder)continue;

			//avanzamos la fase
			tablero[fila][col].fase += (float)(dt * CYCLE_SPEED);

			//cuando llega al final de ciclo, vuelve al inicio
			if (tablero[fila][col].fase > 1.0f)
				tablero[fila][col].fase -= 1.0f;
		}
	}
}*/
//en primer lugar la pieza puede moverse a un destino que esté dentro del tablero, no puede moverse a una casilla ocupada por su mismo equipo y puede moverse a una casilla vacía o con enemigo
bool Tablero::puedeMover(int fr, int fc, int tr, int tc) const
{
	//dentro del tablero
	if (tr < 0 || tr >= N || tc < 0 || tc >= N) return false;
	//tiene que haber una pieza en el origen
	if (tablero[fr][fc].pieza == pieza_nada) return false;
	//no puede ir donde hay un aliado
	if (tablero[tr][tc].pieza != pieza_nada && tablero[tr][tc].bando == tablero[fr][fc].bando) return false;

	return true;
}

bool Tablero::muevePieza(int fr, int fc, int tr, int tc)
{
	bool batalla = (tablero[tr][tc].pieza != pieza_nada && tablero[tr][tc].bando != tablero[fr][fc].bando);
	//Movemos la pieza al destino
	tablero[tr][tc].pieza = tablero[fr][fc].pieza;
	tablero[tr][tc].bando = tablero[fr][fc].bando;

	//Vaciamos el origen
	tablero[fr][fc].pieza = pieza_nada;
	tablero[fr][fc].bando = bando_nada;

	if(batalla)
		cout << "[Board] COMBATE en (" << tr << "," << tc << ")!" << endl;
	else
		cout << "[Board] Movido: (" << fr << "," << fc<< ") → (" << tr << "," << tc << ")" << endl;
	return batalla;
}
