//HECHICERO: clase independiente que solo gestiona los connjuros

#pragma once
#include <array>

// Los 4 conjuros disponibles, cada uno solo se puede usar una vez
enum class Conjuro {
    RUTAS_SECRETAS,    // teleporta una pieza aliada
    AVITUALLAMIENTO,   // cura una pieza aliada
    RELEVO_GUARDIA,    // intercambia dos piezas aliadas
    ASEDIO             // bloquea una pieza enemiga
};

class Hechicero {
protected:

    //lista para los conjuros de tamaño fijo, siempre son 7
    std::array<bool, 7> conjurosUsados;  // false = disponible
    //TRUE usado una vez, no esta disponible

public:
    //constructor, no recibe parámetros porq todos los que tengan hechizos van a inicializarse con todos FALSE
    Hechicero();

    // Comprueba si un conjuro está disponible
    bool puedeUsarConjuro(Conjuro c) const;

    // Marca el conjuro como usado
    void usarConjuro(Conjuro c);
};


