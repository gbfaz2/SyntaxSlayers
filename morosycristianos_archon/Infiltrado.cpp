#include "Infiltrado.h"
#include <iostream>


Infiltrado::Infiltrado(Bando bando)
    : PiezaTeleporte(
        bando == Bando::CRISTIANO ? "Infiltrado" : "Asesino de Elite",  //nombre de la pieza
        // bando cristiano: Infiltrado, bando andalusí: Asesino de Elite
        bando,
        40,   // vidaMax   — poca vida, es una pieza de golpe y huida
        90,   // fuerza    — daño muy alto, ataque cuerpo a cuerpo letal
        100,  // velAtaque — el más rápido atacando de todo el juego
        20,   // recarga   — cooldown bajísimo, puede atacar casi sin pausa
        5     // radioMov  — teleporte de largo alcance
    )
{
    // vacío, PiezaTeleporte (y Pieza) ya inicializan todo
}

void Infiltrado::copiarStats(Pieza& enemigo) {
    // ESPECIAL INFILTRADO:  copia las stats de combate del enemigo
    // Solo copia fuerza, velAtaque y recarga — no la vida ni la posición(no tiene sentido)
    fuerza = enemigo.getFuerza();
    velAtaque = enemigo.getVelAtaque();
    recarga = enemigo.getRecarga();

    //imprime por pantalla lo que va haciendo (solo para la demo)
    std::cout << "[" << nombre << "] Espionaje Tactico activado!" << std::endl;
    std::cout << "  Copiando stats de: " << enemigo.getNombre() << std::endl;
    std::cout << "  Fuerza    -> " << fuerza << std::endl;
    std::cout << "  VelAtaque -> " << velAtaque << std::endl;
    std::cout << "  Recarga   -> " << recarga << std::endl;
}

void Infiltrado::mostrarStats() const {
    // Llama al mostrarStats de Pieza 
    Pieza::mostrarStats();
    std::cout << "Habilidad : Espionaje Tactico (copia stats del enemigo)" << std::endl;
}
