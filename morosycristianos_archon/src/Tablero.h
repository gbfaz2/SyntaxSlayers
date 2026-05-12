//tablero.h - lógica del tablero 9x9.
//Lo separamos de tablerogl.h porque tablero sabe qué hay en cada casilla y tablerogl sabe cómo pintarlo
#pragma once

// enumeración del tipo de terreno de cada casilla
//a la izq está la zona local
//a la derecha está la zona rival
//en el medio está la zona dinámica
//5 posiciones de puntos de poder medio, y centros de cada lateral
//el tipo de casilla va a determinar QUÉ par de colores usar y con (fila+col)%2 (par/impar) determino cuál de los dos colores

enum TipoCasilla {
	Casilla_local, //columnas 0-2
	Casilla_rival,//columnas 6-8
	Casilla_dinamica,//columnas 3-5
	Casilla_poder//uno de los 5 puntos estratégicos del tablero
};

//estrucura de casilla
//vamos a crear un struct ya que todos los miembros van a ser públicos y no hay invariante que proteger
//ahora, que primero vamos a dibujar el tablero vacio solo necesitaremos el tipo de terreno que será de tipocasilla
struct Casilla {
	TipoCasilla tipo{ Casilla_dinamica };//Inicializamos aquí en la declaración 
};
class Tablero
{
};

