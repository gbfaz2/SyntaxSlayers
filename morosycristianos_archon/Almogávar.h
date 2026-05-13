#pragma once
#include "PiezaVoladora.h"

// ALMOGÁVAR (Cristiano) / ARQUERO A CABALLO (Musulman)
// Pieza voladora de proyectil. Fuerte y resistente, pero más lenta que la CaballeríaLigera.
// HABILIDAD ESPECIAL: Fuego Griego / Carga Incendiaria (ataque de área).

class Almogávar :
    public PiezaVoladora
{
public:
    // Constructor que solo recibe el bando, el resto de stats están fijos en el .cpp, con el bando se decide el nombre de la pieza
    //Bando cristiano: almogávar, bando musulman: arquero a caballo
    Almogávar(Bando bando);

    //HABILIDAD ESPECIAL
    // De momento solo imprime por consola, la lógica real va en la arena
    void cargarIncendiaria();


    //herencia de piezaVoladora
    // Sobreescribe mostrarStats para identificar la pieza claramente
    void mostrarStats() const override;
};

