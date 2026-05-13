#pragma once
#include "PiezaTerrestre.h"

// BALLESTERO / ARQUERO
// Pieza de proyectil terrestre. Poca vida pero buen daño y velocidad de ataque.

class Ballestero :
    public PiezaTerrestre
{
    // Constructor para recibe el bando y decidir nombre de la pieza 
    //Bando cristiano: ballestero, bando andalusí: arquero
    Ballestero(Bando bando);

    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

