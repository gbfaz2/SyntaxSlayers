#include "CaballeríaLigera.h"
#include <iostream>

CaballeríaLigera::CaballeríaLigera(Bando bando)
    : PiezaVoladora(
        bando == Bando::CRISTIANO ? "Caballeria Ligera" : "Jinete Bereber",  //nombre pieza
        // Bando cristiano: Caballeria Ligera, bando andalusí: Jinete Bereber
        bando,
        50,   // vidaMax   — vida media, no es un tanque
        60,   // fuerza    — daño medio, carga rápida con lanza
        90,   // velAtaque — muy rápida atacando, su principal ventaja
        30,   // recarga   — cooldown bajo, puede atacar seguido
        5     // radioMov  — el mayor radio de movimiento volador
    ), ArmaCarga("Lanza") //usan lanza siempre, sean cristianos o andalusí
{
    // vacío, PiezaVoladora (y Pieza) ya inicializan todo
}

void CaballeríaLigera::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();

    //informa del arma y su tipo
    std::cout << "Arma     : " << getNombreArma() << " (Carga)" << std::endl;
}
