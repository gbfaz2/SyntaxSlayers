#include "Almogavar.h"
#include <iostream>

//El constructor debe recibir bando y pasárselo a las clases padre
Almogavar::Almogavar(Bando bando)
    : PiezaVoladora(
        bando == Bando::CRISTIANO ? "Almogavar" : "Arquero a Caballo",
        bando,
        60,   // vidaMax
        85,   // fuerza
        50,   // velAtaque
        70,   // recarga
        4     // radioMov
    ), //Aquí cerramos el constructor de PiezaVoladora
    Arma(bando == Bando::CRISTIANO ? "Jabalinas incendiarias" : "Flechas", TipoAtaque::DISTANCIA)
{
    //El cuerpo del constructor se queda vacío
}

void Almogavar::mostrarStats() const {
    //Llamamos explícitamente a la clase padre para evitar líos de herencia
    PiezaVoladora::mostrarStats();

    //Accedemos al nombre del arma desde la rama de ArmaDistancia
    std::cout << "Arma:    " << Arma::getNombreArma() << " (A distancia)" << std::endl;
}