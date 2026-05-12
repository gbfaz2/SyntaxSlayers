#pragma once
#include "PiezaTerrestre.h"
#include"Hechicero.h"
class Rey :
    public PiezaTerrestre, public Hechicero
{
public:
    // Constructor que recibe bando que es lo que va a variar con respecto a las demás piezas terrestres
    Rey(Bando bando);

    //métodos normales:
    // Implementa aplicarConjuro de Hechicero
    void aplicarConjuro(Conjuro c) override;

    // Sobreescribe mostrarStats para añadir info de conjuros
    void mostrarStats() const override;


};

