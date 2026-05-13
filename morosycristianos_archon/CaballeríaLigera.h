#pragma once
#include "PiezaVoladora.h"

// CABALLERÍA LIGERA (cristiana)/ JINETE BEREBER (musulmana)
//NADA ESPECIAL

class CaballeríaLigera :
    public PiezaVoladora
{
    // Constructor que recibe el bando para decidir que tipo de ficha es, el resto de stats están fijos en el .cpp
    // Bando Cristiano: caballería ligera, bando musulman: jinete bereber
    CaballeríaLigera(Bando bando);


    //metodo hederado de piezaVoladora
    // Sobreescribe mostrarStats para identificar la pieza claramente y añadir info de conjuros
    void mostrarStats() const override;
};

