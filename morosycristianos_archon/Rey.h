#pragma once
#include "PiezaTerrestre.h"
#include"Hechicero.h"

// REY(cristiana) / EMIR(Musulmana)
//Puede lanzar hechicos, herencia multiple


class Rey :
    public PiezaTerrestre, public Hechicero
{
public:
    // Constructor que recibe bando para poder decidir en el rey.cpp el bando y que sea rey(cristiano) o emir(musulman)
    Rey(Bando bando);

    //métodos normales:
    // Implementa aplicarConjuro de Hechicero
    void aplicarConjuro(Conjuro c) override;

    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;


};

