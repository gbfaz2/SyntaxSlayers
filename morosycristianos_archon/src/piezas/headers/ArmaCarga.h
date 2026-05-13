#pragma once
#include "Arma.h"

//Hereda de Arma

class ArmaCarga :
    public Arma
{
public:

    //constructor que recibe el nombre del arma, y segun el nombre se decide el resto de variables
    ArmaCarga(std::string nombreArma);

    //hederado de Arma
    // Define cómo ataca esta arma
    void realizarAtaque() const override;
};

