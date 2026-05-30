
#pragma once
#include "tablero.h"
#include "GestorMovimiento.h"
#include "GestorTurnos.h"
#include "Hechicero.h"
#include "Pieza.h"
#include "GestorHechizos.h"

//creo las enumeraciones con las variables del raton y de las teclas especiales para signarles el mismo valor que tenemos en el freeglut.h
enum { MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON };
enum { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

class Tablerogl
{
	friend class GestorInput; // LE DOY PERMISO AL GESTOR DE INPUT
	friend class Coordinador; //LE DOY PERMISO AL COORDINADOR PARA BANDO INICIAL Y CASILLAS DINAMICAS
	friend class DibujaTablero; //LE DOY PERMISO A DIBUJATABLERO PARA HACER EL DIBUJO

	GestorMovimiento gestorMovimiento;
	GestorTurnos     gestorTurnos;

	int _batallaActual{ 0 }; // 0=GUADALETE, 1=ALARCOS, 2=NAVAS_TOLOSA, 3=GRANADA

	static int _anchoVentana;
	static int _altoVentana;

	//Movimiento interpolado:
	struct AnimMovimiento {
		Pieza* pieza; // qué pieza se está moviendo
		float  origenX, origenY; // posición GL de origen
		float  destinoX, destinoY; // posición GL de destino
		float  t; // progreso 0.0 → 1.0
		bool   activa{ false };  //flag para saber si la pieza esta moviendose
	};
	AnimMovimiento _animMov;

	int _pausaSeleccion{ 0 }; // OPCIÓN SELECCIONADA EN MENÚ PAUSA (0-3)

protected:
	float ancho;
	int N;
	Tablero* m_tablero;

	double centro_x, centro_y, centro_z;
	double dist;

	float _tiempoCuracionPoder{ 0.0f }; //contador para curar cada segundo a las piezas que estén en los puntos de poder

	int Filacursor[2], Colcursor[2];
	int fromFila, fromCol; //origen del movimiento en curso
	BandoPieza fromBando;
	BandoPieza victoria_; //resultado de chackVictoria actualizado cada movimiento
	bool piezaSeleccionada; //hay una pieza seleccionada esperando destino

	int xcasilla_sel, ycasilla_sel; //casilla bajo el cursor
	bool controlKey, shiftKey;
	bool leftButton, rightButton, midButton;

	bool _combatePendiente{ false };
	float _tiempoMensajeInvalido{ 0.0f };
	std::string _mensajeInvalido;
	int _turnosJugados{ 0 };
	Pieza* _pAtacante{ nullptr }; //puntero al obj que sigue en tablero
	Pieza* _pDefensora{ nullptr }; //extraída del tablero

	int getVentajaTerrenoCombate()const;

	bool _modoHechizo{ false }; // HECHIZO PENDIENTE DE SELECCIONAR CASILLA
	Conjuro _conjuroActivo{ Conjuro::AVITUALLAMIENTO }; // CONJURO SELECCIONADO, por defecto será el primero 
	BandoPieza _bandoHechizo{ bando_nada }; // BANDO QUE ACTIVO EL HECHIZO
	
	bool _esperandoDestino{ false }; // ESPERANDO SEGUNDA CASILLA PARA HECHIZO DE 2 PASOS
	int  _hechizoFilaOrigen{ -1 }; // FILA ORIGEN DEL HECHIZO
	int  _hechizoColOrigen{ -1 }; // COL ORIGEN DEL HECHIZO
	bool _conjuroElegido{ false }; // EL JUGADOR YA ELIGIÓ UN NÚMERO

	//VARIABLES PARA LA HABILIDAD DEL INFILTRADO
	bool _modoInfiltrado{ false }; //controla si estamos en modo seleccionar víctima
	int _infiltradoFila{ -1 }; //guarda donde está el infiltrado original
	int _infiltradoCol{ -1 };

	// SUGERENCIA DE LA IA PARA EL JUGADOR
	int _sugerenciaFila{ -1 };
	int _sugerenciaCol{ -1 };

public:
	Tablerogl(Tablero* pb); //constructor que inicializaremos en el .cpp con inicializadores
	virtual ~Tablerogl() {} //destructor virtual + destruir rey

	void trySelectorMove(BandoPieza bando);

	void redimensionar(int ancho, int alto);

	//para actualizar el dt para la interpolacion
	void update(double dt);

	void cell2center(int casilla_x, int casilla_y, float& glx, float& gly);
	void world2cell(double x, double y, int& casilla_x, int& casilla_y); //dado un punto (x,y) en coordenadas opengl, devuelve la fila y columna del tablero

	bool huboColision() const { return _combatePendiente; }
	Pieza* getPiezaAtacante() const { return _pAtacante; }
	Pieza* getPiezaDefensora() const { return _pDefensora; }

	void limpiarCombate(); //para liberar a la defensora y resetear los flags
	void mostrarMensajeInvalido(const std::string& mensaje, bool forzar = false,bool reproducirSonido = true) {
		// SI FORZAR ES TRUE, IGNORAMOS EL BLOQUEO DE LOS HECHIZOS Y MOSTRAMOS
		if (_modoHechizo && !forzar) return;

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
	static const int TURNOS_DINAMICOS = 4; //cada cuatro turnos se intercamnian local por rival

	void setVictoria(BandoPieza ganador) { victoria_ = ganador; }

	void setBatalla(int b) { _batallaActual = b; } // SETTER

	// GETTER
	std::string nombre_j1{ "Jugador 1" };
	std::string nombre_j2{ "Jugador 2" };

	static void setVentana(int ancho, int alto) { _anchoVentana = ancho; _altoVentana = alto; }

	GestorHechizos* _gestorHechizos{ nullptr };
};