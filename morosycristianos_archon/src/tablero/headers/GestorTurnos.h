#pragma once
#include "Tablero.h"
#include <iostream>

//La idea principal de esta clase es gestionar los turnos: 
// Saber de quien es el turno (local o rival)
// activar un cronometro en cada turno para limitar el tiempo
// cambiar de turno al acabar el cronometro o simplemente cuando ya ha movido pieza
// comprobar que al intentar mover una pieza estas moviendo la de tu bando


constexpr float TIEMPO_LIMITE_TURNO = 60.0f; //constante tiempo limite / turno (seg)


class GestorTurnos
{

//variables privadas para que no se pueda acceder a ellas
private:
    BandoPieza           _turnoActual;       // variable tipo bando que almacena de quién es el turno ahora
    float           _tiempoRestante;    // segundos que le quedan al jugador actual
    int             _numeroTurno;       // contador de turnos totales jugados

    // Hace el cambio de turno — privado porque solo lo llama update() o terminarTurno()
    void cambiarTurno();

public:

    // Constructor para inicializar variables privadas (empieza siempre el bando local (cristiano))
    GestorTurnos();

    // Cronómetro para cada turno que se actializa con cada frame desde OnTimer (main.cpp) con dt
    // Si el tiempo llega a 0, cambia el turno automáticamente
    void update(double dt);

    // El jugador juega su turno -> termina su turno y le toca al bando contrario
    void terminarTurno();

    // Comprueba si la pieza en (fila, col) es del bando que tiene el turno ahora
    // Lo usará GestorMovimiento antes de mover para validar,
    // Si pieza que intenta mover es del bando con el turno actual -> PUEDE MOVERSE
    // Si pieza es del turno contrario -> BLOQUEO 
    bool esDelBandoActual(const Tablero& tablero, int fila, int col) const;


    // Getters para que tablerogl pueda mostrar info en pantalla
    BandoPieza getBandoActual()      const { return _turnoActual; }
    float getTiempoRestante()   const { return _tiempoRestante; }
    int   getNumeroTurno()      const { return _numeroTurno; }



};



