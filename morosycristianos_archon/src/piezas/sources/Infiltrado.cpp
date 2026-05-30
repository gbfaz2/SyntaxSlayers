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
    ), Arma(bando == Bando::CRISTIANO ? "Punal" : "Daga oculta", TipoAtaque::CUERPO_A_CUERPO) //inicializa a una arma u a otra segun bando
{
    // vacío, PiezaTeleporte (y Pieza) ya inicializan todo
}

void Infiltrado::copiarStats(Pieza& enemigo) {
    // Copiamos todas las características principales de la pieza objetivo
    vidaMax = enemigo.getVidaMax();
    vida = enemigo.getVidaMax(); // Le restauramos la vida completa del nuevo rol
    fuerza = enemigo.getFuerza();
    velAtaque = enemigo.getVelAtaque();
    recarga = enemigo.getRecarga();
    radioMov = enemigo.getRadioMov();

    // Mensaje de consola para feedback interno
    std::cout << "[Habilidad] Infiltrado ha copiado los stats de " << enemigo.getNombre() << std::endl;
}

void Infiltrado::mostrarStats() const {
    // Llama al mostrarStats de Pieza 
    Pieza::mostrarStats();

    //informa del tipo de arma 
    std::cout << "Arma     : " << getNombreArma() << " (Cuerpo a cuerpo letal)" << std::endl;
   
    //informa de su habilidad especial
    std::cout << "Habilidad : Espionaje Tactico (copia stats del enemigo)" << std::endl;
}
