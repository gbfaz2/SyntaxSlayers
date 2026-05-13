#include "Infanteria.h"
#include <iostream>

Infanteria::Infanteria(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Infanteria" : "Guardia Negra", //nombre pieza
        //dependiendo si es cristiano, infanteria, si no, guardia negra(musulman)
        bando,
        100,  // vidaMax   — la pieza con más vida del juego, puro tanque
        95,   // fuerza    — daño brutal, maza pesada / hacha de doble filo
        20,   // velAtaque — muy lenta atacando, su gran debilidad
        90,   // recarga   — cooldown altísimo, tarda mucho entre ataques
        2     // radioMov  — movimiento muy corto, avanza despacio
    ), ArmaCuerpoaCuerpo(bando == Bando::CRISTIANO ? "Punal" : "Daga oculta") //inicializa a una arma u a otra segun bando
{
    // vacío, PiezaTerrestre (y Pieza) ya inicializan todo
}

void Infanteria::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();

    //informa del tipo de arma 
    std::cout << "Arma     : " << getNombreArma() << " (Cuerpo a cuerpo letal)" << std::endl;
   
}