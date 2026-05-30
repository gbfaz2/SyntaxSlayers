#include "GestorTurnos.h"

//inicializo constructor con variables 
GestorTurnos::GestorTurnos()
    : _turnoActual(bando_local),        // empieza el bando local (cristiano)
    _tiempoRestante(TIEMPO_LIMITE_TURNO),  //Definido como const
    _numeroTurno(1)  //empieza contando turnos desde 1
{
    //imprime el tunro, el bando, y el tiempo
    std::cout << "Turno 1 - CRISTIANO. Tiempo: " << TIEMPO_LIMITE_TURNO << "s\n";
}

//función para cambiar de turno: 
//-cambia el bando al contrario
//-incrementa el numero de turnos 
//-resetea el cronometro
void GestorTurnos::cambiarTurno()
{
    // Alternamos entre bando_local y bando_rival
    _turnoActual = (_turnoActual == bando_local) ? bando_rival : bando_local;

    // Reseteamos el cronómetro para el nuevo turno
    _tiempoRestante = TIEMPO_LIMITE_TURNO;

    // Incrementamos el contador de turnos
    _numeroTurno++;

    //informa por pantalla
    std::cout << "Turno " << _numeroTurno << " - "
        << (_turnoActual == bando_local ? "CRISTIANO" : "AL ANDALUS")
        << ". Tiempo: " << TIEMPO_LIMITE_TURNO << "s\n";
}

//Calcula el tiempo restante
//si el tiempo restante es igual o menor que 0 -> cambia turno.
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

//Si ya ha movido pieza, también se cambia de turno
void GestorTurnos::terminarTurno()
{
    //Informa del cambio de turno
    std::cout << "[GestorTurnos] Turno terminado por el jugador." << std::endl;
    cambiarTurno();
}


//Comprueba si la ficha que vas a mover es de tu bando o no segun tu turno.
bool GestorTurnos::esDelBandoActual(const Tablero& tablero, int fila, int col) const
{
    // Consultamos el bando de la pieza en esa casilla
    // Si es del bando que tiene el turno ahora, devuelve true
    return tablero.getCasilla(fila, col).bando == _turnoActual;

}