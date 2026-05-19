
//Autor: María Heredero

// lo usamos para separar la lógica del dibujo. Primero probaremos a hacer un tablero vacío con un aspecto visual del Archon original
#pragma once
#include "tablero.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"


//creo las enumeraciones con las variables del raton y de las teclas especiales para signarles el mismo valor que tenemos en el freeglut.h
enum { MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON };
enum { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

class Tablerogl
{
protected:
	float ancho;
	int N;
	Tablero* m_tablero;

	double centro_x, centro_y, centro_z;
	double dist;

	int Filacursor[2], Colcursor[2];
	int fromFila, fromCol;//origen del movimiento en curso
	BandoPieza fromBando;
	BandoPieza victoria_;//resultado de chackVictoria actualizado cada movimiento
	bool piezaSeleccionada; //hay una pieza seleccionada esperando destino

	int xcasilla_sel, ycasilla_sel;//casilla bajo el cursor
	bool controlKey, shiftKey;
	bool leftButton, rightButton, midButton;

	

	bool _combatePendiente{ false };
	Pieza* _pAtacante{ nullptr };//puntero al obj que sigue en tablero
	Pieza* _pDefensora{ nullptr };//extraída del tablero

private:
	friend class GestorInput; // ACCESO TOTAL AL GESTOR DE INPUT
	GestorMovimiento gestorMovimiento;
	GestorTurnos     gestorTurnos;

public:
	Tablerogl(Tablero* pb);//constructor que inicializaremos en el .cpp con inicializadores
	virtual ~Tablerogl() {}//destructor virtual

	void init(); //Luces más perspectiva desde donde lo vamos a ver

	void DibujaFondo();

	void Dibuja();//casillas y cuadrícula
	void DibujaCasillas();//recorre 9x9 y pinta cada casilla
	void DibujaCasilla(int fila, int col); //Dibuja un quad de relleno
	//void DibujaCuadricula();
	void setCasillaColor(int fila, int col);//Elige el color que tiene que tener la casilla correspondiente
	void DibujaSimbolos();//dibuja encima de cada casilla su simbolo correspondiente, ahora si casilla local(cristianos) cruz blanca, si no (media luna blanca)
	void DibujaCruz(float cx, float cy, float size);//dibuja la cruz de los cristianos centrada
	void DibujaLuna(float cx, float cy, float size);//dibuja la media luna de los musulmanes centrada
	void DibujaPuntoPoder(float cx, float cy, float size);//dibuja el círculo amarillo en las casillas de punto de poder
	void DibujaCasSelec(int fila, int col, float r, float g, float b, float lw, float z);
	void DibujaCursores();
	void DibujaSeleccion();

	void DibujaMovimientosValidos();//se llama cuando hay una pieza seleccionada para resaltar las casillas a las que puede moverse
	void DibujaVictoria();//dibuja el cartel de victoria si alguien ha ganado 

	void DibujaPiezas();
	void DibujaPieza(int fil, int col);//dibuja la pieza de esa casilla

	void trySelectorMove(BandoPieza bando);


	//conversores de coordenadas cogidas del repositorio de Pablo
	void cell2center(int casilla_x, int casilla_y, float& glx, float& gly);
	void world2cell(double x, double y, int& casilla_x, int& casilla_y);//dado un punto (x,y) en coordenadas opengl, devuelve la fila y columna del tablero

	bool huboColision() const { return _combatePendiente; }
	Pieza* getPiezaAtacante() const { return _pAtacante; }
	Pieza* getPiezaDefensora() const { return _pDefensora; }

	void limpiarCombate();//para liberar a la defensora y resetear los flags
};


