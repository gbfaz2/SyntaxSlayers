#pragma once
#include "Tablero.h"
#include "Hechicero.h"
# include <vector>

class GestorHechizos 
{
	Tablero& _tablero;
    Hechicero* _hechiceroLocal;
    Hechicero* _hechiceroRival;

    // Pieza bloqueada por Asedio
    Pieza* _piezaBloqueada{ nullptr };

    // Devuelve el hechicero del bando que toca
    Hechicero* hechiceroActual(BandoPieza bando) const;

public:
public:
    // Constructor
    GestorHechizos(Tablero& tablero, Hechicero* hechiceroLocal, Hechicero* hechiceroRival);

    // Comprueba si el bando puede usar ese conjuro (no lo ha gastado ya)
    bool puedeUsarConjuro(BandoPieza bando, Conjuro c) const;

    // Los 4 conjuros
    bool rutasSecretas(BandoPieza bando, int filaOrigen, int colOrigen, int filaDest, int colDest);
    bool avituallamiento(BandoPieza bando, int fila, int col);
    bool relevoDeguardia(BandoPieza bando, int fila1, int col1, int fila2, int col2);
    bool asedio(BandoPieza bando, int fila, int col);

    // Para el asedio
    bool estaBloqueada(int fila, int col) const;
    void liberarAsedio();
};