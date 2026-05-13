#include "Almogávar.h"
#include <iostream>

Almogávar::Almogávar(Bando bando)
    : PiezaVoladora(
        bando == Bando::CRISTIANO ? "Almogavar" : "Arquero a Caballo", //nombre pieza
        // Bando cristiano: Almogavar(jabalinas incendiarias), bando andalusí: Arquero a Caballo" (flechas)
        bando,
        60,   // vidaMax   — más resistente que la CaballeríaLigera
        85,   // fuerza    — daño muy alto, proyectil letal a media distancia
        50,   // velAtaque — velocidad media, más lento que la CaballeríaLigera
        70,   // recarga   — cooldown alto, necesita tiempo entre ataques
        4     // radioMov  — buen alcance volador
    ), ArmaDistancia(bando == Bando::CRISTIANO ? "Jabalinas incendiarias" : "Flechas") //segun bando, elige un arma u otra
{
    // vacío, PiezaVoladora (y Pieza) ya inicializan todo
}



//HABILIDAD ESPECIAL: Ataque de área que envuelve al Almogávar en llamas,
// quemando enemigos cercanos y bloqueando el daño recibido unos segundos.
void Almogávar::cargarIncendiaria() {

    // La lógica real (área, bloqueo de daño) se implementará en la arena con Sofía.
    //por el momento solo imprime por pantalla 
    std::cout << "[" << nombre << "] Carga Incendiaria activada!" << std::endl;
    std::cout << "  Ataque de area: quema enemigos cercanos" << std::endl;
    std::cout << "  Daño bloqueado durante unos segundos" << std::endl;
}

void Almogávar::mostrarStats() const {
    // Llama al mostrarStats de Pieza (imprime nombre, vida, fuerza, etc.)
    Pieza::mostrarStats();

    //informa del arma utilizada a distancia
    std::cout << "Arma     : " << getNombreArma() << " (A distancia)" << std::endl;
  
    //informa de su habilidad especial
    std::cout << "Habilidad : Carga Incendiaria (ataque de area)" << std::endl;
}
