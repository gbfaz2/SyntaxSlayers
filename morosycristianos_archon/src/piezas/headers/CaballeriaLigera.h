#pragma once
#include "PiezaVoladora.h"
#include"ArmaCarga.h"

// CABALLERÍA LIGERA (cristiana)/ JINETE BEREBER (musulmana)
// herencia múltiple: pieza voladora que usa arma carga
//NADA ESPECIAL

class CaballeriaLigera :
    public PiezaVoladora, public ArmaCarga
{
public:
    // Constructor que recibe el bando para decidir que tipo de ficha es, el resto de stats están fijos en el .cpp
    // Bando Cristiano: caballería ligera, bando musulman: jinete bereber
    CaballeriaLigera(Bando bando);


    //metodo hederado de piezaVoladora
    // Sobreescribe mostrarStats para identificar la pieza claramente y añadir info de conjuros
    void mostrarStats() const override;
};

