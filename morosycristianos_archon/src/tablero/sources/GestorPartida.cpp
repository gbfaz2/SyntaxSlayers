#include "GestorPartida.h"
#include <fstream>
#include <iostream>

static const char* FICHERO = "partida.txt";

bool GestorPartida::hayPartidaGuardada()
{
	std::ifstream f(FICHERO);
	return f.good();
}

bool GestorPartida::guardar(const Tablero& tablero, const GestorTurnos& turnos, const ConfigPartida& cfg)
{
    std::ofstream f(FICHERO);
    if (!f.is_open()) return false;

    // Configuracion
    f << "modo " << (int)cfg.modo << "\n";
    f << "batalla " << (int)cfg.batalla << "\n";
    f << "turno " << (int)turnos.getBandoActual() << "\n";
    f << "tiempo " << turnos.getTiempoRestante() << "\n";
    f << "numturno " << turnos.getNumeroTurno() << "\n";
    f << "j1 " << cfg.nombre_j1 << "\n";
    f << "j2 " << cfg.nombre_j2 << "\n";

    // Tablero: fila, col, tipoPieza, bandoPieza, vida
    for (int fila = 0; fila < Tablero::N; fila++) {
        for (int col = 0; col < Tablero::N; col++) {
            const Casilla& c = tablero.getCasilla(fila, col);
            if (c.pieza != pieza_nada) {
                int vida = (c.obj != nullptr) ? c.obj->getVida() : 0;
                f << fila << " " << col << " "
                    << (int)c.pieza << " "
                    << (int)c.bando << " "
                    << vida << "\n";
            }
        }
    }

    std::cout << "[GestorPartida] Partida guardada correctamente." << std::endl;
    return true;
}

bool GestorPartida::cargar(Tablero& tablero, GestorTurnos& turnos, ConfigPartida& cfg)
{
    std::ifstream f(FICHERO);
    if (!f.is_open()) return false;

    // Leo configiguracion
    std::string clave;
    int modo, batalla, turno, numturno;
    float tiempo;

    f >> clave >> modo;
    f >> clave >> batalla;
    f >> clave >> turno;
    f >> clave >> tiempo;
    f >> clave >> numturno;

    cfg.modo = (ModoJuego)modo;
    cfg.batalla = (Batalla)batalla;
    turnos.setBandoInicial((BandoPieza)turno);

    std::string j1label, j2label;
    f >> j1label >> cfg.nombre_j1;
    f >> j2label >> cfg.nombre_j2;

    tablero.limpiarPiezas();

    // Leo piezas
    int fila, col, tipoPieza, bandoPieza, vida;
    while (f >> fila >> col >> tipoPieza >> bandoPieza >> vida) 
    {
        Casilla& c = tablero.getCasilla(fila, col);
        c.pieza = (TipoPieza)tipoPieza;
        c.bando = (BandoPieza)bandoPieza;
        c.obj = tablero.crearPieza((TipoPieza)tipoPieza, (BandoPieza)bandoPieza);

        if (c.obj) 
        {
            c.obj->setVida(vida);
            c.obj->setPosicion(fila, col);
        }
    }

    // Asignar ID de animación después de cargar
    int contadores[(int)TipoPieza::pieza_basica1 + 1] = { 0 };
    for (int fila = 0; fila < Tablero::N; fila++) 
    {
        for (int col = 0; col < Tablero::N; col++) 
        {
            Casilla& c = tablero.getCasilla(fila, col);
            if (c.obj != nullptr) {
                int tipo = (int)c.pieza;
                c.obj->setIdAnimacion(contadores[tipo]);
                contadores[tipo]++;
            }
        }
    }

    std::cout << "[GestorPartida] Partida cargada correctamente." << std::endl;
    return true;
}