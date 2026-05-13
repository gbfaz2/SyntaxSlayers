//tablero.h - lógica del tablero 9x9.
//Lo separamos de tablerogl.h porque tablero sabe qué hay en cada casilla y tablerogl sabe cómo pintarlo
//cada casilla dinamica tiene un estado que oscila local-neutral-rival-neutral-local-...
//el cambio se produce gradualmente usando una fase 0.1 que tablerogl interpola para colorear la casilla. Segun la fase 0 maxima ventaja local, 0.5 neutral y 1.0 máxima ventaja rival
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

//bando de una pieza
enum Bando {
	bando_nada, //sin pieza
	bando_local, //pieza cristiana(de momento)
	bando_rival //pieza musulmana (de momento)
};

//tipo de pieza (forma geometrica de momento)
enum TipoPieza {
	pieza_nada,
	pieza_capitan,//esfera grande-1 por bando
	pieza_soldado,//cubo-6 por bando
	pieza_caballero,//piramide-3 por bando
	pieza_arquero//cilindro -3 por bando
};

//estrucura de casilla
//vamos a crear un struct ya que todos los miembros van a ser públicos y no hay invariante que proteger
//ahora, que primero vamos a dibujar el tablero vacio solo necesitaremos el tipo de terreno que será de tipocasilla
//añado un campo fase para que el valor oscile continuamente en el tiempo
//añado que piza hay y a que bando pertenece 
struct Casilla {
	TipoCasilla tipo{ Casilla_dinamica };//Inicializamos aquí en la declaración
	float fase{ 0.0f };//0-local, 0.5-neutral, 1-rival
	TipoPieza pieza{ pieza_nada };
	Bando bando{ bando_nada };
};
//con la clase tablero voy a gestionar la cuadrícula lógica de 9x9. Para ello un array estático donde vamos a fijar el tamaño N=9
//añado un método que avanza el ciclo  de luz, recibe el tiempo transcurrido desde el ultimo frame y actualiza la fase de cada casilla dinámica
class Tablero
{
	Casilla tablero[9][9];// la cuadrícula que es un array estático de 9*9
	static constexpr float CYCLE_SPEED = 0.05f;//va a ser un ciclo cada 20 segundos

	void iniCasillas();//asigna tipos de casilla
	void iniPiezas();//coloca las piezas en sus posiciones iniciales
	bool esPuntoPoder(int fila, int col) const;

	//coloca una pieza de forma explícita para evitar confusión de índices
	void lugar(int fila, int col, TipoPieza p, Bando b) {
		tablero[fila][col].pieza = p;
		tablero[fila][col].bando = b;
	}
public:
	static const int N = 9;//constante no vamos a modificarlo
	Tablero();//constructor por defecto que llama al método iniCasillas()

	int getSize() const { return N; }
	//acceso de solo lectura a una casilla, lo usará tablerogl para saber qué color pintar. usaremos (0,0) como la esquina superior izquierda 
	const Casilla& getCasilla(int fila, int col) const { return tablero[fila][col]; }// el valor de retorno es const para que no se pueda modificar desde fuera
	Casilla& getCasilla(int fila, int col) { return tablero[fila][col]; }
	void update(double dt);//en cada frame sumo (dt*velocidad) a la fase de cada casilla, y en tablerogl interpola el color entre el color oscuro y el claro según el valor de fase
	//añado método para ver si se puede mover la pieza 
	bool puedeMover(int fr, int fc, int tr, int tc)const;
	//añado método para ver si va a haber combate o no, false si no hay combate y true si hay combate
	bool muevePieza(int fr, int fc, int tr, int tc);
};

