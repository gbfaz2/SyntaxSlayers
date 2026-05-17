#include "Hechicero.h"

Hechicero::Hechicero() {
    //FALSE=todos disponibles
    // Todos los conjuros se inicializan a FALSE, por eso no es necesario meterlos como argumentos, se puede incializar como lista.
    conjurosUsados.fill(false);
    //.fill() es un metofo de std::array para rellenar todos con el mismo valor
}

bool Hechicero::puedeUsarConjuro(Conjuro c) const {
    return !conjurosUsados[static_cast<int>(c)];
    //static_cast<int>(c) convierte el tipo de enum Conjuro a un indice int
    //retorna lo contrario a lo que lee en ese indice (true/false)
    //si conjurosUsados[ej 1]=TRUE (ya se ha usado) -> return FALSE, indicando que el conjuro ya se gasto
 
}



void Hechicero::usarConjuro(Conjuro c) {
    conjurosUsados[static_cast<int>(c)] = true;
    //marca el conjuro TRUE (gastado)
}
