//son las funciones de dibujo del tablero con OpenGL
#pragma once
#include<iostream>
#include"Tablero.h"
using namespace std;

//enums propios para no depender del glut en la cabecera
enum TipoBoton {BOTON_IZQ,BOTON_MED,BOTON_DER};
enum TipoTecla { TECLA_ARRIBA, TECLA_ABAJO,TECLA_IZQ, TECLA_DER };

class TableroGL {
public:
	//constructor que recibe puntero a tablero
	TableroGL(Tablero* pt) :m_tablero(pt) {
		anchoCasilla = 80.0f;
		N = pt->getTamano;
		turnoActual = 1;
		turnoDeQuien = LOCAL;
		filaRaton = colRaton = -1;
		controlKey = chiftKay = botonIzq = botonDer = botonMed = false;
	}
	virtual ~TableroGL(){}
	//Declaramos el metodo de inicialización
	void init();

	//declaramos el metodo de toda la escena
	void Dibuja();

	//declaramos el metodo para dibujar la cuadricula y las casillas
	void DibujarCuadricula();
	void DibujarCasilla(int fila, int col);

	//para poner en la franja inferior la interfaz del jugador mediante la información de la pantalla (HUD) 
	void DibujaHUD(int turno, TipoOcupante turnoDeQuien);

	//declaramos los metodos de los eventos que ocurren
	void KeyDown(unsigned char tecla);
	void BotonRaton(int x, int y, int boton, int pulsado, bool shiftKey, bool ctrlKey);

	//las conversiones de coordenadas
	//esquina inferior-izquiera de la casilla en píxeles
	void csasilla2pixel(int fila, int col, float& px, float& py) {
		px = (float)col * anchoCasilla;
		py = (float)(N - 1 - fila) * anchoCasilla;
	}
	//centro de la casilla en pixeles
	void casilla2centro(int fila, int col, float& cx, float& cy) {
		csasilla2pixel(fila, col, cx, cy);
		cx += anchoCasilla / 2.0f;
		cy += anchoCasilla / 2.0f;
	}

	//pixel del raton -> (fila, col) del tablero
};