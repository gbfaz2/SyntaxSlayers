#include "GestorInput.h"
#include "freeglut.h"
#include "menu.h"
#include "tablerogl.h"
#include "Coordinador.h" // INCLUDE COMPLETO SOLO EN EL CPP
#include "ETSIDI.h"
#include "Infiltrado.h"

// MENU
void GestorInput::teclaMenu(unsigned char key, EstadoJuego& estado,
    PantallaIntro& intro, MenuPrincipal& menu, PantallaDestino& destino)
{
    switch (estado) {

    case EstadoJuego::INTRO:
        intro.saltar(); // CUALQUIER TECLA SALTA LA INTRO
        break;

    case EstadoJuego::MENU:

        if (key == 27) { // ESC: RETROCEDE UN PASO
            if (menu.m_paso == 4) { menu.m_paso = 3; menu.m_seleccion = 0; }       // CONFIRMAR → BATALLA
            else if (menu.m_paso == 3) { menu.m_paso = 2; menu.m_seleccion = 0; }  // BATALLA → CONFIG
            else if (menu.m_paso == 2) { menu.m_paso = 1; menu.m_seleccion = 0; }  // CONFIG → MODO
            else if (menu.m_paso == 1) { menu.m_paso = 0; menu.m_seleccion = 0; }  // MODO → PRINCIPAL
            else { menu.m_siguiente = EstadoJuego::INTRO; menu.m_terminado = true; } // PRINCIPAL → INTRO
            return;
        }
        if (menu.m_paso == 2) // PASO 2: CAPTURA NOMBRES Y TAB PARA CAMBIAR FOCO
            menu.teclaTexto(key);
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
    if (estado != EstadoJuego::MENU) return;

    if (menu.m_paso == 2) {
        // FLECHAS IZQUIERDA/DERECHA: CAMBIAN BANDO DE QUIEN TIENE EL FOCO
        if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT) {
            if (menu.m_focoNombre == 0) {
                // CAMBIA BANDO J1 Y ACTUALIZA J2 AUTOMÁTICAMENTE
                menu.m_cfg.bando = (menu.m_cfg.bando == BandoJugador::CRISTIANO)
                    ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
                menu.m_cfg.bando_j2 = (menu.m_cfg.bando == BandoJugador::CRISTIANO)
                    ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
            }
            else if (menu.m_focoNombre == 1 && menu.m_cfg.modo == ModoJuego::JVJ) {
                // CAMBIA BANDO J2 Y ACTUALIZA J1 AUTOMÁTICAMENTE
                menu.m_cfg.bando_j2 = (menu.m_cfg.bando_j2 == BandoJugador::CRISTIANO)
                    ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
                menu.m_cfg.bando = (menu.m_cfg.bando_j2 == BandoJugador::CRISTIANO)
                    ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
            }

            // FLECHAS ARRIBA/ABAJO: CAMBIAN DIFICULTAD DE LA IA
            if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN) {
                // Si estamos en modo IA, las flechas arriba/abajo siempre cambian la dificultad
                // (quitamos lo del foco porque la IA no tiene caja de texto para escribir su nombre)
                if (menu.m_cfg.modo == ModoJuego::JVIA) {
                    if (key == GLUT_KEY_UP) {
                        if (menu.m_cfg.dificultad == NivelDificultad::FACIL) menu.m_cfg.dificultad = NivelDificultad::DIFICIL;
                        else if (menu.m_cfg.dificultad == NivelDificultad::MEDIO) menu.m_cfg.dificultad = NivelDificultad::FACIL;
                        else if (menu.m_cfg.dificultad == NivelDificultad::DIFICIL) menu.m_cfg.dificultad = NivelDificultad::MEDIO;
                    }
                    else if (key == GLUT_KEY_DOWN) {
                        if (menu.m_cfg.dificultad == NivelDificultad::FACIL) menu.m_cfg.dificultad = NivelDificultad::MEDIO;
                        else if (menu.m_cfg.dificultad == NivelDificultad::MEDIO) menu.m_cfg.dificultad = NivelDificultad::DIFICIL;
                        else if (menu.m_cfg.dificultad == NivelDificultad::DIFICIL) menu.m_cfg.dificultad = NivelDificultad::FACIL;
                    }
                }
            }
        }
        return; // EN PASO 2 LAS FLECHAS SOLO CAMBIAN BANDO
    }

    // FLECHAS ARRIBA/ABAJO: NAVEGAN LA SELECCIÓN
    if (menu.maxOpciones() > 0) {
        if (key == GLUT_KEY_UP) {
            if (menu.m_seleccion > 0) menu.m_seleccion--;
            else menu.m_seleccion = menu.maxOpciones() - 1;
        }
        if (key == GLUT_KEY_DOWN) {
            if (menu.m_seleccion < menu.maxOpciones() - 1) menu.m_seleccion++;
            else menu.m_seleccion = 0;
        }
    }

    // FLECHAS IZQUIERDA/DERECHA EN PASO 1 Y 4 (opciones horizontales)
    if (menu.m_paso == 1 || menu.m_paso == 4) {
        if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT)
            menu.m_seleccion = (menu.m_seleccion == 0) ? 1 : 0;
    }
}

void GestorInput::teclaEspecialAyuda(int key, EstadoJuego& estado)
{
    if (_coordinador->_ayudaSeccion == -1) {
        if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT ||
            key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
            _coordinador->_ayudaSeleccion = (_coordinador->_ayudaSeleccion == 0) ? 1 : 0;
    }
}

void GestorInput::teclaAyuda(unsigned char key, EstadoJuego& estado)
{
    if (key == 13 && _coordinador->_ayudaSeccion == -1)
        _coordinador->_ayudaSeccion = _coordinador->_ayudaSeleccion; // ENTRA EN SECCIÓN
    if (key == 27) {
        if (_coordinador->_ayudaSeccion != -1)
            _coordinador->_ayudaSeccion = -1; // VUELVE AL SELECTOR
        else
            estado = EstadoJuego::MENU; // VUELVE AL MENÚ
    }
}

void GestorInput::ratonMenu(int boton, int state, int x, int y,
    EstadoJuego& estado, PantallaIntro& intro,
    MenuPrincipal& menu, PantallaDestino& destino)
{
    if (state != GLUT_DOWN) return;

    switch (estado) {
    case EstadoJuego::INTRO:
        intro.saltar();
        break;
    case EstadoJuego::MENU:
        if (boton == GLUT_LEFT_BUTTON) {
            ratonMovidoMenu(x, y, estado, menu); // actualiza hover general
            if (menu.m_paso == 2) {
                int gy = _alto - y; // invertir coordenada y

                // detecta click en boton continuar
                float btnW = 200, btnH = 40;
                float btnX = _ancho / 2.0f - btnW / 2.0f;
                float btnY = _alto * 0.12f;
                if (x >= btnX && x <= btnX + btnW && gy >= btnY && gy <= btnY + btnH) {
                    menu.confirmar();
                    return;
                }

                // 2. Detectar clics en paneles de configuración (Matemática extraída de DibujaMenu)
                float mitad = _ancho / 2.0f;
                float colW = mitad * 0.80f;
                float col1X = mitad - colW - 20;
                float col2X = mitad + 20;
                float topY = _alto * 0.60f;
                float campoY = topY - 50;
                float bandoY = campoY - 80;
                float bw = (colW - 10) / 2.0f;

                // Función lambda para comprobar si el clic cae dentro de una caja
                auto enCaja = [&](float bx, float by, float anchoCaja, float altoCaja) {
                    return x >= bx && x <= bx + anchoCaja && gy >= by && gy <= by + altoCaja;
                    };

                // CLICS JUGADOR 1 (Columna Izquierda)
                if (enCaja(col1X, campoY - 30, colW, 36)) menu.m_focoNombre = 0; // Foco Nombre J1

                if (enCaja(col1X, bandoY - 30, bw, 36)) { // Botón Cristiano J1
                    menu.m_cfg.bando = BandoJugador::CRISTIANO;
                    menu.m_cfg.bando_j2 = BandoJugador::MUSULMAN;
                    menu.m_focoNombre = 0;
                }
                if (enCaja(col1X + bw + 10, bandoY - 30, bw, 36)) { // Botón Andalusí J1
                    menu.m_cfg.bando = BandoJugador::MUSULMAN;
                    menu.m_cfg.bando_j2 = BandoJugador::CRISTIANO;
                    menu.m_focoNombre = 0;
                }

                // CLICS JUGADOR 2 / IA (Columna Derecha)
                if (menu.m_cfg.modo == ModoJuego::JVJ) {
                    if (enCaja(col2X, campoY - 30, colW, 36)) menu.m_focoNombre = 1; // Foco Nombre J2

                    if (enCaja(col2X, bandoY - 30, bw, 36)) { // Botón Cristiano J2
                        menu.m_cfg.bando_j2 = BandoJugador::CRISTIANO;
                        menu.m_cfg.bando = BandoJugador::MUSULMAN;
                        menu.m_focoNombre = 1;
                    }
                    if (enCaja(col2X + bw + 10, bandoY - 30, bw, 36)) { // Botón Andalusí J2
                        menu.m_cfg.bando_j2 = BandoJugador::MUSULMAN;
                        menu.m_cfg.bando = BandoJugador::CRISTIANO;
                        menu.m_focoNombre = 1;
                    }
                }
                else if (menu.m_cfg.modo == ModoJuego::JVIA) {
                    for (int i = 0; i < 3; i++) {
                        float dy = campoY - 30 - i * 46;
                        if (enCaja(col2X, dy, colW, 36)) { // Clic en Dificultad IA
                            menu.m_cfg.dificultad = (i == 0) ? NivelDificultad::FACIL :
                                (i == 1) ? NivelDificultad::MEDIO :
                                NivelDificultad::DIFICIL;
                        }
                    }
                }
            }
            else {
                menu.confirmar();
            }
        }
        break;
    case EstadoJuego::DESTINO:
        destino.avanzar();
        break;
    default:
        break;
    }
}

void GestorInput::ratonMovidoMenu(int mx, int my, EstadoJuego& estado, MenuPrincipal& menu)
{
    if (estado != EstadoJuego::MENU) return;

    int gy = _alto - my;

    auto enCaja = [&](float x, float y, float aw, float ah) {
        return mx >= x && mx <= x + aw && gy >= y && gy <= y + ah;
        };

    // PASO 0: Menú Principal (5 opciones verticales)
    if (menu.m_paso == 0) {
        float aw = 240, ah = 52, sep = 12;
        float sx = _ancho * 0.08f;
        float sy = _alto / 2.0f + 110;
        for (int i = 0; i < 5; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) menu.m_seleccion = i;
    }
    // PASO 1: Elegir Modo (JvJ / JvIA, horizontales)
    else if (menu.m_paso == 1) {
        float aw = 300, ah = 52, sep = 40;
        float sy = _alto / 2.0f + 10;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, sy, aw, ah))      menu.m_seleccion = 1;
    }
    // PASO 2: Configuración — clic cambia foco de nombre y bando
    else if (menu.m_paso == 2) {
        float mitad = _ancho / 2.0f;
        float colW = mitad * 0.80f;
        float col1X = mitad - colW - 20;
        float col2X = mitad + 20;
        float campoY = _alto * 0.60f - 50;

        // BOTÓN CONTINUAR — solo actualiza hover, NO llama confirmar()
        float btnW = 200, btnH = 40;
        float btnX = _ancho / 2.0f - btnW / 2.0f;
        float btnY = _alto * 0.12f;
    }
    // PASO 3: Selección de Batalla (4 opciones verticales)
    else if (menu.m_paso == 3) {
        float aw = 400, ah = 48, sep = 12;
        float sx = _ancho / 2.0f - aw / 2.0f;
        float sy = _alto / 2.0f + 90;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) menu.m_seleccion = i;
    }
    // PASO 4: Confirmar (JUGAR / VOLVER, horizontales)
    else if (menu.m_paso == 4) {
        float aw = 180, ah = 44, sep = 30;
        float cy = _alto / 2.0f - 70;
        if (enCaja(_ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah)) menu.m_seleccion = 0;
        if (enCaja(_ancho / 2.0f + sep / 2.0f, cy, aw, ah))      menu.m_seleccion = 1;
    }
}

// TABLERO
bool GestorInput::ejecutarHechizo(BandoPieza bando, int fila, int col)
{
    bool ejecutado = false; // CONTROLA SI SE EJECUTÓ
    std::string mensajeError = ""; //VARIABLE PARA GUARDAR EL MOTIVO DEL ERROR EN EL ASEDIO

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
        ejecutado = _coordinador->pGestorHechizos->asedio(bando, fila, col,mensajeError);
        //SI FALLA FORZAMOS LA APARICIÓN DEL CARTEL TEMPORAL
        if (!ejecutado && !mensajeError.empty()) 
            _tablerogl->mostrarMensajeInvalido(mensajeError, true);
        break;

    default: break;                                    
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

    // H: ACTIVA MODO HECHIZO P1 (SOLO SI EL REY ESTÁ SELECCIONADO)
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

    // J: ACTIVA MODO HECHIZO P2 (SOLO SI EL EMIR ESTÁ SELECCIONADO)
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

    // I: ACTIVA HABILIDAD INFILTRADO/ASESINO
    if (key == 'i' || key == 'I') {
        if (_tablerogl->piezaSeleccionada) {
            const Casilla& cas = _coordinador->pTablero->getCasilla(_tablerogl->fromFila, _tablerogl->fromCol);

            // Verificamos si la pieza seleccionada es un Infiltrado
            if (cas.obj != nullptr && dynamic_cast<Infiltrado*>(cas.obj) != nullptr) {
                _tablerogl->_modoInfiltrado = true;
                _tablerogl->_infiltradoFila = _tablerogl->fromFila;
                _tablerogl->_infiltradoCol = _tablerogl->fromCol;

                // Deseleccionamos para que pueda mover el cursor libremente a ver a sus rivales
                _tablerogl->piezaSeleccionada = false;
                _tablerogl->fromFila = _tablerogl->fromCol = -1;

                _tablerogl->mostrarMensajeInvalido("Modo Copia: Elige rival", true,false);
            }
            else 
                _tablerogl->mostrarMensajeInvalido("No tienes la habilidad", true, true);
        }
        return;
    }

    // ENTER: CONFIRMAR ROBO DE STATS DEL INFILTRADO
    if (key == 13) {
        if (_tablerogl->_modoInfiltrado && _tablerogl->piezaSeleccionada) {
            const Casilla& casEnemigo = _coordinador->pTablero->getCasilla(_tablerogl->fromFila, _tablerogl->fromCol);
            Infiltrado* inf = dynamic_cast<Infiltrado*>(_coordinador->pTablero->getCasilla(_tablerogl->_infiltradoFila, _tablerogl->_infiltradoCol).obj);

            if (inf && casEnemigo.obj) {
                inf->copiarStats(*(casEnemigo.obj));

                // Limpiamos los estados
                _tablerogl->_modoInfiltrado = false;
                _tablerogl->piezaSeleccionada = false;
                _tablerogl->fromFila = _tablerogl->fromCol = -1;

                _tablerogl->mostrarMensajeInvalido("Stats copiados", true, false); // Sin sonido
                _tablerogl->gestorTurnos.terminarTurno();
            }
            return;
        }
    }

    // Z: CANCELAR MODO INFILTRADO
    if (key == 'z' && _tablerogl->_modoInfiltrado) {
        _tablerogl->_modoInfiltrado = false;
        _tablerogl->piezaSeleccionada = false;
        _tablerogl->fromFila = _tablerogl->fromCol = -1;
        _tablerogl->mostrarMensajeInvalido("Habilidad cancelada", true, false);
        return;
    }

    // NÚMERO: ELIGE HECHIZO (solo en modo hechizo, conjuro no elegido aún)
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

    // MOVIMIENTO CURSOR P1 CON WASD (disponible siempre)
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

    // ESPACIO: CONFIRMA HECHIZO P1 O SELECCIONA/MUEVE PIEZA P1
    if (key == ' ') {
        //PARA LA SELECCION DE PIEZAS CON EL PERSONAJE DE INFILTRADO
        if (_tablerogl->_modoInfiltrado && _tablerogl->gestorTurnos.getBandoActual() == bando_local) {
            int fila = _tablerogl->Filacursor[0];
            int col = _tablerogl->Colcursor[0];
            const Casilla& casEnemigo = _coordinador->pTablero->getCasilla(fila, col);

            if (casEnemigo.obj && casEnemigo.bando == bando_rival) {
                _tablerogl->piezaSeleccionada = true;
                _tablerogl->fromFila = fila;
                _tablerogl->fromCol = col;
                _tablerogl->mostrarMensajeInvalido("Pulsa ENTER para robar stats", true, false);
            }
            else 
                _tablerogl->mostrarMensajeInvalido("Debes elegir un rival", true, true);
            return;
        }

        if (_tablerogl->_modoHechizo && _tablerogl->_bandoHechizo == bando_local) {
            int fila = _tablerogl->Filacursor[0];          // FILA CURSOR P1
            int col = _tablerogl->Colcursor[0];           // COL CURSOR P1
            ejecutarHechizo(bando_local, fila, col);       // EJECUTA HECHIZO P1
            return;                                        // NUNCA LLEGA A trySelectorMove
        }
        _tablerogl->trySelectorMove(bando_local);          // SELECCIONA O MUEVE P1
        return;
    }

    // PUNTO: CONFIRMA HECHIZO P2 O SELECCIONA/MUEVE PIEZA P2
    if (key == '.') {
        //PARA LA SELECCION DE PIEZAS CON EL PERSONAJE DE ASESINO DE ÉLITE
        if (_tablerogl->_modoInfiltrado && _tablerogl->gestorTurnos.getBandoActual() == bando_rival) {
            int fila = _tablerogl->Filacursor[1];
            int col = _tablerogl->Colcursor[1];
            const Casilla& casEnemigo = _coordinador->pTablero->getCasilla(fila, col);

            if (casEnemigo.obj && casEnemigo.bando == bando_local) {
                _tablerogl->piezaSeleccionada = true;
                _tablerogl->fromFila = fila;
                _tablerogl->fromCol = col;
                _tablerogl->mostrarMensajeInvalido("Pulsa ENTER para robar stats", true, false);
            }
            else 
                _tablerogl->mostrarMensajeInvalido("Debes elegir un rival", true, true);
            return;
        }

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

    // MODO INFILTRADO: CLIC IZQUIERDO SELECCIONA PARA VER CARACTERÍSTICAS
    if (_tablerogl->_modoInfiltrado) {
        const Casilla& casEnemigo = _tablerogl->m_tablero->getCasilla(clickFila, clickCol);
        Infiltrado* inf = dynamic_cast<Infiltrado*>(_tablerogl->m_tablero->getCasilla(_tablerogl->_infiltradoFila, _tablerogl->_infiltradoCol).obj);

        BandoPieza miBando = _tablerogl->gestorTurnos.getBandoActual();

        if (inf && casEnemigo.obj && casEnemigo.bando != miBando && casEnemigo.bando != bando_nada) {
            // Solo seleccionamos la pieza para que aparezca el panel
            _tablerogl->piezaSeleccionada = true;
            _tablerogl->fromFila = clickFila;
            _tablerogl->fromCol = clickCol;

            // Avisamos de cómo confirmar (forzar = true, sonido = false)
            _tablerogl->mostrarMensajeInvalido("Pulsa ENTER para robar stats", true, false);
        }
        else {
            // Error real: sí hace sonido
            _tablerogl->mostrarMensajeInvalido("Debes elegir un rival", true, true);
        }
        return; // IMPORTANTE: Cortamos aquí
    }

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

// ARENA
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
        ETSIDI::playMusica("sonidos/MENU.mp3", true);
        
        return;
    }

    // ENTER VUELVE AL TABLERO SI EL COMBATE HA TERMINADO
    if (key == 13 && _coordinador->_arena.resultado() != ResultadoCombate::EnCurso) {
        ETSIDI::stopMusica();
        bool ganaP1 = (_coordinador->_arena.resultado() == ResultadoCombate::GanaP1);

        // P1 FUE EL JUGADOR LOCAL, P2 FUE EL RIVAL
        bool atacanteEraLocal = (_coordinador->_pAtacanteCombate &&
            _coordinador->_pAtacanteCombate->getBando() == Bando::CRISTIANO);

        Pieza* pJugador = atacanteEraLocal ? _coordinador->_pAtacanteCombate
            : _coordinador->_pDefensoraCombate;
        Pieza* pRival = atacanteEraLocal ? _coordinador->_pDefensoraCombate
            : _coordinador->_pAtacanteCombate;

        bool ganaJugador = ganaP1; // P1 siempre fue el jugador

        if (ganaJugador) {
            bool jugadorEraAtacante = (_coordinador->_pAtacanteCombate->getBando() == Bando::CRISTIANO);

            if (jugadorEraAtacante) {
                // JUGADOR ATACÓ Y GANÓ → elimina defensora, mueve atacante a su casilla
                Casilla& cDef = _coordinador->pTablero->getCasilla(
                    _coordinador->_filaDefensora, _coordinador->_colDefensora);
                delete cDef.obj;
                cDef.obj = nullptr; cDef.pieza = pieza_nada; cDef.bando = bando_nada;

                _coordinador->pTablero->muevePieza(
                    _coordinador->_filaAtacante, _coordinador->_colAtacante,
                    _coordinador->_filaDefensora, _coordinador->_colDefensora);

                if (pJugador) pJugador->setVida((int)_coordinador->_arena.p1().vida());
            }
            else {
                // RIVAL ATACÓ, JUGADOR DEFENDIÓ Y GANÓ → elimina atacante, defensora se queda
                Casilla& cAtac = _coordinador->pTablero->getCasilla(
                    _coordinador->_filaAtacante, _coordinador->_colAtacante);
                delete cAtac.obj;
                cAtac.obj = nullptr; cAtac.pieza = pieza_nada; cAtac.bando = bando_nada;

                // LA DEFENSORA YA ESTÁ EN SU CASILLA, SOLO ACTUALIZAMOS SU VIDA
                if (pJugador) pJugador->setVida((int)_coordinador->_arena.p1().vida());
            }
        }
        else {
            bool jugadorEraAtacante = (_coordinador->_pAtacanteCombate->getBando() == Bando::CRISTIANO);

            if (jugadorEraAtacante) {
                // JUGADOR ATACÓ Y PERDIÓ → elimina atacante (jugador)
                Casilla& c = _coordinador->pTablero->getCasilla(
                    _coordinador->_filaAtacante, _coordinador->_colAtacante);
                delete c.obj;
                c.obj = nullptr; c.pieza = pieza_nada; c.bando = bando_nada;

                if (pRival) pRival->setVida((int)_coordinador->_arena.p2().vida());
            }
            else {
                // RIVAL ATACÓ Y GANÓ → elimina defensora (jugador), mueve atacante a su casilla
                Casilla& cDef = _coordinador->pTablero->getCasilla(
                    _coordinador->_filaDefensora, _coordinador->_colDefensora);
                delete cDef.obj;
                cDef.obj = nullptr; cDef.pieza = pieza_nada; cDef.bando = bando_nada;

                _coordinador->pTablero->muevePieza(
                    _coordinador->_filaAtacante, _coordinador->_colAtacante,
                    _coordinador->_filaDefensora, _coordinador->_colDefensora);

                if (pRival) pRival->setVida((int)_coordinador->_arena.p2().vida());
            }
        }

        // Guarda el combate terminado en el historial de la partida
        //Con el push_back se guarda una copia del combate actual, por lo q no hace falta grabar la partida, simplemente se reproducen los mismos movimientos
        _coordinador->_combateEnCurso.ganoP1 = ganaP1;
        _coordinador->_replayPartida.combates.push_back(_coordinador->_combateEnCurso);
        _coordinador->_combateEnCurso = CombateRegistro{};
        _coordinador->_grabandoCombate = false;
        _coordinador->_pAtacanteCombate = nullptr;
        _coordinador->_pDefensoraCombate = nullptr;

        ETSIDI::playMusica("sonidos/TABLERO.mp3", true);
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

// MENÚ PAUSA
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
        case 0:  // CONTINUAR
            estado = EstadoJuego::TABLERO;
            break;
        case 1:  // GUARDAR
            GestorPartida::guardar(*_coordinador->pTablero,
                _tablerogl->gestorTurnos, _coordinador->configuracion);
            std::cout << "[Pausa] Partida guardada.\n";
            ETSIDI::playMusica("sonidos/MENU.mp3", true);
            _coordinador->reiniciarTablero();
            _coordinador->estado = EstadoJuego::MENU;
            break;
        case 2:  // AYUDA
            ETSIDI::playMusica("sonidos/AYUDA.mp3", false);
            _coordinador->estado = EstadoJuego::AYUDA;
            break;
        case 3: // SALIR SIN GUARDAR
            ETSIDI::playMusica("sonidos/MENU.mp3", true);
            _coordinador->reiniciarTablero();
            estado = EstadoJuego::MENU;
            break;
        }
    }
    if (key == 27) {

        estado = EstadoJuego::TABLERO;
    }// ESC SIEMPRE CONTINÚA
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

void GestorInput::ratonMovidoAyuda(int mx, int my, EstadoJuego& estado)
{
    // Solo actuamos si estamos en la pantalla de ayuda seleccionando sección
    if (estado != EstadoJuego::AYUDA || _coordinador->_ayudaSeccion != -1) return;

    int gy = _alto - my; // Invertimos la Y para que coincida con OpenGL
    float aw = 300, ah = 52, sep = 40;
    float sy = _alto / 2.0f + 10;

    // Coordenadas exactas de los botones según dibujamenu.cpp
    float btnControlesX = _ancho / 2.0f - aw - sep / 2.0f;
    float btnNormasX = _ancho / 2.0f + sep / 2.0f;

    // ¿El ratón está sobre "Controles"?
    if (mx >= btnControlesX && mx <= btnControlesX + aw && gy >= sy && gy <= sy + ah) {
        _coordinador->_ayudaSeleccion = 0;
    }
    // ¿El ratón está sobre "Normas"?
    else if (mx >= btnNormasX && mx <= btnNormasX + aw && gy >= sy && gy <= sy + ah) {
        _coordinador->_ayudaSeleccion = 1;
    }
}

void GestorInput::ratonAyuda(int boton, int state, int x, int y, EstadoJuego& estado)
{
    if (estado != EstadoJuego::AYUDA || state != GLUT_DOWN || boton != GLUT_LEFT_BUTTON) return;

    if (_coordinador->_ayudaSeccion == -1) {
        // Actualizamos la selección por si el "hover" no se registró a tiempo
        ratonMovidoAyuda(x, y, estado);

        int gy = _alto - y;
        float aw = 300, ah = 52, sep = 40;
        float sy = _alto / 2.0f + 10;
        float btnControlesX = _ancho / 2.0f - aw - sep / 2.0f;
        float btnNormasX = _ancho / 2.0f + sep / 2.0f;

        // Solo confirmamos (simulando un ENTER) si hemos hecho clic REALMENTE dentro del botón
        if ((x >= btnControlesX && x <= btnControlesX + aw && gy >= sy && gy <= sy + ah) ||
            (x >= btnNormasX && x <= btnNormasX + aw && gy >= sy && gy <= sy + ah)) {

            unsigned char fakeKey = 13; // 13 es el código de ENTER
            teclaAyuda(fakeKey, estado);
        }
    }
    else {
        // Si ya estás dentro leyendo las normas o controles, hacer clic en la pantalla te devuelve atrás
        unsigned char fakeKey = 27; // 27 es el código de ESC
        teclaAyuda(fakeKey, estado);
    }
}