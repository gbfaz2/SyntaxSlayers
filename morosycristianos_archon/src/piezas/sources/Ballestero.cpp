#include "Ballestero.h"
#include <iostream>

Ballestero::Ballestero(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Ballestero" : "Arquero",  //nombre de la pieza
        // bando cristiano: Ballestero, bando andalusí: Arquero 
        bando,
        20,   // vidaMax   — la pieza con menos vida del juego, muy frágil
        60,   // fuerza    — daño medio, proyectil a distancia
        80,   // velAtaque — rápido atacando, segunda mayor velocidad terrestre
        50,   // recarga   — cooldown medio
        3     // radioMov  — movimiento estándar terrestre
    ), ArmaDistancia(bando == Bando::CRISTIANO ? "Ballesta" : "Arco") //inicializas arma según bando
{
    // vacío, PiezaTerrestre (y Pieza) ya inicializan todo
}



void Ballestero::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();

    //informa arma escogida y tipo
    std::cout << "Arma     : " << getNombreArma() << " (A distancia)" << std::endl;
 
}
