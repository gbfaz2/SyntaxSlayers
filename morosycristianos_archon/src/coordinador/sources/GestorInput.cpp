#include "GestorInput.h"
#include "freeglut.h"
#include "menu.h"
#include "tablerogl.h"
#include "Coordinador.h" // INCLUDE COMPLETO SOLO EN EL CPP

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
        if (key == 27) { // ESC: RETROCEDE UN PASO O VUELVE A INTRO
            if (menu.m_paso > 0) { menu.m_paso--; menu.m_seleccion = 0; }
            else { menu.m_siguiente = EstadoJuego::INTRO; menu.m_terminado = true; }
            return;
        }
        if (key == 13) menu.confirmar(); // SOLO ENTER CONFIRMA (ESPACIO RESERVADO PARA TABLERO)
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

    if (key == GLUT_KEY_UP)
        menu.m_seleccion = (menu.m_seleccion - 1 + menu.maxOpciones()) % menu.maxOpciones(); // SUBE SELECCION
    if (key == GLUT_KEY_DOWN)
        menu.m_seleccion = (menu.m_seleccion + 1) % menu.maxOpciones(); // BAJA SELECCION
    if (key == GLUT_KEY_LEFT && menu.m_paso == 1) menu.m_seleccion = 0; // SELECCIONA IZQUIERDA
    if (key == GLUT_KEY_RIGHT && menu.m_paso == 1) menu.m_seleccion = 1; // SELECCIONA DERECHA
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
    else if (menu.m_paso == 1) { // PASO 1: SELECCION BANDO
        float aw = 240, ah = 55, sep = 30, sy = _alto / 2.0f + 10;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, sy, aw, ah))      menu.m_seleccion = 1;
    }
    else if (menu.m_paso == 2) { // PASO 2: SELECCION BATALLA
        float aw = 340, ah = 48, sep = 12;
        float sx = _ancho / 2.0f - aw / 2.0f, sy = _alto / 2.0f + 80;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) menu.m_seleccion = i;
    }
    else if (menu.m_paso == 3) { // PASO 3: CONFIRMACION
        float aw = 180, ah = 44, sep = 30, cy = _alto / 2.0f - 20;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, cy, aw, ah))      menu.m_seleccion = 1;
    }
}

// =============================================================
// TABLERO
// =============================================================

void GestorInput::teclaTablero(unsigned char key, EstadoJuego& estado)
{
    if (!_tablerogl) return;                           // TABLEROGL NO ASIGNADO
    if (_tablerogl->victoria_ != bando_nada) return;  // PARTIDA TERMINADA

    if (key == 'z' || key == 'Z') { // Z: CANCELA SELECCION P1
        if (_tablerogl->piezaSeleccionada) {
            _tablerogl->piezaSeleccionada = false;
            _tablerogl->fromFila = _tablerogl->fromCol = -1;
        }
        return;
    }

    if (key == 'q' || key == 'Q') { exit(0); return; } // Q: CIERRA EL JUEGO

    if (_tablerogl->gestorTurnos.getBandoActual() == bando_local) {
        int& rL = _tablerogl->Filacursor[0]; // FILA CURSOR LOCAL
        int& cL = _tablerogl->Colcursor[0];  // COLUMNA CURSOR LOCAL
        if (key == 'w' || key == 'W') { if (rL > 0)                  rL--; } // CURSOR ARRIBA
        if (key == 's' || key == 'S') { if (rL < _tablerogl->N - 1)  rL++; } // CURSOR ABAJO
        if (key == 'a' || key == 'A') { if (cL > 0)                  cL--; } // CURSOR IZQUIERDA
        if (key == 'd' || key == 'D') { if (cL < _tablerogl->N - 1)  cL++; } // CURSOR DERECHA
        if (key == ' ') _tablerogl->trySelectorMove(bando_local);             // SELECCIONA O MUEVE
    }

    if (_tablerogl->gestorTurnos.getBandoActual() == bando_rival) {
        if (key == '.') _tablerogl->trySelectorMove(bando_rival); // PUNTO: SELECCIONA O MUEVE
        if (key == ',') { // COMA: CANCELA SELECCION
            _tablerogl->piezaSeleccionada = false;
            _tablerogl->fromFila = _tablerogl->fromCol = -1;
        }
    }

    if (key == 'h' || key == 'H') { // H: ACTIVA HECHIZO AVITUALLAMIENTO P1
        _tablerogl->_modoHechizo = true;
        _tablerogl->_conjuroActivo = Conjuro::AVITUALLAMIENTO;
        _tablerogl->_bandoHechizo = bando_local;
    }

    if (key == ' ') {
        if (_tablerogl->_modoHechizo) { // MODO HECHIZO: APLICA EN CASILLA ACTUAL
            int fila = _tablerogl->Filacursor[0];
            int col = _tablerogl->Colcursor[0];
            _coordinador->pGestorHechizos->avituallamiento(bando_local, fila, col);
            _tablerogl->_modoHechizo = false; // DESACTIVA MODO HECHIZO
        }
        else {
            _tablerogl->trySelectorMove(bando_local); // MOVIMIENTO NORMAL
        }
    }

    if (key == 'j' || key == 'J') { // J: ACTIVA HECHIZO AVITUALLAMIENTO P2
        _tablerogl->_modoHechizo = true;
        _tablerogl->_conjuroActivo = Conjuro::AVITUALLAMIENTO;
        _tablerogl->_bandoHechizo = bando_rival;
    }

    if (key == '.') {
        if (_tablerogl->_modoHechizo) { // MODO HECHIZO: APLICA EN CASILLA ACTUAL
            int fila = _tablerogl->Filacursor[1];
            int col = _tablerogl->Colcursor[1];
            _coordinador->pGestorHechizos->avituallamiento(bando_rival, fila, col);
            _tablerogl->_modoHechizo = false; // DESACTIVA MODO HECHIZO
        }
        else {
            _tablerogl->trySelectorMove(bando_rival); // MOVIMIENTO NORMAL
        }
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

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);   // LEE MATRIZ MODELVIEW
    glGetDoublev(GL_PROJECTION_MATRIX, projection); // LEE MATRIZ PROYECCION
    glGetIntegerv(GL_VIEWPORT, viewport);            // LEE VIEWPORT

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;           // INVIERTE Y
    glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); // LEE PROFUNDIDAD
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ); // PIXEL A MUNDO

    int clickFila, clickCol;
    _tablerogl->world2cell(posX, posY, clickFila, clickCol); // MUNDO A CASILLA

    _tablerogl->xcasilla_sel = clickFila; // ACTUALIZA CASILLA BAJO CURSOR
    _tablerogl->ycasilla_sel = clickCol;

    if (down) { _tablerogl->controlKey = ctrlKey; _tablerogl->shiftKey = shiftKey; }
    else { _tablerogl->controlKey = _tablerogl->shiftKey = false; }

    if (button == MOUSE_LEFT_BUTTON)   _tablerogl->leftButton = down;   // ACTUALIZA BOTON IZQ
    if (button == MOUSE_RIGHT_BUTTON)  _tablerogl->rightButton = down;  // ACTUALIZA BOTON DER
    if (button == MOUSE_MIDDLE_BUTTON) _tablerogl->midButton = down;    // ACTUALIZA BOTON MED

    if (!down) return; // SOLO PROCESAMOS AL PULSAR

    if (button == MOUSE_RIGHT_BUTTON) { // CLIC DERECHO: CANCELA SELECCION
        _tablerogl->piezaSeleccionada = false;
        _tablerogl->fromFila = _tablerogl->fromCol = -1;
        return;
    }

    if (button != MOUSE_LEFT_BUTTON) return; // SOLO CLIC IZQUIERDO A PARTIR DE AQUI

    if (clickFila < 0 || clickFila >= _tablerogl->N ||
        clickCol < 0 || clickCol >= _tablerogl->N) return; // FUERA DEL TABLERO

    const Casilla& clicked = _tablerogl->m_tablero->getCasilla(clickFila, clickCol);

    if (!_tablerogl->piezaSeleccionada) { // SIN PIEZA SELECCIONADA: SELECCIONA
        if (clicked.pieza != pieza_nada) {
            int idx = (clicked.bando == bando_local) ? 0 : 1;
            _tablerogl->Filacursor[idx] = clickFila;
            _tablerogl->Colcursor[idx] = clickCol;
            _tablerogl->trySelectorMove(clicked.bando);
        }
    }
    else { // CON PIEZA SELECCIONADA: MUEVE
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
    if (!_coordinador) return; // COORDINADOR NO ASIGNADO

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

        /*Pieza* perdedora = ganaP1 ? _coordinador->_pDefensoraCombate
            : _coordinador->_pAtacanteCombate;*/

        if (ganaP1) {
            Pieza* atacante = _coordinador->_pAtacanteCombate;
            Pieza* defensora = _coordinador->_pDefensoraCombate;
            if (atacante && defensora && _coordinador->pTablero) {
                int filaAtacante = atacante->getFila();
                int colAtacante = atacante->getColumna();
                int filaDefensora = defensora->getFila();
                int colDefensora = defensora->getColumna();

                Casilla& cDef = _coordinador->pTablero->getCasilla(filaDefensora, colDefensora);
                delete cDef.obj;
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
                delete c.obj;
                c.obj = nullptr;
                c.pieza = pieza_nada;
                c.bando = bando_nada;
            }
        }

        // Guardamos la vida restante de la ganadora
        if (ganaP1 && _coordinador->_pAtacanteCombate)
            _coordinador->_pAtacanteCombate->setVida((int)_coordinador->_arena.p1().vida());
        else if (!ganaP1 && _coordinador->_pDefensoraCombate)
            _coordinador->_pDefensoraCombate->setVida((int)_coordinador->_arena.p2().vida());

        _coordinador->_pAtacanteCombate = nullptr;
        _coordinador->_pDefensoraCombate = nullptr;

        ETSIDI::playMusica("sonido_fondo_tablero.wav", true);
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