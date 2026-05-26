#pragma once
#include <string>
#include <vector>

struct EntradaRanking {
    std::string ganador;
    int puntuacion;
    std::string batalla;
    int turnos;
    int piezasEliminadas;
};

class GestorRanking
{
    static const int MAX_ENTRADAS = 10;
    static const char* FICHERO;

public:
    // Carga el ranking desde fichero
    static std::vector<EntradaRanking> cargar();

    // Añade una nueva entrada y guarda el ranking ordenado
    static void guardar(const std::string& ganador, int turnos,
        const std::string& batalla, int piezasEliminadas);

    // Calcula la puntuacion
    static int calcularPuntuacion(int piezasEliminadas, int turnos);
};