#include <iostream>
#include "Miliciano.h"
#include "Ballestero.h"
#include "Rey.h"

//main de prueba para las clases

int main() {
    std::cout << "=== PRUEBA DE ARCHON: BATALLAS DE AL-ANDALUS ===" << std::endl;
    std::cout << std::endl;

    // 1. Probamos un Miliciano del bando Cristiano
    Miliciano soldadoCristiano(Bando::CRISTIANO);
    soldadoCristiano.mostrarStats();

    std::cout << "---------------------------------------" << std::endl;

    // 2. Probamos un Arquero del bando Andalusí (usa la clase Ballestero)
    Ballestero arqueroAndalusi(Bando::ANDALUSI);
    arqueroAndalusi.mostrarStats();

    std::cout << "---------------------------------------" << std::endl;

    // 3. Probamos al Rey (¡El de la triple herencia!)
    Rey miRey(Bando::CRISTIANO);
    miRey.mostrarStats();

    std::cout << std::endl;
    std::cout << "Todo compila perfectamente. Pulse Enter para salir..." << std::endl;

    // Pausamos la consola para que te dé tiempo a leerlo
    std::cin.get();

    return 0;
}