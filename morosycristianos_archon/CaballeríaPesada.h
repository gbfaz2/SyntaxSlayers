#pragma once
#include "PiezaTerrestre.h"

//  CABALLERÍA PESADA(cristiana) / CABALLERÍA ACORAZADA(Musulmana)

class CaballeríaPesada :
    public PiezaTerrestre
{
    // Constructor que recibe bando para poder decidir en el miliciano.cpp el bando y que se llame de una manera o de otra
    //Caballería Pesada(cristiano) y Caballería Acorazada(musulman)
    CaballeríaPesada(Bando bando);

    //metodo heredaro de piezaterrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;
};

