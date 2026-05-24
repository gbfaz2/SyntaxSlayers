
//Autor: María Heredero

// lo usamos para separar la lógica del dibujo. Primero probaremos a hacer un tablero vacío con un aspecto visual del Archon original
#pragma once
#include "tablero.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"
#include "Hechicero.h"
#include "SpriteRey.h"
#include "menu.h"
//#include"coordinador.h"

//creo las enumeraciones con las variables del raton y de las teclas especiales para signarles el mismo valor que tenemos en el freeglut.h
enum { MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON };
enum { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

class Tablerogl
{
	friend class GestorInput; // ACCESO TOTAL AL GESTOR DE INPUT
	friend class Coordinador;//ACCESO PARA BANDO INICIAL Y CASILLAS DINAMICAS
	friend class DibujaTablero;//ACCESO PARA DIBUJO

	GestorMovimiento gestorMovimiento;
	GestorTurnos     gestorTurnos;
	SpriteRey   _spriteReyLocal;   // rey del bando local (cristiano)
	//SpriteRey   _spriteReyRival;   // emir del bando rival (andalusí) — misma textura por ahora


	int _batallaActual{ 0 }; // 0=GUADALETE, 1=ALARCOS, 2=NAVAS_TOLOSA, 3=GRANADA

	static int _anchoVentana;
	static int _altoVentana;

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
	float _tiempoMensajeInvalido{ 0.0f };
	std::string _mensajeInvalido;
	int _turnosJugados{ 0 };
	TipoCasilla _casillaColision;
	Pieza* _pAtacante{ nullptr };//puntero al obj que sigue en tablero
	Pieza* _pDefensora{ nullptr };//extraída del tablero

	bool _modoHechizo{ false };           // HECHIZO PENDIENTE DE SELECCIONAR CASILLA
	Conjuro _conjuroActivo{ Conjuro::AVITUALLAMIENTO }; // CONJURO SELECCIONADO
	BandoPieza _bandoHechizo{ bando_nada }; // BANDO QUE ACTIVO EL HECHIZO

	//int anchoVentana{ 800 };  
	//int altoVentana{ 600 };   
public:
	Tablerogl(Tablero* pb);//constructor que inicializaremos en el .cpp con inicializadores
	virtual ~Tablerogl() { SpriteRey::liberarTextura(); }//destructor virtual + destruir rey

	void trySelectorMove(BandoPieza bando);

	void redimensionar(int ancho, int alto);

	//conversores de coordenadas cogidas del repositorio de Pablo
	void cell2center(int casilla_x, int casilla_y, float& glx, float& gly);
	void world2cell(double x, double y, int& casilla_x, int& casilla_y);//dado un punto (x,y) en coordenadas opengl, devuelve la fila y columna del tablero

	bool huboColision() const { return _combatePendiente; }
	Pieza* getPiezaAtacante() const { return _pAtacante; }
	TipoCasilla getCasillaColision() const { return _casillaColision; }
	Pieza* getPiezaDefensora() const { return _pDefensora; }

	void limpiarCombate();//para liberar a la defensora y resetear los flags
	void mostrarMensajeInvalido(const std::string& mensaje) {
		_mensajeInvalido = mensaje;
		_tiempoMensajeInvalido = 2.0f;
	}
	//descuenta el temporizador del cartel 
	void updateMensaje(double dt){
		if (_tiempoMensajeInvalido > 0.0f)
			_tiempoMensajeInvalido -= (float)dt;
	}
	//fija el bando que empieza según la batalla elegida en el menu 
	void setBandoInicial(BandoPieza bando) {
		gestorTurnos.setBandoInicial(bando);
	}
	//aplicar intercambio de casillas dinámicas
	void aplicarCambiosDinamicos();
	static const int TURNOS_DINAMICOS = 4;//cada cuatro turnos se intercamnian local por rival

	void setVictoria(BandoPieza ganador) { victoria_ = ganador; }

	void setBatalla(int b) { _batallaActual = b; } // SETTER

	static void setVentana(int ancho, int alto) { _anchoVentana = ancho; _altoVentana = alto; }

};


