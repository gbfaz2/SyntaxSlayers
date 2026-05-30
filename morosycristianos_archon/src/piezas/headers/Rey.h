#pragma once
#include "PiezaTerrestre.h"
#include"Hechicero.h"
#include"Arma.h"

// REY(cristiana) / EMIR(Musulmana)
//HERENCIA MÚLTIPLE (triple)
//HABILIDAD ESPECIAL: puede usar conjuros

class Rey :
    public PiezaTerrestre, public Hechicero, public Arma
{
public:
    // Constructor que recibe bando para poder decidir en el rey.cpp el bando y que sea rey(cristiano) o emir(musulman)
    Rey(Bando bando);

    //Hederado de piezaTerrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;


};

