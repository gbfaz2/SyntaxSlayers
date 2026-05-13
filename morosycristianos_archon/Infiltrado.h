#pragma once
#include "PiezaTeleporte.h"

//herencia de piezateleporte y además capacidad de copiar los stats
class Infiltrado :
    public PiezaTeleporte
{
public:
    // Constructor que solo recibe el bando para poder decidir el nombre de la pieza
    //Bando cristiano: infiltrado, bando musulman: asesino de élite
    Infiltrado(Bando bando);

    // Habilidad especial: copia las stats de combate del enemigo
    // Se llama al inicio del combate en la arena
    void copiarStats(Pieza& enemigo);

    // Sobreescribe mostrarStats para identificar la pieza con sus propios stats
    void mostrarStats() const override;
 
};

