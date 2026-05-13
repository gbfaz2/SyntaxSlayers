#include "Rey.h"
#include <iostream>

Rey::Rey(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Rey" : "Emir",   //nombre pieza
        //dependiendo si es cristiano, rey, si no, emir (rey musulman)
        bando,
        80,   // vidaMax
        75,   // fuerza
        60,   // velAtaque
        40,   // recarga
        3     // radioMov
    ),
    Hechicero()  // inicializa los conjuros a false
{
}

void Rey::aplicarConjuro(Conjuro c) {
    // Primero comprueba que puede usarlo
    if (!puedeUsarConjuro(c)) {
        //si es FALSE (no puede usar conjuro), va a hacerse TRUE con ! y por tanto se ejecutara:

        std::cout << "Conjuro ya gastado!" << std::endl;
        return; //sale del método completamente para no ejecutarlo, hechizo gastado
    }


    // Lo marca como gastado
    usarConjuro(c);

    // Aplica el efecto: de momento solo imprime por pantalla, cuando tengmos el resto de piezas y tablero cambiaremos la logica a la real
    switch (c) {
    case Conjuro::RUTAS_SECRETAS:
        std::cout << "[" << nombre << "] Rutas Secretas: teleporta una tropa aliada" << std::endl;
        break;
    case Conjuro::AVITUALLAMIENTO:
        std::cout << "[" << nombre << "] Avituallamiento: cura una tropa aliada" << std::endl;
        break;
    case Conjuro::CONTROL_FRONTERAS:
        std::cout << "[" << nombre << "] Control de Fronteras: cambia casillas oscilantes" << std::endl;
        break;
    case Conjuro::RELEVO_GUARDIA:
        std::cout << "[" << nombre << "] Relevo de Guardia: intercambia dos tropas aliadas" << std::endl;
        break;
    case Conjuro::MERCENARIOS:
        std::cout << "[" << nombre << "] Mercenarios: invoca tropa temporal" << std::endl;
        break;
    case Conjuro::REFUERZOS:
        std::cout << "[" << nombre << "] Refuerzos: resucita una tropa caída" << std::endl;
        break;
    case Conjuro::ASEDIO:
        std::cout << "[" << nombre << "] Asedio: bloquea una tropa enemiga" << std::endl;
        break;
    }
}

void Rey::mostrarStats() const {
    // Llama al mostrarStats de Pieza
    Pieza::mostrarStats();

    // Añade info de conjuros de si el elegido esta gastado o no (especial rey conjuro)
    std::cout << "--- Conjuros ---" << std::endl;
    std::cout << "Rutas Secretas    : " << (puedeUsarConjuro(Conjuro::RUTAS_SECRETAS) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Avituallamiento   : " << (puedeUsarConjuro(Conjuro::AVITUALLAMIENTO) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Control Fronteras : " << (puedeUsarConjuro(Conjuro::CONTROL_FRONTERAS) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Relevo Guardia    : " << (puedeUsarConjuro(Conjuro::RELEVO_GUARDIA) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Mercenarios       : " << (puedeUsarConjuro(Conjuro::MERCENARIOS) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Refuerzos         : " << (puedeUsarConjuro(Conjuro::REFUERZOS) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Asedio            : " << (puedeUsarConjuro(Conjuro::ASEDIO) ? "Disponible" : "Gastado") << std::endl;
}
