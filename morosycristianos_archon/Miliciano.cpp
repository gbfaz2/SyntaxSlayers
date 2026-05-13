#include "Miliciano.h"
#include <iostream>


Miliciano::Miliciano(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Miliciano" : "Ghazí", //nombre pieza
        //dependiendo si es cristiano, miliciano, si no, ghazí (musulman)
        bando,
        30,   // vidaMax   — poca vida, es carnaza del frente
        40,   // fuerza    — daño básico, cuerpo a cuerpo (espada corta / daga)
        70,   // velAtaque — rápido atacando, su principal ventaja
        30,   // recarga   — cooldown bajo, puede atacar seguido
        3     // radioMov  — movimiento estándar terrestre
    )
{
    // vacío, PiezaTerrestre (y Pieza) ya inicializan todo
}


void Miliciano::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();
}