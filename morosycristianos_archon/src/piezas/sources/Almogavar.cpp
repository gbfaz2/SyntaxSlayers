#include "Almogavar.h"
#include <iostream>

// Comentario: El constructor debe recibir bando y pasárselo a las clases padre
Almogavar::Almogavar(Bando bando)
    : PiezaVoladora(
        bando == Bando::CRISTIANO ? "Almogavar" : "Arquero a Caballo",
        bando,
        60,   // vidaMax
        85,   // fuerza
        50,   // velAtaque
        70,   // recarga
        4     // radioMov
    ), // Comentario: Aquí cerramos el constructor de PiezaVoladora
    ArmaDistancia(bando == Bando::CRISTIANO ? "Jabalinas incendiarias" : "Flechas")
{
    // Comentario: El cuerpo del constructor se queda vacío
}

void Almogavar::cargarIncendiaria() {
    // Comentario: Usamos std::cout para evitar la ambigüedad del operador <<
    std::cout << "[" << getNombre() << "] Carga Incendiaria activada!" << std::endl;
    std::cout << "  Ataque de area: quema enemigos cercanos" << std::endl;
    std::cout << "  Daño bloqueado durante unos segundos" << std::endl;
}

void Almogavar::mostrarStats() const {
    // Comentario: Llamamos explícitamente a la clase padre para evitar líos de herencia
    PiezaVoladora::mostrarStats();

    // Comentario: Accedemos al nombre del arma desde la rama de ArmaDistancia
    std::cout << "Arma     : " << ArmaDistancia::getNombreArma() << " (A distancia)" << std::endl;
    std::cout << "Habilidad : Carga Incendiaria (ataque de area)" << std::endl;
}