#include "Pieza.h"
#include <iostream>

// Constructor inicializar con variables iniciales, posicion 0, no en casilla especial, y el resto con variables

Pieza::Pieza(std::string nombre, Bando bando, int vidaMax, int fuerza, 
    int velAtaque, int recarga, int radioMov): nombre(nombre), bando(bando), vidaMax(vidaMax),
    fuerza(fuerza), velAtaque(velAtaque), recarga(recarga), radioMov(radioMov),
    fila(0), columna(0), viva(true), enPuntoDePoder(false)  //inicializas estas variables porque sino daria basura, aunque no esten en el constructor
{
    // cuerpo vacío, todo ya está inicializado arriba
}

//definicion de metodos
// Resta vida cuando la atacan
void Pieza::recibirDanio(int danio) {
    vida -= danio;  //se va restando el daño
    if (vida <= 0) {
        vida = 0;
        viva = false;  // la pieza muere si la vida <=0
    }
}

// Suma vida, con bonus si está en punto de poder
void Pieza::curar(int cantidad) {
    if (enPuntoDePoder)
        vida += cantidad * 2;  // cura doble en punto de poder
    else
        vida += cantidad;  

    // Que no supere el máximo
    if (vida > vidaMax)  //limite vida max
        vida = vidaMax;
}

// Muestra las stats por consola (para la demo)
void Pieza::mostrarStats() const {
    std::cout << "=== " << nombre << " ===" << std::endl;
    std::cout << "Bando    : "
        << (bando == Bando::CRISTIANO ? "Cristiano" : "Andalusi")
        << std::endl;
    std::cout << "Vida     : " << vida << "/" << vidaMax << std::endl;
    std::cout << "Fuerza   : " << fuerza << std::endl;
    std::cout << "Vel.Atq  : " << velAtaque << std::endl;
    std::cout << "Recarga  : " << recarga << std::endl;
    std::cout << "RadioMov : " << radioMov << std::endl;
    std::cout << "Posicion : (" << fila << "," << columna << ")" << std::endl;
    std::cout << "Viva     : " << (viva ? "Si" : "No") << std::endl;
}