
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
//hacemos la cuarta parte superior izq luego espejo vertical de las filas inferiores. Una vez está esa mitad completa espejo invertido horizontal de la otra mitad
void Tablero::iniCasillas()//Asigna el tipo de terreno a cada casilla (zona local, neutra, rival) y los 5 puntos de poder sobreescriben su casilla
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
		{ R,  D,  D,  D,  D,  D,  D,  D,  L },  // fila 4 (col0=cristiano, col8=moro, resto gris)
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
		if (obj) {
			obj->setPosicion(f, c);
			// Sincronizamos el flag de punto de poder desde el tablero
			obj->setEnPuntoDePoder(tablero[f][c].tipo == Casilla_poder);
		}
	};


	int idAlm = 0; // Contador de IDs para almogávares

	Pieza* Alm = new Almogavar(Bando::CRISTIANO);
	Alm->setIdAnimacion(idAlm++);
	poner(5, 0, pieza_volador2, bando_local, Alm);


	int idCabLig = 0; // Contador de IDs para caballería ligera

	Pieza* CabLig = new CaballeriaLigera(Bando::CRISTIANO);
	CabLig->setIdAnimacion(idCabLig++);
	poner(0, 0, pieza_volador1, bando_local, CabLig);

	Pieza* CabLig2 = new CaballeriaLigera(Bando::CRISTIANO);
	CabLig2->setIdAnimacion(idCabLig++);
	poner(8, 0, pieza_volador1, bando_local, CabLig2);


	int idBall = 0; // Contador de IDs para ballesteros

	Pieza* Balle = new Ballestero(Bando::CRISTIANO);
	Balle->setIdAnimacion(idBall++);
	poner(0, 1, pieza_basica2, bando_local, Balle);
	
	Pieza* Ballest = new Ballestero(Bando::CRISTIANO);
	Ballest->setIdAnimacion(idBall++);
	poner(8, 1, pieza_basica2, bando_local, Ballest);


	int idCab = 0; // Contador de IDs para caballería pesada

	Pieza* Cabb = new CaballeriaPesada(Bando::CRISTIANO); 
	Cabb->setIdAnimacion(idCab++);
	poner(2, 0, pieza_fuerte2, bando_local, Cabb);

	Pieza* Cab = new CaballeriaPesada(Bando::CRISTIANO);
	Cab->setIdAnimacion(idCab++);
	poner(6, 0, pieza_fuerte2, bando_local, Cab);

	

	int idMiliciano = 0; //contador ID para milicianos

	//asignamos ID para cada miliciano
	for (int fila = 1; fila <= 7; fila++) {
		Pieza* mLocal = new Miliciano(Bando::CRISTIANO);
		mLocal->setIdAnimacion(idMiliciano); // Le damos ID 0, 1, 2...
		idMiliciano++;
		poner(fila, 1, pieza_basica1, bando_local, mLocal);
	}

	int idInfiltrado = 0; // Contador de IDs para infiltrado

	Pieza* Infil = new Infiltrado(Bando::CRISTIANO);
	Infil->setIdAnimacion(idInfiltrado++);
	poner(3, 0, pieza_teleporte, bando_local, Infil);


	int idReyy = 0; // Contador de IDs para rey

	Pieza* Re = new Rey(Bando::CRISTIANO);
	Re->setIdAnimacion(idInfiltrado++);
	poner(4, 0, pieza_lider, bando_local, Re);


	int idInfanT = 0; // Contador de IDs para infantería

	Pieza* Infanterr = new Infanteria(Bando::CRISTIANO);
	Infanterr->setIdAnimacion(idInfanT++);
	poner(1, 0, pieza_fuerte1, bando_local, Infanterr);

	Pieza* Infanterii = new Infanteria(Bando::CRISTIANO);
	Infanterii->setIdAnimacion(idInfanT++);
	poner(7, 0, pieza_fuerte1, bando_local, Infanterii);
	
	int idAlmo = 0; // Contador de IDs para almogávares

	Pieza* Arqc = new Almogavar(Bando::ANDALUSI);
	Arqc ->setIdAnimacion(idAlmo++);
	poner(5, 8, pieza_volador2, bando_rival, Arqc);


	int idBalle = 0; // Contador de IDs para ballesteros

	Pieza* Arq = new Ballestero(Bando::ANDALUSI);
	Arq->setIdAnimacion(idBalle++);
	poner(0, 7, pieza_basica2, bando_rival, Arq);

	Pieza* Arq2 = new Ballestero(Bando::ANDALUSI);
	Arq2->setIdAnimacion(idBalle++);
	poner(8, 7, pieza_basica2, bando_rival, Arq2);


	int idCabLige = 0; // Contador de IDs para caballería ligera	

	//asignar ID para caballero pesado de la fila 0
	Pieza* CabPes = new CaballeriaPesada(Bando::ANDALUSI);
	CabPes->setIdAnimacion(idCabLige++);
	poner(2, 8, pieza_fuerte2, bando_rival, CabPes);

	Pieza* CabPes2 = new CaballeriaPesada(Bando::ANDALUSI);
	CabPes2->setIdAnimacion(idCabLige++);
	poner(6, 8, pieza_fuerte2, bando_rival, CabPes2);


	int idMilici = 0; //contador ID para milicianos


	//asignamos ID para cada miliciano
	for (int fila = 1; fila <= 7; fila++) {
		Pieza* mRival = new Miliciano(Bando::ANDALUSI);
		mRival->setIdAnimacion(idMiliciano); // Le damos ID 0, 1, 2...
		idMiliciano++;
		poner(fila, 7, pieza_basica1, bando_rival, mRival);
	}


	int idRey = 0; // Contador de IDs para rey

	//asignar ID para emir de la fila 0
	Pieza* Em = new Rey(Bando::ANDALUSI);
	Em->setIdAnimacion(idRey++);
	poner(4, 8, pieza_lider, bando_rival, Em);

	int idInfanteria = 0; // Contador de IDs para infantería

	Pieza* guar = new Infanteria(Bando::ANDALUSI);
	guar->setIdAnimacion(idInfanteria++);
	poner(1, 8, pieza_fuerte1, bando_rival, guar);

	Pieza* guard = new Infanteria(Bando::ANDALUSI);
	guard->setIdAnimacion(idInfanteria++);
	poner(7, 8, pieza_fuerte1, bando_rival, guard);

	int idJinete = 0; // Contador de IDs para jinetes

	//asignar ID para caballero ligero de la fila 0
	Pieza* jine = new CaballeriaLigera(Bando::ANDALUSI);
	jine->setIdAnimacion(idJinete++);
	poner(0, 8, pieza_volador1, bando_rival, jine);

	Pieza* jin = new CaballeriaLigera(Bando::ANDALUSI);
	jin->setIdAnimacion(idJinete++);
	poner(8, 8, pieza_volador1, bando_rival, jin);

	//asignar ID para caballero ligero de la fila 0
	Pieza* Inf = new Infiltrado(Bando::ANDALUSI);
	Inf->setIdAnimacion(idInfiltrado++);
	poner(3, 8, pieza_teleporte, bando_rival, Inf);

	int idGhazi = 0; // CONTADOR DE ID PARA LA ANIMACIÓN RIVAL

	// RECORREMOS LAS FILAS CENTRALES PARA LA PRIMERA LÍNEA (COLUMNA 7)
	for (int fila = 1; fila <= 7; fila++) {

		// CREAMOS LA PIEZA COMO MILICIANO PERO DEL BANDO ANDALUSÍ
		Pieza* mRival = new Miliciano(Bando::ANDALUSI);

		// ASIGNAMOS SU DNI/ID
		mRival->setIdAnimacion(idGhazi);
		idGhazi++;

		// COLOCAMOS LA PIEZA: FILA DEL BUCLE, COL 7, TIPO CUBO_P, RIVAL, PUNTERO
		poner(fila, 7, pieza_basica1, bando_rival, mRival);
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
	if (tablero[tr][tc].obj) {
		tablero[tr][tc].obj->setPosicion(tr, tc);
		// La pieza sabe si ahora está en un punto de poder
		tablero[tr][tc].obj->setEnPuntoDePoder(tablero[tr][tc].tipo == Casilla_poder);
	}

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
	case pieza_lider:      return 3; // Rey/Emir: radio grande
	case pieza_teleporte:  return 9; // Infiltrado: puede ir a cualquier lado
	case pieza_volador2:   return 4; // Almogávar
	case pieza_volador1:   return 3; // Caballería ligera
	case pieza_fuerte1:       return 2; // Infantería pesada
	case pieza_fuerte2:        return 3; // Caballería pesada
	case pieza_basica2:    return 3; // Ballestero/Arquero
	case pieza_basica1:      return 3; // Miliciano/Soldado
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
	Bando b = toBando(bando);
	switch (tipo) {
	case pieza_lider:      return new Rey(b);
	case pieza_teleporte:  return new Infiltrado(b);
	case pieza_volador2:   return new Almogavar(b);
	case pieza_volador1:   return new CaballeriaLigera(b);
	case pieza_fuerte1:       return new Infanteria(b);
	case pieza_fuerte2:        return new CaballeriaPesada(b);
	case pieza_basica2:    return new Ballestero(b);
	case pieza_basica1:      return new Miliciano(b);
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