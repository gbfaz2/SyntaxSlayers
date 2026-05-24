#include "GestorHechizos.h"
#include <iostream>
#include "Miliciano.h"

GestorHechizos::GestorHechizos(Tablero& tablero, Hechicero* hechiceroLocal, Hechicero* hechiceroRival)
    : _tablero(tablero),
    _hechiceroLocal(hechiceroLocal),
    _hechiceroRival(hechiceroRival)
{

}

Hechicero* GestorHechizos::hechiceroActual(BandoPieza bando) const
{
    return (bando == bando_local) ? _hechiceroLocal : _hechiceroRival;
}

bool GestorHechizos::puedeUsarConjuro(BandoPieza bando, Conjuro c) const
{
    Hechicero* h = hechiceroActual(bando);
    if (!h) return false;
    return h->puedeUsarConjuro(c);
}

// AVITUALLAMIENTO: curacion
bool GestorHechizos::avituallamiento(BandoPieza bando, int fila, int col)
{
    if (!puedeUsarConjuro(bando, Conjuro::AVITUALLAMIENTO)) {
        std::cout << "[Hechizos] AVITUALLAMIENTO ya fue usado.\n";
        return false;
    }

    Casilla& cas = _tablero.getCasilla(fila, col);
    if (cas.bando != bando || cas.obj == nullptr) {
        std::cout << "[Hechizos] No hay pieza aliada en esa casilla.\n";
        return false;
    }

    cas.obj->curar(cas.obj->getVidaMax());

    hechiceroActual(bando)->usarConjuro(Conjuro::AVITUALLAMIENTO);
    std::cout << "[Hechizos] " << cas.obj->getNombre() << " curado a vida maxima.\n";
    return true;
}

// RUTAS SECRETAS: teleport
bool GestorHechizos::rutasSecretas(BandoPieza bando,
    int filaOrigen, int colOrigen,
    int filaDest, int colDest)
{
    if (!puedeUsarConjuro(bando, Conjuro::RUTAS_SECRETAS)) {
        std::cout << "[Hechizos] RUTAS SECRETAS ya fue usado.\n";
        return false;
    }

    const Casilla& origen = _tablero.getCasilla(filaOrigen, colOrigen);
    if (origen.bando != bando || origen.obj == nullptr) {
        std::cout << "[Hechizos] No hay pieza aliada en origen.\n";
        return false;
    }

    const Casilla& destino = _tablero.getCasilla(filaDest, colDest);
    if (destino.bando != bando_nada) {
        std::cout << "[Hechizos] La casilla destino esta ocupada.\n";
        return false;
    }

    _tablero.muevePieza(filaOrigen, colOrigen, filaDest, colDest);

    hechiceroActual(bando)->usarConjuro(Conjuro::RUTAS_SECRETAS);
    std::cout << "[Hechizos] RUTAS SECRETAS ejecutado.\n";
    return true;
}

// RELEVO DE GUARDIA: intercambia dos piezas aisladas
bool GestorHechizos::relevoDeguardia(BandoPieza bando,
    int fila1, int col1,
    int fila2, int col2)
{
    if (!puedeUsarConjuro(bando, Conjuro::RELEVO_GUARDIA)) {
        std::cout << "[Hechizos] RELEVO DE GUARDIA ya fue usado.\n";
        return false;
    }

    Casilla& cas1 = _tablero.getCasilla(fila1, col1);
    Casilla& cas2 = _tablero.getCasilla(fila2, col2);

    if (cas1.bando != bando || cas1.obj == nullptr) {
        std::cout << "[Hechizos] No hay pieza aliada en la primera casilla.\n";
        return false;
    }
    if (cas2.bando != bando || cas2.obj == nullptr) {
        std::cout << "[Hechizos] No hay pieza aliada en la segunda casilla.\n";
        return false;
    }

    std::swap(cas1.pieza, cas2.pieza); // Intercambia el contenido de las dos casillas en una sola linea y luego se actualiza la posicion interna de cada pieza con setPosicion
    std::swap(cas1.obj, cas2.obj);

    cas1.obj->setPosicion(fila1, col1);
    cas2.obj->setPosicion(fila2, col2);

    hechiceroActual(bando)->usarConjuro(Conjuro::RELEVO_GUARDIA);
    std::cout << "[Hechizos] RELEVO DE GUARDIA ejecutado.\n";
    return true;
}

// ASEDIO: bloquear una pieza enemiga
bool GestorHechizos::asedio(BandoPieza bando, int fila, int col)
{
    if (!puedeUsarConjuro(bando, Conjuro::ASEDIO)) {
        std::cout << "[Hechizos] ASEDIO ya fue usado.\n";
        return false;
    }

    BandoPieza bandoEnemigo = (bando == bando_local) ? bando_rival : bando_local;
    Casilla& cas = _tablero.getCasilla(fila, col);

    if (cas.bando != bandoEnemigo || cas.obj == nullptr) {
        std::cout << "[Hechizos] No hay pieza enemiga en esa casilla.\n";
        return false;
    }

    _piezaBloqueada = cas.obj;

    hechiceroActual(bando)->usarConjuro(Conjuro::ASEDIO);
    std::cout << "[Hechizos] " << cas.obj->getNombre() << " bloqueado.\n";
    return true;
}

bool GestorHechizos::estaBloqueada(int fila, int col) const
{
    if (!_piezaBloqueada) return false;
    return (_piezaBloqueada->getFila() == fila &&
        _piezaBloqueada->getColumna() == col);
}

void GestorHechizos::liberarAsedio()
{
    if (_piezaBloqueada) {
        std::cout << "[Hechizos] Asedio levantado sobre "
            << _piezaBloqueada->getNombre() << ".\n";
        _piezaBloqueada = nullptr;
    }
}

// REFUERZOS: resucita una pieza eliminada
bool GestorHechizos::refuerzos(BandoPieza bando, int filaRey, int colRey)
{
    if (!puedeUsarConjuro(bando, Conjuro::REFUERZOS)) {
        std::cout << "[Hechizos] REFUERZOS ya fue usado.\n";
        return false;
    }

    std::vector<Pieza*>& eliminadas = (bando == bando_local)
        ? _eliminadasLocal
        : _eliminadasRival;

    if (eliminadas.empty()) {
        std::cout << "[Hechizos] No hay piezas eliminadas que resucitar.\n";
        return false;
    }

    Pieza* resucitada = eliminadas.back();
    eliminadas.pop_back();

    // Buscamos casilla libre alrededor del Rey/Emir
    int d[8][2] = { {-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1} };

    for (auto& desplazamiento : d) {
        int f = filaRey + desplazamiento[0];
        int c = colRey + desplazamiento[1];

        if (f < 0 || f >= Tablero::N || c < 0 || c >= Tablero::N) continue;

        Casilla& cas = _tablero.getCasilla(f, c);
        if (cas.bando == bando_nada) {
            resucitada->setPosicion(f, c);
            cas.obj = resucitada;
            cas.bando = bando;
            std::cout << "[Hechizos] " << resucitada->getNombre()
                << " resucitado en (" << f << "," << c << ").\n";
            hechiceroActual(bando)->usarConjuro(Conjuro::REFUERZOS);
            return true;
        }
    }

    // Si no habia hueco, devolvemos la pieza a la lista
    eliminadas.push_back(resucitada);
    std::cout << "[Hechizos] No hay casilla libre junto al Rey/Emir.\n";
    return false;
}

void GestorHechizos::registrarEliminada(Pieza* pieza, BandoPieza bando)
{
    if (bando == bando_local)
        _eliminadasLocal.push_back(pieza);
    else
        _eliminadasRival.push_back(pieza);
}

// MERCENARIOS: se invoca una tropa temporal (un Miliciano ya que es la pieza más básica)
bool GestorHechizos::mercenarios(BandoPieza bando, int fila, int col)
{
    if (!puedeUsarConjuro(bando, Conjuro::MERCENARIOS)) {
        std::cout << "[Hechizos] MERCENARIOS ya fue usado.\n";
        return false;
    }

    Casilla& cas = _tablero.getCasilla(fila, col);
    if (cas.bando != bando_nada) {
        std::cout << "[Hechizos] La casilla destino esta ocupada.\n";
        return false;
    }

    Pieza* mercenario = new Miliciano(bando == bando_local ? Bando::CRISTIANO : Bando::ANDALUSI);
    mercenario->setPosicion(fila, col);

    cas.obj = mercenario;
    cas.bando = bando;
    cas.pieza = pieza_cubo_p;

    hechiceroActual(bando)->usarConjuro(Conjuro::MERCENARIOS);
    std::cout << "[Hechizos] Mercenario invocado en (" << fila << "," << col << ").\n";
    return true;
}

// CONTROL DE FRONTERAS: las casillas oscilan entre colores, dando ventaja de terreno
bool GestorHechizos::controlFronteras(BandoPieza bando)
{
    if (!puedeUsarConjuro(bando, Conjuro::CONTROL_FRONTERAS)) {
        std::cout << "[Hechizos] CONTROL DE FRONTERAS ya fue usado.\n";
        return false;
    }

    // María: aquí va _tablero.invertirCiclo() cuando lo implementes

    hechiceroActual(bando)->usarConjuro(Conjuro::CONTROL_FRONTERAS);
    std::cout << "[Hechizos] CONTROL DE FRONTERAS ejecutado.\n";
    return true;
}