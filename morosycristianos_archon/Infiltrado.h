#pragma once
#include "PiezaTeleporte.h"


// INFILTRADO(Cristiana) / ASESINO DE ÉLITE(Musulmana)
//herencia de piezaTeleporte
//HABILIDAD ESPECIAL:capacidad de copiar los stats

class Infiltrado :
    public PiezaTeleporte
{
public:
    // Constructor que solo recibe el bando para poder decidir el nombre de la pieza
    //Bando cristiano: infiltrado, bando musulman: asesino de élite
    Infiltrado(Bando bando);

    //HABILIDAD ESPECIAL: copia las stats de combate del enemigo
    // Se llama al inicio del combate en la arena
    void copiarStats(Pieza& enemigo);

    //Herencia de PiezaTeleporte
    // Sobreescribe mostrarStats para identificar la pieza con sus propios stats
    void mostrarStats() const override;
 
};

