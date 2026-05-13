#pragma once
#include "Pieza.h"
class PiezaTeleporte :
    public Pieza
{
public:

    //constructor para pasarle estos datos a pieza y que se creen las variables
    PiezaTeleporte(std::string nombre, Bando bando,
        int vidaMax, int fuerza,
        int velAtaque, int recarga, int radioMov);

    // MÉTODOS VIRTUALES de pieza.h
    TipoMovimiento getTipoMovimiento() const override {
        return TipoMovimiento::TELEPORTE;  //devuelve terrestre de la clase tipomovimiento declarada en pieza.h
    }

    //definicion 
    bool puedeMoverse(int filaDestino, int colDestino) const override;

    //override sirve para informar que estamos definiendo métodos virtuales de la clase madre (pieza)

};

