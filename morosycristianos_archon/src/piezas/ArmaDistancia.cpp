#include "ArmaDistancia.h"
#include <iostream>

//constructor inicializa con nombres de variables
ArmaDistancia::ArmaDistancia(std::string nombreArma)
    : Arma(nombreArma, TipoAtaque::DISTANCIA)
{
}

//metodo para realizar ataque
//de momento solo imprime por pantalla lo que hace y con que arma
void ArmaDistancia::realizarAtaque() const {
    std::cout << "Lanzando con: " << nombreArma << "!" << std::endl;
}