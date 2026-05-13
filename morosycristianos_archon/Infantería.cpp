#include "InfanterÍa.h"
#include <iostream>

Infantería::Infantería(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Infanteria" : "Guardia Negra", //nombre pieza
        //dependiendo si es cristiano, infanteria, si no, guardia negra(musulman)
        bando,
        100,  // vidaMax   — la pieza con más vida del juego, puro tanque
        95,   // fuerza    — daño brutal, maza pesada / hacha de doble filo
        20,   // velAtaque — muy lenta atacando, su gran debilidad
        90,   // recarga   — cooldown altísimo, tarda mucho entre ataques
        2     // radioMov  — movimiento muy corto, avanza despacio
    )
{
    // vacío, PiezaTerrestre (y Pieza) ya inicializan todo
}

void Infantería::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();
   
}