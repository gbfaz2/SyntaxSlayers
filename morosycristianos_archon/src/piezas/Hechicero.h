//HECHICERO: clase independiente que solo gestiona los connjuros

#pragma once
#include <array>

// Los 7 conjuros disponibles, cada uno solo se puede usar una vez
enum class Conjuro {
    RUTAS_SECRETAS,    // teleporta una pieza aliada
    AVITUALLAMIENTO,   // cura una pieza aliada
    CONTROL_FRONTERAS, // cambia casillas oscilantes
    RELEVO_GUARDIA,    // intercambia dos piezas aliadas
    MERCENARIOS,       // invoca tropa temporal
    REFUERZOS,         // resucita una pieza eliminada
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

    //métodos normales:
    // Comprueba si un conjuro está disponible
    bool puedeUsarConjuro(Conjuro c) const;

    // Marca el conjuro como usado
    void usarConjuro(Conjuro c);

    // MÉTODO VIRTUAL PURO (=0): cada líder aplica el conjuro a su manera
    virtual void aplicarConjuro(Conjuro c) = 0;
};


