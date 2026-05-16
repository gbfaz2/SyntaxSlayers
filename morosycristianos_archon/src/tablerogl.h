// lo usamos para separar la lógica del dibujo. Primero probaremos a hacer un tablero vacío con un aspecto visual del Archon original
#pragma once
#include "Tablero.h"


//creo las enumeraciones con las variables del raton y de las teclas especiales para signarles el mismo valor que tenemos en el freeglut.h
enum {MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON};
enum {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

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
	Bando fromBando;
	bool piezaSeleccionada; //hay una pieza seleccionada esperando destino

	int xcasilla_sel, ycasilla_sel;//casilla bajo el cursor
	bool controlKey, shiftKey;
	bool leftButton, rightButton, midButton;
public:
	Tablerogl(Tablero* pb);//constructor que inicializaremos en el .cpp con inicializadores
	virtual ~Tablerogl(){}//destructor virtual

	void init(); //Luces más perspectiva desde donde lo vamos a ver

	void DibujaFondo();
	void DibujaTitulo();
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

	void DibujaPiezas();
	void DibujaPieza(int fil, int col);//dibuja la pieza de esa casilla

	void trySelectorMove(Bando bando);

	void KeyDown(unsigned char key);
	void SpecialKey(int key);
	void MouseButton(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey);

	//conversores de coordenadas cogidas del repositorio de Pablo
	void cell2center(int casilla_x, int casilla_y, float& glx, float& gly);
	void world2cell(double x, double y, int& casilla_x, int& casilla_y);//dado un punto (x,y) en coordenadas opengl, devuelve la fila y columna del tablero
};

