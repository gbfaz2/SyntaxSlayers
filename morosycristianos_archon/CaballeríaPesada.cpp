#include "CaballerÍaPesada.h"
#include <iostream>

CaballeríaPesada::CaballeríaPesada(Bando bando)
    : PiezaTerrestre(
        bando == Bando::CRISTIANO ? "Caballeria Pesada" : "Caballeria Acorazada",  //nombre pieza
        // si es cristiano Caballeria Pesada, si es musulman Caballeria Acorazada
        bando,
        90,   // vidaMax   — muy resistente, segunda pieza con más vida
        80,   // fuerza    — daño alto, proyectil/carga lineal de impacto
        40,   // velAtaque — velocidad media-baja atacando
        60,   // recarga   — cooldown medio, no puede spamear ataques
        3     // radioMov  — movimiento estándar terrestre
    ), ArmaCarga("Lanza") //usan lanza siempre, sean cristianos o andalusí
{
    // vacío, PiezaTerrestre (y Pieza) ya inicializan todo
}

void CaballeríaPesada::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();

    //informa del arma y su tipo
    std::cout << "Arma     : " << getNombreArma() << " (Carga)" << std::endl;
}
