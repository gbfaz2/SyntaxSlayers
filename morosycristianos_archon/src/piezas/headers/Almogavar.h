#pragma once
#include "PiezaVoladora.h"
#include "ArmaDistancia.h"

// ALMOGÁVAR (Cristiano) / ARQUERO A CABALLO (Musulman)
// Pieza voladora de proyectil. Fuerte y resistente, pero más lenta que la CaballeríaLigera.
//Herencia múltiple: pieza voladora que usa armas a distancia
//NADA ESPECIAL

class Almogavar :
    public PiezaVoladora, public ArmaDistancia
{
public:
    // Constructor que solo recibe el bando, el resto de stats están fijos en el .cpp, con el bando se decide el nombre de la pieza
    //Bando cristiano: almogávar, bando musulman: arquero a caballo
    Almogavar(Bando bando);

    //herencia de piezaVoladora
    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

