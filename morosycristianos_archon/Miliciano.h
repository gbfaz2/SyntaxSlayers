#pragma once
#include "PiezaTerrestre.h"

// MILICIANO(cristiana) / GHAZÍ(Musulmana)
//NADA ESPECIAL

class Miliciano :
    public PiezaTerrestre
{
    // Constructor que recibe bando para poder decidir en el miliciano.cpp el bando y que se llame de una manera o de otra
    //miliciano(cristiano) y Ghazí(musulman)
    Miliciano(Bando bando);

    //metodo heredaro de piezaterrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;
};

