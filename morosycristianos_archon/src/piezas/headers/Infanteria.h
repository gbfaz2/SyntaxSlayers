#pragma once
#include "PiezaTerrestre.h"
#include"ArmaCuerpoaCuerpo.h"

// INFANTERÍA(cristiana) / GUARDIA NEGRA(Musulmana)
// Herencia múltiple: pieza terrestre con arma cuerpo a cuerpo
// La pieza más resistente del juego. Lenta pero muy fuerte. Extrema
//NADA ESPECIAL

class Infanteria :
    public PiezaTerrestre, public ArmaCuerpoaCuerpo
{
    Infanteria(Bando bando);

    //METODO HEREDADO DE PIEZA TERRESTRE
    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

