#include "GestorInput.h"
#include "freeglut.h"
#include "menu.h"
#include "tablerogl.h"
#include "Coordinador.h" // INCLUDE COMPLETO SOLO EN EL CPP
#include "ETSIDI.h"

// =============================================================
// MENU
// =============================================================

void GestorInput::teclaMenu(unsigned char key, EstadoJuego& estado,
    PantallaIntro& intro, MenuPrincipal& menu, PantallaDestino& destino)
{
    switch (estado) {

    case EstadoJuego::INTRO:
        intro.saltar(); // CUALQUIER TECLA SALTA LA INTRO
        break;

    case EstadoJuego::MENU:
        if (key == 27) { // ESC: RETROCEDE UN PASO
            if (menu.m_paso > 0) { menu.m_paso--; menu.m_seleccion = 0; menu.m_nombreActual = ""; }
            else { menu.m_siguiente = EstadoJuego::INTRO; menu.m_terminado = true; }
            return;
        }
        if (menu.m_paso == 1 || menu.m_paso == 3)
            menu.teclaTexto(key); // PASOS DE TEXTO: CAPTURA CARACTERES
        else
            if (key == 13) menu.confirmar(); // RESTO DE PASOS: SOLO ENTER CONFIRMA
        break;

    case EstadoJuego::DESTINO:
        destino.avanzar(); // CUALQUIER TECLA AVANZA LA PANTALLA
        break;

    default:
        break;
    }
} 

void GestorInput::teclaEspecialMenu(int key, EstadoJuego& estado, MenuPrincipal& menu)
{
    if (estado != EstadoJuego::MENU) return; // SOLO EN MENU
    if (menu.maxOpciones() == 0) return; 

    if (key == GLUT_KEY_UP)
        menu.m_seleccion = (menu.m_seleccion - 1 + menu.maxOpciones()) % menu.maxOpciones(); // SUBE SELECCION
    if (key == GLUT_KEY_DOWN)
        menu.m_seleccion = (menu.m_seleccion + 1) % menu.maxOpciones(); // BAJA SELECCION
    if (key == GLUT_KEY_LEFT && menu.m_paso == 2) menu.m_seleccion = 0; // SELECCIONA IZQUIERDA
    if (key == GLUT_KEY_RIGHT && menu.m_paso == 2) menu.m_seleccion = 1; // SELECCIONA DERECHA
}

void GestorInput::ratonMenu(int boton, int state, int x, int y,
    EstadoJuego& estado, PantallaIntro& intro,
    MenuPrincipal& menu, PantallaDestino& destino)
{
    if (state != GLUT_DOWN) return; // SOLO AL PULSAR

    switch (estado) {
    case EstadoJuego::INTRO:
        intro.saltar(); // CLIC SALTA LA INTRO
        break;
    case EstadoJuego::DESTINO:
        destino.avanzar(); // CLIC AVANZA LA PANTALLA
        break;
    case EstadoJuego::MENU:
        if (boton == GLUT_LEFT_BUTTON) {
            ratonMovidoMenu(x, y, estado, menu); // ACTUALIZA SELECCION
            menu.confirmar();                    // CONFIRMA SELECCION
        }
        break;
    default:
        break;
    }
}

void GestorInput::ratonMovidoMenu(int mx, int my, EstadoJuego& estado, MenuPrincipal& menu)
{
    if (estado != EstadoJuego::MENU) return; // SOLO EN MENU

    int gy = _alto - my; // INVIERTE Y (GLUT DESDE ARRIBA, OPENGL DESDE ABAJO)

    auto enCaja = [&](float x, float y, float aw, float ah) {
        return mx >= x && mx <= x + aw && gy >= y && gy <= y + ah;
        };

    if (menu.m_paso == 0) { // PASO 0: SELECCION MODO
        float aw = 280, ah = 44, sep = 14;
        float sx = _ancho / 2.0f - aw / 2.0f, sy = _alto / 2.0f + 60;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) menu.m_seleccion = i;
    }
    else if (menu.m_paso == 2) { // PASO 1: SELECCION BANDO
        float aw = 240, ah = 55, sep = 30, sy = _alto / 2.0f + 10;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, sy, aw, ah))      menu.m_seleccion = 1;
    }
    else if (menu.m_paso == 4) { // PASO 2: SELECCION BATALLA
        float aw = 340, ah = 48, sep = 12;
        float sx = _ancho / 2.0f - aw / 2.0f, sy = _alto / 2.0f + 80;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) menu.m_seleccion = i;
    }
    else if (menu.m_paso == 5) { // PASO 3: CONFIRMACION
        float aw = 180, ah = 44, sep = 30, cy = _alto / 2.0f - 20;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, cy, aw, ah))      menu.m_seleccion = 1;
    }
}

// =============================================================
// TABLERO
// =============================================================

bool GestorInput::ejecutarHechizo(BandoPieza bando, int fila, int col)
{
    bool ejecutado = false;                            // CONTROLA SI SE EJECUTÓ

    switch (_tablerogl->_conjuroActivo) {

    case Conjuro::AVITUALLAMIENTO:                     // CURA PIEZA ALIADA EN CASILLA ACTUAL
        ejecutado = _coordinador->pGestorHechizos->avituallamiento(bando, fila, col);
        break;

    case Conjuro::RUTAS_SECRETAS:                      // TELEPORT: PRIMER CONFIRM=ORIGEN, SEGUNDO=DESTINO
        if (!_tablerogl->_esperandoDestino) {
            _tablerogl->_hechizoFilaOrigen = fila;     // GUARDA FILA ORIGEN
            _tablerogl->_hechizoColOrigen = col;      // GUARDA COL ORIGEN
            _tablerogl->_esperandoDestino = true;     // ACTIVA ESPERA DESTINO
            _tablerogl->_mensajeInvalido = "";         // LIMPIA MENSAJE INVALIDO
            _tablerogl->_tiempoMensajeInvalido = 0.0f; // RESETEA TEMPORIZADOR
            std::cout << "[Hechizos] Origen guardado. Mueve cursor al destino y confirma.\n";
            return false;                              // ESPERA SEGUNDA CONFIRMACION
        }
        else {
            ejecutado = _coordinador->pGestorHechizos->rutasSecretas(bando,
                _tablerogl->_hechizoFilaOrigen, _tablerogl->_hechizoColOrigen, fila, col);
            _tablerogl->_esperandoDestino = false;     // RESETEA ESPERA
        }
        break;

    case Conjuro::RELEVO_GUARDIA:                      // INTERCAMBIA 2 PIEZAS: PRIMER=PIEZA1, SEGUNDO=PIEZA2
        if (!_tablerogl->_esperandoDestino) {
            _tablerogl->_hechizoFilaOrigen = fila;     // GUARDA FILA PRIMERA PIEZA
            _tablerogl->_hechizoColOrigen = col;      // GUARDA COL PRIMERA PIEZA
            _tablerogl->_esperandoDestino = true;     // ACTIVA ESPERA SEGUNDA PIEZA
            _tablerogl->_mensajeInvalido = "";         // LIMPIA MENSAJE INVALIDO
            _tablerogl->_tiempoMensajeInvalido = 0.0f; // RESETEA TEMPORIZADOR
            std::cout << "[Hechizos] Primera pieza guardada. Mueve cursor a la segunda y confirma.\n";
            return false;                              // ESPERA SEGUNDA CONFIRMACION
        }
        else {
            ejecutado = _coordinador->pGestorHechizos->relevoDeguardia(bando,
                _tablerogl->_hechizoFilaOrigen, _tablerogl->_hechizoColOrigen, fila, col);
            _tablerogl->_esperandoDestino = false;     // RESETEA ESPERA
        }
        break;

    case Conjuro::ASEDIO:                              // BLOQUEA PIEZA ENEMIGA EN CASILLA ACTUAL
        ejecutado = _coordinador->pGestorHechizos->asedio(bando, fila, col);
        break;

    default: break;                                    // HECHIZOS NO IMPLEMENTADOS
    }

    if (ejecutado) {
        int idx = (bando == bando_local) ? 0 : 1;     // ÍNDICE CURSOR SEGÚN BANDO
        _tablerogl->_modoHechizo = false;              // DESACTIVA MODO HECHIZO
        _tablerogl->_conjuroElegido = false;           // RESETEA CONJURO ELEGIDO
        _tablerogl->_esperandoDestino = false;         // RESETEA ESPERA
        _tablerogl->piezaSeleccionada = false;         // DESELECCIONA PIEZA
        _tablerogl->fromFila = _tablerogl->fromCol = -1; // RESETEA ORIGEN
        _tablerogl->_mensajeInvalido = "";             // LIMPIA MENSAJE INVALIDO
        _tablerogl->_tiempoMensajeInvalido = 0.0f;    // RESETEA TEMPORIZADOR
        Pieza* lider = _coordinador->pTablero->buscarPieza(pieza_esfera, bando);
        if (lider) {
            _tablerogl->Filacursor[idx] = lider->getFila();    // CURSOR VUELVE AL LIDER
            _tablerogl->Colcursor[idx] = lider->getColumna(); // CURSOR VUELVE AL LIDER
        }
        _tablerogl->gestorTurnos.terminarTurno();      // PASA TURNO
    }

    return ejecutado;                                  // DEVUELVE SI SE EJECUTÓ
}

void GestorInput::teclaTablero(unsigned char key, EstadoJuego& estado)
{
    if (!_tablerogl) return;                          // TABLEROGL NO ASIGNADO
    if (_tablerogl->victoria_ != bando_nada) return;  // PARTIDA TERMINADA

    // Q: CIERRA EL JUEGO
    if (key == 'q' || key == 'Q') { exit(0); return; }

    // ============================================================
    // H: ACTIVA MODO HECHIZO P1 (SOLO SI EL REY ESTÁ SELECCIONADO)
    // ============================================================
    if (key == 'h' || key == 'H') {
        if (_tablerogl->piezaSeleccionada) {               // HAY PIEZA SELECCIONADA
            const Casilla& cas = _coordinador->pTablero->getCasilla(
                _tablerogl->fromFila, _tablerogl->fromCol);
            if (cas.pieza == pieza_esfera && cas.bando == bando_local) { // ES EL REY LOCAL
                std::cout << "[Hechizos] Modo hechizo P1 activo. Elige 1-4.\n 1. Avituallamiento: Cura una pieza tuya a vida maxima";
                std::cout << "\n 2. Rutas Secretas: Teleporta una pieza tuya a cualquier casilla vacia del tablero.";
                std::cout << "\n 3. Relevo de Guardia: Intercambia la posicion de dos piezas tuyas.";
                std::cout << "\n 4. Asedio: Bloquea una pieza enemiga en su casilla para siempre, no puede moverse.";

                _tablerogl->_modoHechizo = true;       // ACTIVA MODO HECHIZO
                _tablerogl->_bandoHechizo = bando_local; // MARCA BANDO P1
                _tablerogl->_conjuroElegido = false;      // RESETEA CONJURO ELEGIDO
                _tablerogl->_esperandoDestino = false;     // RESETEA ESPERA
                _tablerogl->piezaSeleccionada = false;     // DESELECCIONA EL REY
                _tablerogl->fromFila = _tablerogl->fromCol = -1; // RESETEA ORIGEN
                _tablerogl->_mensajeInvalido = "";         // LIMPIA MENSAJE INVALIDO
                _tablerogl->_tiempoMensajeInvalido = 0.0f; // RESETEA TEMPORIZADOR
            }
            else {
                std::cout << "[Hechizos] Selecciona al Rey primero.\n"; // AVISO
            }
        }
        else {
            std::cout << "[Hechizos] Selecciona al Rey primero.\n";     // AVISO SIN SELECCION
        }
        return;
    }

    // ============================================================
    // J: ACTIVA MODO HECHIZO P2 (SOLO SI EL EMIR ESTÁ SELECCIONADO)
    // ============================================================
    if (key == 'j' || key == 'J') {
        if (_tablerogl->piezaSeleccionada) {               // HAY PIEZA SELECCIONADA
            const Casilla& cas = _coordinador->pTablero->getCasilla(
                _tablerogl->fromFila, _tablerogl->fromCol);
            if (cas.pieza == pieza_esfera && cas.bando == bando_rival) { // ES EL EMIR RIVAL
                std::cout << "[Hechizos] Modo hechizo P2 activo. Elige 1-4.\n 1. Avituallamiento: Cura una pieza tuya a vida maxima";
                std::cout << "\n 2. Rutas Secretas: Teleporta una pieza tuya a cualquier casilla vacia del tablero.";
                std::cout << "\n 3. Relevo de Guardia: Intercambia la posicion de dos piezas tuyas.";
                std::cout << "\n 4. Asedio: Bloquea una pieza enemiga en su casilla para siempre, no puede moverse.";
                _tablerogl->_modoHechizo = true;       // ACTIVA MODO HECHIZO
                _tablerogl->_bandoHechizo = bando_rival; // MARCA BANDO P2
                _tablerogl->_conjuroElegido = false;      // RESETEA CONJURO ELEGIDO
                _tablerogl->_esperandoDestino = false;     // RESETEA ESPERA
                _tablerogl->piezaSeleccionada = false;     // DESELECCIONA EL EMIR
                _tablerogl->fromFila = _tablerogl->fromCol = -1; // RESETEA ORIGEN
                _tablerogl->_mensajeInvalido = "";         // LIMPIA MENSAJE INVALIDO
                _tablerogl->_tiempoMensajeInvalido = 0.0f; // RESETEA TEMPORIZADOR
            }
            else {
                std::cout << "[Hechizos] Selecciona al Emir primero.\n"; // AVISO
            }
        }
        else {
            std::cout << "[Hechizos] Selecciona al Emir primero.\n";     // AVISO SIN SELECCION
        }
        return;
    }

    // ============================================================
    // NÚMERO: ELIGE HECHIZO (solo en modo hechizo, conjuro no elegido aún)
    // ============================================================
    if (_tablerogl->_modoHechizo && !_tablerogl->_conjuroElegido) {
        switch (key) {
        case '1': _tablerogl->_conjuroActivo = Conjuro::AVITUALLAMIENTO;
            _tablerogl->_conjuroElegido = true;
            std::cout << "[Hechizos] AVITUALLAMIENTO: mueve cursor a tu pieza y confirma.\n";   return;
        case '2': _tablerogl->_conjuroActivo = Conjuro::RUTAS_SECRETAS;
            _tablerogl->_conjuroElegido = true;
            std::cout << "[Hechizos] RUTAS SECRETAS: mueve cursor al origen y confirma.\n";     return;
        case '3': _tablerogl->_conjuroActivo = Conjuro::RELEVO_GUARDIA;
            _tablerogl->_conjuroElegido = true;
            std::cout << "[Hechizos] RELEVO GUARDIA: mueve cursor a pieza 1 y confirma.\n";     return;
        case '4': _tablerogl->_conjuroActivo = Conjuro::ASEDIO;
            _tablerogl->_conjuroElegido = true;
            std::cout << "[Hechizos] ASEDIO: mueve cursor al enemigo y confirma.\n";            return;
        }
    }

    // ============================================================
    // MOVIMIENTO CURSOR P1 CON WASD (disponible siempre)
    // ============================================================
    if (_tablerogl->gestorTurnos.getBandoActual() == bando_local ||
        (_tablerogl->_modoHechizo && _tablerogl->_bandoHechizo == bando_local)) {
        int& rL = _tablerogl->Filacursor[0];               // FILA CURSOR P1
        int& cL = _tablerogl->Colcursor[0];                // COL CURSOR P1
        if (key == 'w' || key == 'W') { if (rL > 0)                 rL--; } // CURSOR ARRIBA
        if (key == 's' || key == 'S') { if (rL < _tablerogl->N - 1) rL++; } // CURSOR ABAJO
        if (key == 'a' || key == 'A') { if (cL > 0)                 cL--; } // CURSOR IZQUIERDA
        if (key == 'd' || key == 'D') { if (cL < _tablerogl->N - 1) cL++; } // CURSOR DERECHA
    }

    // SI HAY MODO HECHIZO ACTIVO, SOLO PASAN ESPACIO Y PUNTO
    if (_tablerogl->_modoHechizo) {
        if (key != ' ' && key != '.') return;              // BLOQUEA RESTO DE TECLAS
    }

    // ============================================================
    // ESPACIO: CONFIRMA HECHIZO P1 O SELECCIONA/MUEVE PIEZA P1
    // ============================================================
    if (key == ' ') {
        if (_tablerogl->_modoHechizo && _tablerogl->_bandoHechizo == bando_local) {
            int fila = _tablerogl->Filacursor[0];          // FILA CURSOR P1
            int col = _tablerogl->Colcursor[0];           // COL CURSOR P1
            ejecutarHechizo(bando_local, fila, col);       // EJECUTA HECHIZO P1
            return;                                        // NUNCA LLEGA A trySelectorMove
        }
        _tablerogl->trySelectorMove(bando_local);          // SELECCIONA O MUEVE P1
        return;
    }

    // ============================================================
    // PUNTO: CONFIRMA HECHIZO P2 O SELECCIONA/MUEVE PIEZA P2
    // ============================================================
    if (key == '.') {
        if (_tablerogl->_modoHechizo && _tablerogl->_bandoHechizo == bando_rival) {
            int fila = _tablerogl->Filacursor[1];          // FILA CURSOR P2
            int col = _tablerogl->Colcursor[1];           // COL CURSOR P2
            ejecutarHechizo(bando_rival, fila, col);       // EJECUTA HECHIZO P2
            return;                                        // NUNCA LLEGA A trySelectorMove
        }
        _tablerogl->trySelectorMove(bando_rival);          // SELECCIONA O MUEVE P2
        return;
    }
}

void GestorInput::teclaEspecialTablero(int key)
{
    if (!_tablerogl) return;                           // TABLEROGL NO ASIGNADO
    if (_tablerogl->victoria_ != bando_nada) return;  // PARTIDA TERMINADA

    if (_tablerogl->gestorTurnos.getBandoActual() == bando_rival) {
        int& rR = _tablerogl->Filacursor[1]; // FILA CURSOR RIVAL
        int& cR = _tablerogl->Colcursor[1];  // COLUMNA CURSOR RIVAL
        if (key == GLUT_KEY_UP && rR > 0)                  rR--; // CURSOR ARRIBA
        if (key == GLUT_KEY_DOWN && rR < _tablerogl->N - 1)  rR++; // CURSOR ABAJO
        if (key == GLUT_KEY_LEFT && cR > 0)                  cR--; // CURSOR IZQUIERDA
        if (key == GLUT_KEY_RIGHT && cR < _tablerogl->N - 1)  cR++; // CURSOR DERECHA
    }
}

void GestorInput::ratonTablero(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey)
{
    if (!_tablerogl) return;                           // TABLEROGL NO ASIGNADO
    if (_tablerogl->victoria_ != bando_nada) return;  // PARTIDA TERMINADA

    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);      // LEE MATRIZ MODELVIEW
    glGetDoublev(GL_PROJECTION_MATRIX, projection);    // LEE MATRIZ PROYECCION
    glGetIntegerv(GL_VIEWPORT, viewport);              // LEE VIEWPORT

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;             // INVIERTE Y
    glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); // LEE PROFUNDIDAD
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ); // PIXEL A MUNDO

    int clickFila, clickCol;
    _tablerogl->world2cell(posX, posY, clickFila, clickCol); // MUNDO A CASILLA

    _tablerogl->xcasilla_sel = clickFila;              // ACTUALIZA CASILLA BAJO CURSOR
    _tablerogl->ycasilla_sel = clickCol;

    if (down) { _tablerogl->controlKey = ctrlKey; _tablerogl->shiftKey = shiftKey; }
    else { _tablerogl->controlKey = _tablerogl->shiftKey = false; }

    if (button == MOUSE_LEFT_BUTTON)   _tablerogl->leftButton = down;  // ACTUALIZA BOTON IZQ
    if (button == MOUSE_RIGHT_BUTTON)  _tablerogl->rightButton = down;  // ACTUALIZA BOTON DER
    if (button == MOUSE_MIDDLE_BUTTON) _tablerogl->midButton = down;  // ACTUALIZA BOTON MED

    if (!down) return;                                 // SOLO PROCESAMOS AL PULSAR

    if (button == MOUSE_RIGHT_BUTTON) {                // CLIC DERECHO: CANCELA SELECCION
        _tablerogl->piezaSeleccionada = false;
        _tablerogl->fromFila = _tablerogl->fromCol = -1;
        return;
    }

    if (button != MOUSE_LEFT_BUTTON) return;           // SOLO CLIC IZQUIERDO A PARTIR DE AQUI

    if (clickFila < 0 || clickFila >= _tablerogl->N ||
        clickCol < 0 || clickCol >= _tablerogl->N) return; // FUERA DEL TABLERO

    // MODO HECHIZO: CLIC IZQUIERDO CONFIRMA EN CASILLA CLICKADA
    if (_tablerogl->_modoHechizo && _tablerogl->_conjuroElegido) {
        BandoPieza bando = _tablerogl->_bandoHechizo;  // BANDO QUE HECHIZA
        int idx = (bando == bando_local) ? 0 : 1;      // ÍNDICE CURSOR
        _tablerogl->Filacursor[idx] = clickFila;       // MUEVE CURSOR A CASILLA CLICKADA
        _tablerogl->Colcursor[idx] = clickCol;        // MUEVE CURSOR A CASILLA CLICKADA
        ejecutarHechizo(bando, clickFila, clickCol);    // EJECUTA HECHIZO
        return;                                        // NO PROCESA MÁS
    }

    const Casilla& clicked = _tablerogl->m_tablero->getCasilla(clickFila, clickCol);

    if (!_tablerogl->piezaSeleccionada) {              // SIN PIEZA SELECCIONADA: SELECCIONA
        if (clicked.pieza != pieza_nada) {
            int idx = (clicked.bando == bando_local) ? 0 : 1;
            _tablerogl->Filacursor[idx] = clickFila;
            _tablerogl->Colcursor[idx] = clickCol;
            _tablerogl->trySelectorMove(clicked.bando);
        }
    }
    else {                                             // CON PIEZA SELECCIONADA: MUEVE
        int idx = (_tablerogl->fromBando == bando_local) ? 0 : 1;
        _tablerogl->Filacursor[idx] = clickFila;
        _tablerogl->Colcursor[idx] = clickCol;
        _tablerogl->trySelectorMove(_tablerogl->fromBando);
    }
}

// =============================================================
// ARENA
// =============================================================

void GestorInput::teclaArena(unsigned char key)
{
    if (!_coordinador) return;// COORDINADOR NO ASIGNADO

    // CONTROLES P1: WASD + F
    if (key == 'w' || key == 'W') _coordinador->_input.p1.delante = true;
    if (key == 's' || key == 'S') _coordinador->_input.p1.atras = true;
    if (key == 'a' || key == 'A') _coordinador->_input.p1.izquierda = true;
    if (key == 'd' || key == 'D') _coordinador->_input.p1.derecha = true;
    if (key == 'f' || key == 'F') _coordinador->_input.p1.atacar = true;

    // CONTROLES P2 SOLO EN JVJ: L ATACA
    if (_coordinador->configuracion.modo == ModoJuego::JVJ)
        if (key == 'l' || key == 'L') _coordinador->_input.p2.atacar = true;

    // ESC SIEMPRE VUELVE AL MENU
    if (key == 27) {
        ETSIDI::stopMusica();
        _coordinador->reiniciarTablero();
        _coordinador->estado = EstadoJuego::MENU;
        return;
    }

    // ENTER VUELVE AL TABLERO SI EL COMBATE HA TERMINADO
    if (key == 13 && _coordinador->_arena.resultado() != ResultadoCombate::EnCurso) {
        bool ganaP1 = (_coordinador->_arena.resultado() == ResultadoCombate::GanaP1);

        if (ganaP1) {
            Pieza* atacante = _coordinador->_pAtacanteCombate;
            Pieza* defensora = _coordinador->_pDefensoraCombate;
            if (atacante && defensora && _coordinador->pTablero) {
                int filaAtacante = atacante->getFila();
                int colAtacante = atacante->getColumna();
                int filaDefensora = defensora->getFila();
                int colDefensora = defensora->getColumna();

                Casilla& cDef = _coordinador->pTablero->getCasilla(filaDefensora, colDefensora);
                delete cDef.obj;                         // ELIMINA PIEZA PERDEDORA
                cDef.obj = nullptr;
                cDef.pieza = pieza_nada;
                cDef.bando = bando_nada;

                _coordinador->pTablero->muevePieza(filaAtacante, colAtacante, filaDefensora, colDefensora);
            }
        }
        else {
            Pieza* atacante = _coordinador->_pAtacanteCombate;
            if (atacante && _coordinador->pTablero) {
                int fila = atacante->getFila();
                int col = atacante->getColumna();
                Casilla& c = _coordinador->pTablero->getCasilla(fila, col);
                delete c.obj;                            // ELIMINA PIEZA PERDEDORA
                c.obj = nullptr;
                c.pieza = pieza_nada;
                c.bando = bando_nada;
            }
        }

        // GUARDA LA VIDA RESTANTE DE LA GANADORA
        if (ganaP1 && _coordinador->_pAtacanteCombate)
            _coordinador->_pAtacanteCombate->setVida((int)_coordinador->_arena.p1().vida());
        else if (!ganaP1 && _coordinador->_pDefensoraCombate)
            _coordinador->_pDefensoraCombate->setVida((int)_coordinador->_arena.p2().vida());

        _coordinador->_pAtacanteCombate = nullptr;
        _coordinador->_pDefensoraCombate = nullptr;

        ETSIDI::playMusica("sonidos/sonido_fondo_tablero.wav", true);
        _coordinador->estado = EstadoJuego::TABLERO;
    }
}

void GestorInput::teclaUpArena(unsigned char key)
{
    if (!_coordinador) return; // COORDINADOR NO ASIGNADO

    // SUELTA TECLAS P1
    if (key == 'w' || key == 'W') _coordinador->_input.p1.delante = false;
    if (key == 's' || key == 'S') _coordinador->_input.p1.atras = false;
    if (key == 'a' || key == 'A') _coordinador->_input.p1.izquierda = false;
    if (key == 'd' || key == 'D') _coordinador->_input.p1.derecha = false;
}

void GestorInput::teclaEspecialArena(int key)
{
    if (!_coordinador) return; // COORDINADOR NO ASIGNADO
    if (_coordinador->configuracion.modo != ModoJuego::JVJ) return; // SOLO EN JVJ

    // FLECHAS MUEVEN P2
    if (key == GLUT_KEY_UP)    _coordinador->_input.p2.delante = true;
    if (key == GLUT_KEY_DOWN)  _coordinador->_input.p2.atras = true;
    if (key == GLUT_KEY_LEFT)  _coordinador->_input.p2.izquierda = true;
    if (key == GLUT_KEY_RIGHT) _coordinador->_input.p2.derecha = true;
}

void GestorInput::teclaEspecialUpArena(int key)
{
    if (!_coordinador) return; // COORDINADOR NO ASIGNADO
    if (_coordinador->configuracion.modo != ModoJuego::JVJ) return; // SOLO EN JVJ

    // SUELTA FLECHAS P2
    if (key == GLUT_KEY_UP)    _coordinador->_input.p2.delante = false;
    if (key == GLUT_KEY_DOWN)  _coordinador->_input.p2.atras = false;
    if (key == GLUT_KEY_LEFT)  _coordinador->_input.p2.izquierda = false;
    if (key == GLUT_KEY_RIGHT) _coordinador->_input.p2.derecha = false;
}

// =============================================================
// MENÚ PAUSA
// =============================================================

void GestorInput::teclaEspecialGuardando(int key, EstadoJuego& estado)
{
    if (key == GLUT_KEY_DOWN)
        _tablerogl->_pausaSeleccion = (_tablerogl->_pausaSeleccion + 1) % 4; // SUBE
    if (key == GLUT_KEY_UP)
        _tablerogl->_pausaSeleccion = (_tablerogl->_pausaSeleccion - 1 + 4) % 4; // BAJA
}

void GestorInput::teclaGuardando(unsigned char key, EstadoJuego& estado)
{
    if (key == 13) { // ENTER CONFIRMA
        switch (_tablerogl->_pausaSeleccion) {
        case 0:                                            // CONTINUAR
            estado = EstadoJuego::TABLERO;
            break;
        case 1:                                            // GUARDAR
            GestorPartida::guardar(*_coordinador->pTablero,
                _tablerogl->gestorTurnos, _coordinador->configuracion);
            std::cout << "[Pausa] Partida guardada.\n";
            estado = EstadoJuego::TABLERO;                 // SIGUE JUGANDO
            break;
        case 2:                                            // AYUDA — DE MOMENTO NADA
            break;
        case 3:                                            // SALIR SIN GUARDAR
            ETSIDI::stopMusica();
            _coordinador->reiniciarTablero();
            estado = EstadoJuego::MENU;
            break;
        }
    }
    if (key == 27) estado = EstadoJuego::TABLERO;         // ESC SIEMPRE CONTINÚA
}

void GestorInput::ratonGuardando(int x, int y, bool click, EstadoJuego& estado)
{
    float pw = 380, btnW = 300, btnH = 44;
    float px = (_ancho - pw) / 2.0f;
    float ph = 320;
    float py = (_alto - ph) / 2.0f;
    float btnX = px + (pw - btnW) / 2.0f;
    float btnYBase = py + ph - 110;
    float btnSep = 56;

    int gy = _alto - y;                                    // INVIERTE Y

    for (int i = 0; i < 4; i++) {
        float by = btnYBase - i * btnSep;
        if (gy >= by && gy <= by + btnH &&
            x >= btnX && x <= btnX + btnW) {
            _tablerogl->_pausaSeleccion = i;               // HOVER
            if (click) {                                   // CLIC
                unsigned char fakeKey = 13;                // SIMULA ENTER
                teclaGuardando(fakeKey, estado);
            }
        }
    }
}

void GestorInput::ratonMovidoGuardando(int x, int y)
{
    float pw = 380, btnW = 300, btnH = 44;
    float px = (_ancho - pw) / 2.0f;
    float ph = 320;
    float py = (_alto - ph) / 2.0f;
    float btnX = px + (pw - btnW) / 2.0f;
    float btnYBase = py + ph - 110;
    float btnSep = 56;

    int gy = _alto - y;                                    // INVIERTE Y

    for (int i = 0; i < 4; i++) {
        float by = btnYBase - i * btnSep;
        if (gy >= by && gy <= by + btnH &&
            x >= btnX && x <= btnX + btnW) {
            _tablerogl->_pausaSeleccion = i;               // ACTUALIZA SELECCIÓN
        }
    }
}