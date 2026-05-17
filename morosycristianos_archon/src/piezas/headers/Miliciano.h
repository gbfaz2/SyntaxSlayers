#pragma once
#include "PiezaTerrestre.h"
#include "ArmaCuerpoaCuerpo.h"

// MILICIANO(cristiana) / GHAZÍ(Musulmana)
// Herencia múltiple: pieza terrestre que tiene armas cuerpo a cuerpo
//NADA ESPECIAL

class Miliciano :
    public PiezaTerrestre, public ArmaCuerpoaCuerpo
{
    // Constructor que recibe bando para poder decidir en el miliciano.cpp el bando y que se llame de una manera o de otra
    //miliciano(cristiano) y Ghazí(musulman)
    Miliciano(Bando bando);

    //metodo heredaro de piezaterrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;
};

