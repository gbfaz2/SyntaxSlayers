//tablero.h - lógica del tablero 9x9.
//Lo separamos de tablerogl.h porque tablero sabe qué hay en cada casilla y tablerogl sabe cómo pintarlo
#pragma once
#include <cmath>
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
//añado un campo fase para que el valor oscile continuamente en el tiempo
struct Casilla {
	TipoCasilla tipo{ Casilla_dinamica };//Inicializamos aquí en la declaración
	float fase{ 0.5f };//fase inicial: neutro
};
//con la clase tablero voy a gestionar la cuadrícula lógica de 9x9. Para ello un array estático donde vamos a fijar el tamaño N=9
//añado un método que avanza el ciclo  de luz, recibe el tiempo transcurrido desde el ultimo frame y actualiza la fase de cada casilla dinámica
class Tablero
{
	Casilla tablero[9][9];// la cuadrícula que es un array estático de 9*9
	static constexpr float CYCLE_SPEED = 0.08f;

	void iniCasillas();//asigna tipos de casilla
	bool esPuntoPoder(int fila, int col) const;
public:
	static const int N = 9;//constante no vamos a modificarlo
	Tablero();//destructor por defecto que llama al método iniCasillas()

	int getSize() const { return N; }
	//acceso de solo lectura a una casilla, lo usará tablerogl para saber qué color pintar. usaremos (0,0) como la esquina superior izquierda 
	const Casilla& getCasilla(int fila, int col) const { return tablero[fila][col]; }// el valor de retorno es const para que no se pueda modificar desde fuera
	void update(double dt);//en cada frame sumo (dt*velocidad) a la fase de cada casilla, y en tablerogl interpola el color entre el color oscuro y el claro según el valor de fase
};

