//Lo separamos de tablerogl.h porque tablero sabe qué hay en cada casilla y tablerogl sabe cómo pintarlo
//cada casilla dinamica tiene un estado que oscila local-neutral-rival-neutral-local-...
//el cambio se produce cada 4 turnos 
#pragma once

#include <vector>
#include "Pieza.h"
// enumeración del tipo de terreno de cada casilla
//a la izq está la zona local
//a la derecha está la zona rival
//en el medio está la zona neutra
//5 posiciones de puntos de poder medio, y centros de cada lateral
//el tipo de casilla va a determinar QUÉ par de colores usar y con (fila+col)%2 (par/impar) determino cuál de los dos colores

enum TipoCasilla {
	Casilla_local, 
	Casilla_rival,
	Casilla_neutra,
	Casilla_poder//uno de los 5 puntos estratégicos del tablero
};

//bando de una pieza
enum BandoPieza {
	bando_nada, //sin pieza
	bando_local, 
	bando_rival 
};

enum TipoPieza {
	pieza_nada,
	pieza_lider,//1 por bando rey/emir
	pieza_teleporte,//1 por bando- infiltrado/asesino
	pieza_volador2,//1 por bando- almogavar/arqueo a caballo
	pieza_volador1,//2 por bando-caballería ligera/jinete bereber
	pieza_fuerte1,//cubo grande - 2 por bando infantería pesada/guardia negra
	pieza_fuerte2,//2 por bando- caballería pesada/cab.acorazada
	pieza_basica2,//2 por bando-ballestero/arquero ghazí
	pieza_basica1,//7 por bando- miliciano/soldado ghazí
};

//estrucura de casilla
//vamos a crear un struct ya que todos los miembros van a ser públicos y no hay invariante que proteger
//añado que piza hay y a que bando pertenece 
struct Casilla {
	TipoCasilla tipo{ Casilla_neutra };//Inicializamos aquí en la declaración
	//float fase{ 0.0f };//0-local, 0.5-neutral, 1-rival
	TipoPieza pieza{ pieza_nada };
	BandoPieza bando{ bando_nada };
	Pieza* obj{ nullptr };   // puntero al objeto real de la pieza (nullptr = casilla vacía)
};

struct CasillaPos { int fila, col; };//Par fila, columna para las casillas válidas de movimiento
//con la clase tablero voy a gestionar la cuadrícula lógica de 9x9. Para ello un array estático donde vamos a fijar el tamaño N=9
class Tablero
{
	Casilla tablero[9][9];// la cuadrícula que es un array estático de 9*9

	void iniCasillas();//asigna tipos de casilla
	void iniPiezas();//coloca las piezas en sus posiciones iniciales
	bool esPuntoPoder(int fila, int col) const;
	void lugar(int fila, int col, TipoPieza p, BandoPieza b) {
		tablero[fila][col].pieza = p;
		tablero[fila][col].bando = b;
	}
	void moverSinAnimacion(int fr, int fc, int tr, int tc); // PARA USO INTERNO DE LA IA

public:
	static const int N = 9;//constante no vamos a modificarlo
	Tablero();//constructor por defecto que llama al método iniCasillas()
	Tablero(const Tablero& otro); // CONSTRUCTOR DE COPIA PROFUNDA
	bool _esCopia{ false }; // SI ES COPIA, NO BORRA LAS PIEZAS

	~Tablero();
	int getSize() const { return N; }
	//acceso de solo lectura a una casilla, lo usará tablerogl para saber qué color pintar. usaremos (0,0) como la esquina superior izquierda 
	const Casilla& getCasilla(int fila, int col) const { return tablero[fila][col]; }// el valor de retorno es const para que no se pueda modificar desde fuera
	Casilla& getCasilla(int fila, int col) { return tablero[fila][col]; }
	//añado método para ver si se puede mover la pieza 
	bool puedeMover(int fr, int fc, int tr, int tc)const;
	//añado método para ver si va a haber combate o no, false si no hay combate y true si hay combate
	Pieza* muevePieza(int fr, int fc, int tr, int tc);
	//añado un método que devuelve el radio de movimiento de la pieza
	int getRadioMovimiento(int fila, int col)const;

	BandoPieza checkVicoria() const;

	//devuelve casillas a las que puede moverse la pieza, respetando su radio de movimiento y las reglas básicas
	std::vector<CasillaPos> casillasValidas(int fila, int col) const;

	Pieza* buscarPieza(TipoPieza tipo, BandoPieza bando) const;

	void limpiarPiezas();
	Pieza* crearPieza(TipoPieza tipo, BandoPieza bando);
};
// ── Utilidades de conversión entre los dos enums de bando ──────────────────
// Cristiano = bando_local, Andalusí = bando_rival (convención del juego)
inline BandoPieza toBandoPieza(Bando b) {
	return (b == Bando::CRISTIANO) ? bando_local : bando_rival;
}
inline Bando toBando(BandoPieza bp) {
	return (bp == bando_local) ? Bando::CRISTIANO : Bando::ANDALUSI;
}