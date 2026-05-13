#include "ArmaCuerpoaCuerpo.h"
#include <iostream>

//constructor inicializa con nombres de variables
ArmaCuerpoaCuerpo::ArmaCuerpoaCuerpo(std::string nombreArma)
    : Arma(nombreArma, TipoAtaque::CUERPO_A_CUERPO)
{
}

//metodo para realizar ataque
//de momento solo imprime por pantalla lo que hace y con que arma
void ArmaCuerpoaCuerpo::realizarAtaque() const {
    std::cout << "Dando un tajo cuerpo a cuerpo con: " << nombreArma << "!" << std::endl;
}
