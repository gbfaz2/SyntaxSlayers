#pragma once
#include "PiezaTerrestre.h"
#include"Hechicero.h"
#include"ArmaCuerpoaCuerpo.h"

// REY(cristiana) / EMIR(Musulmana)
//HERENCIA MÚLTIPLE (triple)
//HABILIDAD ESPECIAL: puede usar conjuros

class Rey :
    public PiezaTerrestre, public Hechicero, public ArmaCuerpoaCuerpo
{
public:
    // Constructor que recibe bando para poder decidir en el rey.cpp el bando y que sea rey(cristiano) o emir(musulman)
    Rey(Bando bando);

    //Hederado de hechicero
    // Implementa aplicarConjuro de Hechicero
    void aplicarConjuro(Conjuro c) override;

    //Hederado de piezaTerrestre
    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;


};

