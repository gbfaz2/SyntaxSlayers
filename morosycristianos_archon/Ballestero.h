#pragma once
#include "PiezaTerrestre.h"

// BALLESTERO(Cristiano)/ ARQUERO(musulman)
// Pieza de proyectil terrestre. Poca vida pero buen daño y velocidad de ataque.
//NADA ESPECIAL

class Ballestero :
    public PiezaTerrestre
{
    // Constructor para recibe el bando y decidir nombre de la pieza 
    //Bando cristiano: ballestero, bando andalusí: arquero
    Ballestero(Bando bando);

    //herencia de piezaTerrestre
    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

