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


};