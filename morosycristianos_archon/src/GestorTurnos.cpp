#include "GestorTurnos.h"

//inicializo constructor con variables 
GestorTurnos::GestorTurnos()
    : _turnoActual(bando_local),        // empieza el bando local (cristiano) CUIDADO DUDA PREGUNTAR 
    _tiempoRestante(TIEMPO_LIMITE_TURNO),  //ya definico como const (60seg)
    _numeroTurno(1)  //empieza contando turnos con 1
{
    //imprime el tunro, el bando, y el tiempo (proponer hacer un cronometro chulo 
    std::cout << "[GestorTurnos] Turno 1 — Bando LOCAL. Tiempo: "
        << TIEMPO_LIMITE_TURNO << "s" << std::endl;
}

//función para cambiar de turno, 
//cambia el bando al contrario
//incrementa el numero de turnos 
//resetea el cronometro

void GestorTurnos::cambiarTurno()
{
    // Alternamos entre bando_local y bando_rival
    _turnoActual = (_turnoActual == bando_local) ? bando_rival : bando_local;

    // Reseteamos el cronómetro para el nuevo turno
    _tiempoRestante = TIEMPO_LIMITE_TURNO;

    // Incrementamos el contador de turnos
    _numeroTurno++;

    //informa por pantalla (luego lo de imprimir lo quitaremos??)
    std::cout << "[GestorTurnos] Turno " << _numeroTurno << " — "
        << (_turnoActual == bando_local ? "Bando LOCAL" : "Bando RIVAL")
        << ". Tiempo: " << TIEMPO_LIMITE_TURNO << "s" << std::endl;
}

//con cada frame del main.cpp del OnTimer se actualiza:
//del tiempo inicial (60seg) le vas restando cada dt
//si el tiemporestante es igual o menor que 0, cambia turnos forzadamente
void GestorTurnos::update(double dt)
{
    // Descontamos el tiempo del turno actual
    _tiempoRestante -= (float)dt;

    // Si se acaba el tiempo, cambiamos de turno automáticamente
    if (_tiempoRestante <= 0.0f) {
        std::cout << "[GestorTurnos] Tiempo agotado! Cambiando turno..." << std::endl;
        cambiarTurno();
    }
}

//Si ya ha movido pieza, naturalmente se cambia de turno
void GestorTurnos::terminarTurno()
{
    // El jugador termina su turno voluntariamente al mover una pieza
    std::cout << "[GestorTurnos] Turno terminado por el jugador." << std::endl;
    cambiarTurno();
}


//comprueba si la ficha que vas a mover es de tu bando o no segun tu turno.
bool GestorTurnos::esDelBandoActual(const Tablero& tablero, int fila, int col) const
{
    // Consultamos el bando de la pieza en esa casilla
    // Si es del bando que tiene el turno ahora, devuelve true
    return tablero.getCasilla(fila, col).bando == _turnoActual;
}