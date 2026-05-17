
//Autor: María Heredero
//Tablero.cpp: implementación del tablero. Primero voy a hacer el tablero vacío 

#include "tablero.h"
#include "Rey.h"
#include "Infiltrado.h"
#include "Almogavar.h"
#include "CaballeriaLigera.h"
#include "Infanteria.h"
#include "CaballeriaPesada.h"
#include "Ballestero.h"
#include "Miliciano.h"
#include <iostream>
using std::cout, std::endl;
//Ahora también asigna una fase inicial distinta a cada casilla dinámica según su posición en el tablero

Tablero::Tablero()
{
	iniCasillas();
	iniPiezas();
	cout << "[Board] Tablero" << N << "x" << N << "creado." << endl;
}
Tablero::~Tablero()
{
	for (int f = 0; f < N; f++)
		for (int c = 0; c < N; c++) {
			delete tablero[f][c].obj;
			tablero[f][c].obj = nullptr;
		}
}
//hacemos la cuarta parte superior izq lugi espejo vertical de las filas inferiores. Una vez está esa mitad completa espejo invertido horizontal de la otra mitad
void Tablero::iniCasillas()//Asigna el tipo de terreno a cada casilla (zona local, dinámica, rival) y los 5 puntos de poder sobreescriben su casilla
{
	const TipoCasilla L = Casilla_local;
	const TipoCasilla R = Casilla_rival;
	const TipoCasilla D = Casilla_dinamica;

	TipoCasilla patron[9][9] = {
		//   c0  c1  c2  c3  c4  c5  c6  c7  c8
		{ L,  R,  L,  D,  D,  D,  R,  L,  R },  // fila 0
		{ R,  L,  D,  R,  D,  L,  D,  R,  L },  // fila 1
		{ L,  D,  R,  L,  D,  R,  L,  D,  R },  // fila 2
		{ D,  R,  L,  R,  D,  L,  R,  L,  D },  // fila 3
		{ R,  D,  D,  D,  D,  D,  D,  D,  L },  // fila 4 (col0=DARK, col8=LIGHT, resto gris)
		{ D,  R,  L,  R,  D,  L,  R,  L,  D },  // fila 5 (=fila 3)
		{ L,  D,  R,  L,  D,  R,  L,  D,  R },  // fila 6 (=fila 2)
		{ R,  L,  D,  R,  D,  L,  D,  R,  L },  // fila 7 (=fila 1)
		{ L,  R,  L,  D,  D,  D,  R,  L,  R}, // fila 8 (=fila 0)
	};
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++)
			tablero[fila][col].tipo = patron[fila][col];
	}

	//los puntos de poder los sobreescribimos al tipo base
	tablero[0][4].tipo = Casilla_poder;
	tablero[4][0].tipo = Casilla_poder;
	tablero[4][4].tipo = Casilla_poder;
	tablero[4][8].tipo = Casilla_poder;
	tablero[8][4].tipo = Casilla_poder;
}

void Tablero::iniPiezas()//coloca las piezas en sus posiciones iniciales
{
	// Lambda para colocar pieza con su objeto real
	auto poner = [&](int f, int c, TipoPieza tipo, BandoPieza b, Pieza* obj) {
		lugar(f, c, tipo, b);
		tablero[f][c].obj = obj;
		if (obj) obj->setPosicion(f, c);
		};

	// BANDO LOCAL (cristiano) — columnas 0 y 1
	poner(4, 0, pieza_esfera, bando_local, new Rey(Bando::CRISTIANO));
	poner(3, 0, pieza_dodecaedro, bando_local, new Infiltrado(Bando::CRISTIANO));
	poner(5, 0, pieza_icosaedro, bando_local, new Almogavar(Bando::CRISTIANO));
	poner(0, 0, pieza_tetraedro, bando_local, new CaballeriaLigera(Bando::CRISTIANO));
	poner(8, 0, pieza_tetraedro, bando_local, new CaballeriaLigera(Bando::CRISTIANO));
	poner(1, 0, pieza_cubog, bando_local, new Infanteria(Bando::CRISTIANO));
	poner(7, 0, pieza_cubog, bando_local, new Infanteria(Bando::CRISTIANO));
	poner(2, 0, pieza_cono, bando_local, new CaballeriaPesada(Bando::CRISTIANO));
	poner(6, 0, pieza_cono, bando_local, new CaballeriaPesada(Bando::CRISTIANO));
	poner(0, 1, pieza_cilindro, bando_local, new Ballestero(Bando::CRISTIANO));
	poner(8, 1, pieza_cilindro, bando_local, new Ballestero(Bando::CRISTIANO));
	for (int fila = 1; fila <= 7; fila++)
		poner(fila, 1, pieza_cubo_p, bando_local, new Miliciano(Bando::CRISTIANO));

	// BANDO RIVAL (andalusí) — columnas 7 y 8
	poner(4, 8, pieza_esfera, bando_rival, new Rey(Bando::ANDALUSI));
	poner(3, 8, pieza_dodecaedro, bando_rival, new Infiltrado(Bando::ANDALUSI));
	poner(5, 8, pieza_icosaedro, bando_rival, new Almogavar(Bando::ANDALUSI));
	poner(0, 8, pieza_tetraedro, bando_rival, new CaballeriaLigera(Bando::ANDALUSI));
	poner(8, 8, pieza_tetraedro, bando_rival, new CaballeriaLigera(Bando::ANDALUSI));
	poner(1, 8, pieza_cubog, bando_rival, new Infanteria(Bando::ANDALUSI));
	poner(7, 8, pieza_cubog, bando_rival, new Infanteria(Bando::ANDALUSI));
	poner(2, 8, pieza_cono, bando_rival, new CaballeriaPesada(Bando::ANDALUSI));
	poner(6, 8, pieza_cono, bando_rival, new CaballeriaPesada(Bando::ANDALUSI));
	poner(0, 7, pieza_cilindro, bando_rival, new Ballestero(Bando::ANDALUSI));
	poner(8, 7, pieza_cilindro, bando_rival, new Ballestero(Bando::ANDALUSI));
	for (int fila = 1; fila <= 7; fila++)
		poner(fila, 7, pieza_cubo_p, bando_rival, new Miliciano(Bando::ANDALUSI));
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
	// Si hay combate, eliminamos la pieza destino
	if (batalla) {
		delete tablero[tr][tc].obj;
		tablero[tr][tc].obj = nullptr;
	}
	//Movemos la pieza al destino
	tablero[tr][tc].obj = tablero[fr][fc].obj;
	if (tablero[tr][tc].obj) tablero[tr][tc].obj->setPosicion(tr, tc);
	tablero[tr][tc].pieza = tablero[fr][fc].pieza;  
	tablero[tr][tc].bando = tablero[fr][fc].bando;

	//Vaciamos el origen
	tablero[fr][fc].pieza = pieza_nada;
	tablero[fr][fc].bando = bando_nada;
	tablero[fr][fc].obj = nullptr;


	if (batalla)
		cout << "[Board] COMBATE en (" << tr << "," << tc << ")!" << endl;
	else
		cout << "[Board] Movido: (" << fr << "," << fc << ") → (" << tr << "," << tc << ")" << endl;
	return batalla;
}