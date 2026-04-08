//solo contiene la lógica del juego del tablero 9x9
#pragma once
#include <iostream>
#include "Casilla.h"
#include "Configequipo.h"

using namespace std;

#define TAMANO_TABLERO 9

//creamos la clase del tablero
class Tablero {
protected:
	const int N; //es una matriz de NXN casillas
	Casilla** tabla;//array dinámico

	ConfigEquipo equipoLocal;
	ConfigEquipo equipoVisitante;

	int filaSelec; // casilla seleccionada (-1 si ninguna)
	int colSelec; 
public:
	//constructor:usa ConfigEquipo::crear() para obtener los datos del equipo
	Tablero(TipoEquipo local, TipoEquipo visitante) : N(TAMANO_TABLERO), filaSelec(-1), colSelec(-1) {
		//Metodo estatico, pero ahora la logica vive dentro de la propia clase
		equipoLocal = ConfigEquipo::crear(local);
		equipoVisitante = ConfigEquipo::crear(visitante);

		//reservar memoria 
		tabla = new Casilla * [N];
		for (int i = 0; i < N; i++)
			tabla[i] = new Casilla[N];

		inicializarZonas();
		marcarPuntosPoder();
		marcarDinamicas();
	}
	~Tablero() {
		for (int i = 0; i < N; i++)
			delete[] tabla[i];
		delete[] tabla;
	}
	//getters
	int getTamano() const { return N; }
	Casilla** getTabla() { return tabla; }

	const ConfigEquipo& getEquipoLocal() const { return equipoLocal; }
	const ConfigEquipo& getEquipoVisitante() const { return equipoVisitante; }
	//la seleccion con el raton
	void seleccionar(int fila, int col);
	void limipiarSeleccion();
	bool haySeleccion() const { return filaSelec >= 0; }
	int getFilaSelec() const { return filaSelec; }
	int geggtColSelec() const { return colSelec; }
	//ocupacion
	void establecerOcupante(int fila, int col, TipoOcupante quien);

	//oscilacion (llamar al final de cada turno)
	void avanzarOscilacion(int turnoActual, int cadaCuantosTurnos = 3);
	//ventaja de combate en una casilla (para sofia y gabriela sirve para que la llamen al iniciar un duelo 1vs1
	VentajaCombate getVentaja(int fila, int col, TipoOcupante quien)const;
	//condicion de victoria
	bool controlCincoPuntos(TipoOcupante quien) const;
	//comprobacion de limetes
	bool enTablero(int fila, int col) const {
		return fila >= 0 && fila < N && col >= 0 && col < N;
	}
private:
	void inicializarZonas();
	void marcarPuntosPoder();
	void marcarDinamicas();
};
