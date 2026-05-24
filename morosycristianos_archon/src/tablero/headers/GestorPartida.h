#pragma once
#include "Tablero.h"
#include "GestorTurnos.h"
#include "estadojuego.h"

// Gestiona el guardado y carga de partidas en fichero de texto

class GestorPartida
{
public:
    // Guarda el estado actual en partida.txt, y devuelve true si se guardó correctamente
    static bool guardar(const Tablero& tablero, const GestorTurnos& turnos, const ConfigPartida& cfg);

    // Carga partida.txt y restaura el estado, devuelve true si se carga completamente
    static bool cargar(Tablero& tablero, GestorTurnos& turnos, ConfigPartida& cfg);

    // Comprueba si existe partida guardada
    static bool hayPartidaGuardada();
};