#include "ArmaCarga.h"
#include <iostream>

//constructor inicializa con nombres de variables
ArmaCarga::ArmaCarga(std::string nombreArma)
    : Arma(nombreArma, TipoAtaque::CARGA)
{
}

//metodo para realizar ataque
//de momento solo imprime por pantalla lo que hace y con que arma
void ArmaCarga::realizarAtaque() const {
    std::cout << "Embestida con: " << nombreArma << "!" << std::endl;
}