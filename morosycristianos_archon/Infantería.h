#pragma once
#include "PiezaTerrestre.h"

// INFANTERÍA(cristiana) / GUARDIA NEGRA(Musulmana)
// La pieza más resistente del juego. Lenta pero muy fuerte. Extrema
//NADA ESPECIAL

class Infantería :
    public PiezaTerrestre
{
    Infantería(Bando bando);

    //METODO HEREDADO DE PIEZA TERRESTRE
    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

