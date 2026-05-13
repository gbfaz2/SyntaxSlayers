#pragma once
#include "PiezaTerrestre.h"
#include "ArmaCarga.h"

//  CABALLERÍA PESADA(cristiana) / CABALLERÍA ACORAZADA(Musulmana)
//Herencia multiple: pieza terrestre con arma carga
//NADA ESPECIAL

class CaballeríaPesada :
    public PiezaTerrestre, public ArmaCarga
{
    // Constructor que recibe bando para poder decidir en el miliciano.cpp el bando y que se llame de una manera o de otra
    //Caballería Pesada(cristiano) y Caballería Acorazada(musulman)
    CaballeríaPesada(Bando bando);

    //metodo hederado de piezaterrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;
};

