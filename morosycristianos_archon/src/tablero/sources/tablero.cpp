
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
	cout << "[Tablero]" << N << "x" << N << "creado." << endl;
}

Tablero::Tablero(const Tablero& otro)
{
	// COPIA LAS CASILLAS SIN COPIAR LOS PUNTEROS — LA IA NO NECESITA OBJETOS REALES
	for (int f = 0; f < N; f++) {
		for (int c = 0; c < N; c++) {
			tablero[f][c].tipo = otro.tablero[f][c].tipo;
			tablero[f][c].pieza = otro.tablero[f][c].pieza;
			tablero[f][c].bando = otro.tablero[f][c].bando;
			tablero[f][c].obj = otro.tablero[f][c].obj; // COPIA EL PUNTERO SIN DUPLICAR
			_esCopia = true; // MARCA QUE ESTA INSTANCIA NO ES DUEÑA DE LAS PIEZAS
		}
	}
}

Tablero::~Tablero()
{
	if (_esCopia) return; // LA COPIA NO ES DUEÑA DE LAS PIEZAS
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
	const TipoCasilla D = Casilla_neutra;

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
	int idSprite = 0;

	// Lambda para colocar pieza con su objeto real
	auto poner = [&](int f, int c, TipoPieza tipo, BandoPieza b, Pieza* obj) {
		lugar(f, c, tipo, b);
		tablero[f][c].obj = obj;
		if (obj) obj->setPosicion(f, c);
		};

	// BANDO LOCAL (cristiano) — columnas 0 y 1
	//poner(4, 0, pieza_esfera, bando_local, new Rey(Bando::CRISTIANO));
	//poner(3, 0, pieza_dodecaedro, bando_local, new Infiltrado(Bando::CRISTIANO));
	poner(5, 0, pieza_icosaedro, bando_local, new Almogavar(Bando::CRISTIANO));
	//poner(0, 0, pieza_tetraedro, bando_local, new CaballeriaLigera(Bando::CRISTIANO));
	//poner(8, 0, pieza_tetraedro, bando_local, new CaballeriaLigera(Bando::CRISTIANO));
	//poner(1, 0, pieza_cubog, bando_local, new Infanteria(Bando::CRISTIANO));
	//poner(7, 0, pieza_cubog, bando_local, new Infanteria(Bando::CRISTIANO));
	//poner(2, 0, pieza_cono, bando_local, new CaballeriaPesada(Bando::CRISTIANO));
	//poner(6, 0, pieza_cono, bando_local, new CaballeriaPesada(Bando::CRISTIANO));
	//poner(0, 1, pieza_cilindro, bando_local, new Ballestero(Bando::CRISTIANO));
	//poner(8, 1, pieza_cilindro, bando_local, new Ballestero(Bando::CRISTIANO));
	//for (int fila = 1; fila <= 7; fila++)
		//poner(fila, 1, pieza_cubo_p, bando_local, new Miliciano(Bando::CRISTIANO));

	int idCabLig = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* CabLig = new CaballeriaLigera(Bando::CRISTIANO);
	CabLig->setIdAnimacion(idCabLig++);
	poner(0, 0, pieza_tetraedro, bando_local, CabLig);

	//asignar ID para caballero ligero de la fila 0
	Pieza* CabLig2 = new CaballeriaLigera(Bando::CRISTIANO);
	CabLig2->setIdAnimacion(idCabLig++);
	poner(8, 0, pieza_tetraedro, bando_local, CabLig2);


	int idBall = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* Balle = new Ballestero(Bando::CRISTIANO);
	Balle->setIdAnimacion(idBall++);
	poner(0, 1, pieza_cilindro, bando_local, Balle);

	//asignar ID para caballero ligero de la fila 0
	Pieza* Ballest = new Ballestero(Bando::CRISTIANO);
	Ballest->setIdAnimacion(idBall++);
	poner(8, 1, pieza_cilindro, bando_local, Ballest);


	int idCab = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* Cabb = new CaballeriaPesada(Bando::CRISTIANO);
	Cabb->setIdAnimacion(idCab++);
	poner(2, 0, pieza_cono, bando_local, Cabb);

	Pieza* Cab = new CaballeriaPesada(Bando::CRISTIANO);
	Cab->setIdAnimacion(idCab++);
	poner(6, 0, pieza_cono, bando_local, Cab);

	

	int idMiliciano = 0; //contador ID para milicianos

	//asignamos ID para cada miliciano
	for (int fila = 1; fila <= 7; fila++) {
		Pieza* mLocal = new Miliciano(Bando::CRISTIANO);
		mLocal->setIdAnimacion(idMiliciano); // Le damos ID 0, 1, 2...
		idMiliciano++;
		poner(fila, 1, pieza_cubo_p, bando_local, mLocal);
	}

	int idInfiltrado = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* Infil = new Infiltrado(Bando::CRISTIANO);
	Infil->setIdAnimacion(idInfiltrado++);
	poner(3, 0, pieza_dodecaedro, bando_local, Infil);


	int idReyy = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* Re = new Rey(Bando::CRISTIANO);
	Re->setIdAnimacion(idInfiltrado++);
	poner(4, 0, pieza_esfera, bando_local, Re);



	int idInfanT = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* Infanterr = new Infanteria(Bando::CRISTIANO);
	Infanterr->setIdAnimacion(idInfanT++);
	poner(1, 0, pieza_cubog, bando_local, Infanterr);

	//asignar ID para caballero ligero de la fila 0
	Pieza* Infanterii = new Infanteria(Bando::CRISTIANO);
	Infanterii->setIdAnimacion(idInfanT++);
	poner(7, 0, pieza_cubog, bando_local, Infanterii);




	// BANDO RIVAL (andalusí) — columnas 7 y 8
	//poner(4, 8, pieza_esfera, bando_rival, new Rey(Bando::ANDALUSI));
	//poner(3, 8, pieza_dodecaedro, bando_rival, new Infiltrado(Bando::ANDALUSI));
	poner(5, 8, pieza_icosaedro, bando_rival, new Almogavar(Bando::ANDALUSI));
	//poner(0, 8, pieza_tetraedro, bando_rival, new CaballeriaLigera(Bando::ANDALUSI));
	//poner(8, 8, pieza_tetraedro, bando_rival, new CaballeriaLigera(Bando::ANDALUSI));
	//poner(1, 8, pieza_cubog, bando_rival, new Infanteria(Bando::ANDALUSI));
	//poner(7, 8, pieza_cubog, bando_rival, new Infanteria(Bando::ANDALUSI));
	//poner(2, 8, pieza_cono, bando_rival, new CaballeriaPesada(Bando::ANDALUSI));
	//poner(6, 8, pieza_cono, bando_rival, new CaballeriaPesada(Bando::ANDALUSI));
	poner(0, 7, pieza_cilindro, bando_rival, new Ballestero(Bando::ANDALUSI));
	poner(8, 7, pieza_cilindro, bando_rival, new Ballestero(Bando::ANDALUSI));
	
	int idCabLige = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* CabPes = new CaballeriaPesada(Bando::ANDALUSI);
	CabPes->setIdAnimacion(idCabLige++);
	poner(2, 8, pieza_cono, bando_local, CabPes);

	//asignar ID para caballero ligero de la fila 0
	Pieza* CabPes2 = new CaballeriaPesada(Bando::ANDALUSI);
	CabPes2->setIdAnimacion(idCabLige++);
	poner(6, 8, pieza_cono, bando_local, CabPes2);




	int idMilici = 0; //contador ID para milicianos


	//asignamos ID para cada miliciano
	for (int fila = 1; fila <= 7; fila++) {
		Pieza* mRival = new Miliciano(Bando::ANDALUSI);
		mRival->setIdAnimacion(idMiliciano); // Le damos ID 0, 1, 2...
		idMiliciano++;
		poner(fila, 7, pieza_cubo_p, bando_rival, mRival);
	}


	int idRey = 0; // Contador de IDs para infiltrado

	//asignar ID para emir de la fila 0
	Pieza* Em = new Rey(Bando::ANDALUSI);
	Em->setIdAnimacion(idRey++);
	poner(4, 8, pieza_esfera, bando_rival, Em);

	int idInfanteria = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* guar = new Infanteria(Bando::ANDALUSI);
	guar->setIdAnimacion(idInfanteria++);
	poner(1, 8, pieza_cubog, bando_rival, guar);

	Pieza* guard = new Infanteria(Bando::ANDALUSI);
	guard->setIdAnimacion(idInfanteria++);
	poner(7, 8, pieza_cubog, bando_rival, guard);

	int idJinete = 0; // Contador de IDs para infiltrado

	//asignar ID para caballero ligero de la fila 0
	Pieza* jine = new CaballeriaLigera(Bando::ANDALUSI);
	jine->setIdAnimacion(idJinete++);
	poner(0, 8, pieza_tetraedro, bando_rival, jine);

	Pieza* jin = new CaballeriaLigera(Bando::ANDALUSI);
	jin->setIdAnimacion(idJinete++);
	poner(8, 8, pieza_tetraedro, bando_rival, jin);

	//asignar ID para caballero ligero de la fila 0
	Pieza* Inf = new Infiltrado(Bando::ANDALUSI);
	Inf->setIdAnimacion(idInfiltrado++);
	poner(3, 8, pieza_dodecaedro, bando_rival, Inf);


	//faltan soldados ghazi
	// GENERAMOS LOS SOLDADOS GHAZI (FRONT-LINE RIVAL) TEMPORALMENTE
	int idGhazi = 0; // CONTADOR DE ID PARA LA ANIMACIÓN RIVAL
	// RECORREMOS LAS FILAS CENTRALES PARA LA PRIMERA LÍNEA (COLUMNA 7)
	for (int fila = 1; fila <= 7; fila++) {

		// CREAMOS LA PIEZA COMO MILICIANO PERO DEL BANDO ANDALUSÍ
		Pieza* mRival = new Miliciano(Bando::ANDALUSI);

		// ASIGNAMOS SU DNI/ID
		mRival->setIdAnimacion(idGhazi);
		idGhazi++;

		// COLOCAMOS LA PIEZA: FILA DEL BUCLE, COL 7, TIPO CUBO_P, RIVAL, PUNTERO
		poner(fila, 7, pieza_cubo_p, bando_rival, mRival);
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

Pieza* Tablero::muevePieza(int fr, int fc, int tr, int tc)
{
	bool batalla = (tablero[tr][tc].pieza != pieza_nada && tablero[tr][tc].bando != tablero[fr][fc].bando);
	// Si hay combate, extraemos la pieza defensora en vez de que hace
	Pieza* capturada = nullptr;
	if (batalla) {
		capturada= tablero[tr][tc].obj;
		tablero[tr][tc].obj = nullptr;
	}
	//Movemos la pieza al destino
	tablero[tr][tc].pieza = tablero[fr][fc].pieza;
	tablero[tr][tc].bando = tablero[fr][fc].bando;
	tablero[tr][tc].obj = tablero[fr][fc].obj;
	if (tablero[tr][tc].obj) tablero[tr][tc].obj->setPosicion(tr, tc);

	//Vaciamos el origen
	tablero[fr][fc].pieza = pieza_nada;
	tablero[fr][fc].bando = bando_nada;
	tablero[fr][fc].obj = nullptr;


	if (batalla)
		cout << "[Tablero] COMBATE en (" << tr << "," << tc << ")!" << endl;
	else
		cout << "[Tablero] Movido: (" << fr << "," << fc << ") -> (" << tr << "," << tc << ")" << endl;
	return capturada;
}

int Tablero::getRadioMovimiento(int fila, int col) const
{
	const Casilla& c = tablero[fila][col];

	// Si la pieza tiene objeto real, usamos su radio propio
	if (c.obj != nullptr)
		return c.obj->getRadioMov();

	// Fallback por tipo (por si obj no estuviera inicializado)
	switch (c.pieza) {
	case pieza_esfera:      return 3; // Rey/Emir: radio grande
	case pieza_dodecaedro:  return 9; // Infiltrado: puede ir a cualquier lado
	case pieza_icosaedro:   return 4; // Almogávar
	case pieza_tetraedro:   return 3; // Caballería ligera
	case pieza_cubog:       return 2; // Infantería pesada
	case pieza_cono:        return 3; // Caballería pesada
	case pieza_cilindro:    return 3; // Ballestero/Arquero
	case pieza_cubo_p:      return 3; // Miliciano/Soldado
	default:                return 1;
	}
}

std::vector<CasillaPos> Tablero::casillasValidas(int fila, int col) const
{
	std::vector<CasillaPos> resultado;
	if (tablero[fila][col].pieza == pieza_nada) return resultado;

	const Casilla& origen = tablero[fila][col];
	Pieza* pObj = origen.obj;

	for (int tf = 0; tf < N; tf++) {
		for (int tc = 0; tc < N; tc++) {
			if (tf == fila && tc == col) continue;

			// puedeMoverse() ya implementa las reglas de cada tipo:
			//   PiezaTerrestre → sin diagonal, max radioMov casillas
			//   PiezaVoladora  → con diagonal, max radioMov casillas
			//   PiezaTeleporte → cualquier casilla del tablero

			//devuelve si puede o no ejecturar el movimiento teniendo en cuenta el destnio
			bool moviendose = (pObj != nullptr)? pObj->puedeMoverse(tf, tc): true; // fallback: sin restricción

			// puedeMover() del tablero comprueba que no hay aliado
			if (moviendose && puedeMover(fila, col, tf, tc)) {

				//declara variable que indica camino despejado true or false
				bool caminoDespejado = true;

				//solo para piezas terrestres:
				if (pObj != nullptr && pObj->getTipoMovimiento() == TipoMovimiento::TERRESTRE) {
					
					//calcula la dif de destino con la fila/columna actual
					int difFila = tf - fila;
					int difCol = tc - col;

					//solo trazamos el camino si es un movimiento en línea recta o diagonal perfecta
					if (difFila == 0 || difCol == 0 || abs(difFila) == abs(difCol)) {

						// Calculamos la dirección del paso (-1, 0, o 1)
						int dirFila = (difFila > 0) ? 1 : ((difFila < 0) ? -1 : 0);
						int dirCol = (difCol > 0) ? 1 : ((difCol < 0) ? -1 : 0);

						//calculamos la fila más la dirección
						int fAct = fila + dirFila;
						int cAct = col + dirCol;

						//avanzamos por el tablero hasta llegar justo 1 casilla antes del destino
						while (fAct != tf || cAct != tc) {
							//si nos chocamos con CUALQUIER pieza en medio del camino, está bloqueado
							if (tablero[fAct][cAct].pieza != pieza_nada) {
								caminoDespejado = false;
								break;
							}

							//suma y actualización para fila y columna
							fAct += dirFila;
							cAct += dirCol;
						}
					}
				}
				// Si el camino está limpio (o es voladora/teleport), lo añadimos a las iluminadas
				if (caminoDespejado) {
					resultado.push_back({ tf, tc });
			    }
			
			}
				
		}
	}
	return resultado;
}

BandoPieza Tablero::checkVicoria() const
{
	// ── Condición 1: puntos de poder ──
	const int poderPos[5][2] = { {0,4},{4,0},{4,4},{4,8},{8,4} };
	int poderLocal = 0, poderRival = 0;

	for (auto& pp : poderPos) {
		const Casilla& c = tablero[pp[0]][pp[1]];
		if (c.pieza != pieza_nada) {
			if (c.bando == bando_local) poderLocal++;
			else                        poderRival++;
		}
	}
	if (poderLocal == 5) return bando_local;
	if (poderRival == 5) return bando_rival;

	// ── Condición 2: solo quedan piezas de un bando ──
	bool hayLocal = false, hayRival = false;
	for (int f = 0; f < N; f++)
		for (int c = 0; c < N; c++) {
			if (tablero[f][c].pieza == pieza_nada) continue;
			if (tablero[f][c].bando == bando_local) hayLocal = true;
			if (tablero[f][c].bando == bando_rival) hayRival = true;
		}

	if (hayLocal && !hayRival) return bando_local;
	if (!hayLocal && hayRival) return bando_rival;

	return bando_nada; // nadie ha ganado todavía
	return BandoPieza();
}

Pieza* Tablero::buscarPieza(TipoPieza tipo, BandoPieza bando) const
{
	for (int f = 0; f < N; f++)
		for (int c = 0; c < N; c++)
			if (tablero[f][c].pieza == tipo && tablero[f][c].bando == bando)
				return tablero[f][c].obj;
	return nullptr;
}

void Tablero::limpiarPiezas()
{
	for (int f = 0; f < N; f++)
		for (int c = 0; c < N; c++) {
			delete tablero[f][c].obj;
			tablero[f][c].obj = nullptr;
			tablero[f][c].pieza = pieza_nada;
			tablero[f][c].bando = bando_nada;
		}
}

Pieza* Tablero::crearPieza(TipoPieza tipo, BandoPieza bando)
{
	Bando b = (bando == bando_local) ? Bando::CRISTIANO : Bando::ANDALUSI;
	switch (tipo) {
	case pieza_esfera:      return new Rey(b);
	case pieza_dodecaedro:  return new Infiltrado(b);
	case pieza_icosaedro:   return new Almogavar(b);
	case pieza_tetraedro:   return new CaballeriaLigera(b);
	case pieza_cubog:       return new Infanteria(b);
	case pieza_cono:        return new CaballeriaPesada(b);
	case pieza_cilindro:    return new Ballestero(b);
	case pieza_cubo_p:      return new Miliciano(b);
	default:                return nullptr;
	}
}

void Tablero::moverSinAnimacion(int fr, int fc, int tr, int tc)
{
	// IGUAL QUE muevePieza PERO SIN setPosicion — LA IA NO MUEVE OBJETOS REALES
	tablero[tr][tc].pieza = tablero[fr][fc].pieza;
	tablero[tr][tc].bando = tablero[fr][fc].bando;
	tablero[tr][tc].obj = tablero[fr][fc].obj;
	// SIN: tablero[tr][tc].obj->setPosicion(tr, tc);

	tablero[fr][fc].pieza = pieza_nada;
	tablero[fr][fc].bando = bando_nada;
	tablero[fr][fc].obj = nullptr;
}