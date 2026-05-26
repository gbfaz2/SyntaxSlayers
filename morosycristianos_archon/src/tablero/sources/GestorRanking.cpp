#include "GestorRanking.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

const char* GestorRanking::FICHERO = "ranking.txt";

int GestorRanking::calcularPuntuacion(int piezasEliminadas, int turnos)
{
    return piezasEliminadas * 100 + (1000 - turnos);
}

std::vector<EntradaRanking> GestorRanking::cargar()
{
    std::cout << "[GestorRanking] Cargando desde: " << FICHERO << std::endl;
    std::vector<EntradaRanking> ranking;
    std::ifstream f(FICHERO);
    std::cout << "[GestorRanking] Fichero abierto: " << f.is_open() << std::endl;
    if (!f.is_open()) return ranking;

    std::string linea;
    while (std::getline(f, linea)) {
        if (linea.empty()) continue;
        std::istringstream ss(linea);
        EntradaRanking e;
        // Leemos ganador y puntuacion primero
        ss >> e.ganador >> e.puntuacion >> e.turnos >> e.piezasEliminadas;
        // El resto es la batalla (puede tener espacios)
        std::getline(ss, e.batalla);
        if (!e.batalla.empty() && e.batalla[0] == ' ')
            e.batalla = e.batalla.substr(1); // quita espacio inicial

        std::cout << "[DEBUG] Leido: " << e.ganador << " puntos=" << e.puntuacion << std::endl;
        ranking.push_back(e);
    }

    // Ordenar de mayor a menor puntuacion al cargar
    std::sort(ranking.begin(), ranking.end(),
        [](const EntradaRanking& a, const EntradaRanking& b) {
            return a.puntuacion > b.puntuacion;
        });

    return ranking;
}

void GestorRanking::guardar(const std::string& ganador, int turnos, const std::string& batalla, int piezasEliminadas)
{
    std::cout << "[GestorRanking] Guardando en: " << FICHERO << std::endl;

    // Cargo el ranking actual
    std::vector<EntradaRanking> ranking = cargar();

    // Añado la nueva entrada
    EntradaRanking nueva;
    nueva.ganador = ganador;
    nueva.puntuacion = calcularPuntuacion(piezasEliminadas, turnos);
    nueva.batalla = batalla;
    nueva.turnos = turnos;
    nueva.piezasEliminadas = piezasEliminadas;
    ranking.push_back(nueva);

    // Ordenamos de mayor a menor puntuacion
    std::sort(ranking.begin(), ranking.end(),
        [](const EntradaRanking& a, const EntradaRanking& b) {
            return a.puntuacion > b.puntuacion;
        });

    // Nos quedamos SOLO con las 10 mejores
    if (ranking.size() > MAX_ENTRADAS)
        ranking.resize(MAX_ENTRADAS);

    // Guardamos en fichero
    std::ofstream out(FICHERO);
    for (const auto& e : ranking)
        out << e.ganador << " " << e.puntuacion << " "
        << e.turnos << " " << e.piezasEliminadas << " "
        << e.batalla << "\n";

    std::cout << "[GestorRanking] Ranking guardado." << std::endl;
}