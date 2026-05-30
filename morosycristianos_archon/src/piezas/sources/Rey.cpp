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
    Hechicero(),// inicializa los conjuros a false
    Arma(bando == Bando::CRISTIANO ? "Punal" : "Daga oculta", TipoAtaque::CUERPO_A_CUERPO) //inicializa a una arma u a otra segun bando
{
}

void Rey::mostrarStats() const {
    // Llama al mostrarStats de Pieza
    Pieza::mostrarStats();

    //informa que arma lleva y que tipo de arma 
    std::cout << "Arma     : " << getNombreArma() << " (Cuerpo a cuerpo)" << std::endl;


    // Añade info de conjuros de si el elegido esta gastado o no (especial rey conjuro)
    std::cout << "--- Conjuros ---" << std::endl;
    std::cout << "Rutas Secretas    : " << (puedeUsarConjuro(Conjuro::RUTAS_SECRETAS) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Avituallamiento   : " << (puedeUsarConjuro(Conjuro::AVITUALLAMIENTO) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Relevo Guardia    : " << (puedeUsarConjuro(Conjuro::RELEVO_GUARDIA) ? "Disponible" : "Gastado") << std::endl;
    std::cout << "Asedio            : " << (puedeUsarConjuro(Conjuro::ASEDIO) ? "Disponible" : "Gastado") << std::endl;
}
