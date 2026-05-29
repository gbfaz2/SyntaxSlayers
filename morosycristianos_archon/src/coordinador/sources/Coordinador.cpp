#include "Coordinador.h"
#include "dibuja.h"
#include "dibujamenu.h"
#include "dibujatablero.h"
#include "dibujaarena.h"
#include "ETSIDI.h"
#include "freeglut.h"
#include <ctime>
#include <cstdlib>
#include "GestorPartida.h"
#include "tablerogl.h"
#include "GestorRanking.h"

Coordinador::~Coordinador()
{
	// ESPERA A QUE EL HILO DE IA TERMINE ANTES DE DESTRUIR TODO
	if (_hiloIA.joinable()) _hiloIA.join();

	delete pTablerogl; // LIBERA TABLEROGL
	delete pTablero;   // LIBERA TABLERO
	delete pGestorHechizos;
}

void Coordinador::inicializa()
{
	ETSIDI::playMusica("sonidos/MENU.mp3", true);
	srand((unsigned)time(nullptr));
	pantallaIntro.reiniciar();
	menuPrincipal.reiniciar();
	DibujaArena::arena_configurar_vista(_anchoVentana, _altoVentana); // CONFIGURA CAMARA ARENA
	estado = EstadoJuego::INTRO; // ARRANCA EN INTRO
	gestorInput.setCoordinador(this); // ASIGNA COORDINADOR AL GESTOR
	DibujaArena::arena_init(); // Inicializa recursos gráficos de la arena (texturas, personajes, etc.)
}

void Coordinador::dibuja()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (estado) {

	case EstadoJuego::INTRO:
		// LA PROPIA CLASE DIBUJAMENU ENTRA Y SALE DE 2D INTERNAMENTE
		DibujaMenu::intro_dibujar(pantallaIntro, _anchoVentana, _altoVentana); // PINTA LA ANIMACION DE INTRO

		if (pantallaIntro.terminado()) {
			pantallaIntro.reiniciar();
			menuPrincipal.reiniciar();
			estado = EstadoJuego::MENU;
		}
		break;

	case EstadoJuego::MENU:

		
		// LA PROPIA CLASE DIBUJAMENU ENTRA Y SALE DE 2D INTERNAMENTE
		DibujaMenu::menu_dibujar(menuPrincipal, _anchoVentana, _altoVentana); // PINTA LAS FASES DEL MENU PRINCIPAL

		if (menuPrincipal.terminado()) {
			EstadoJuego siguiente = menuPrincipal.siguienteEstado();
			configuracion = menuPrincipal.getConfiguracion();
			_minimax.setDificultad(configuracion.dificultad);

			if (siguiente == EstadoJuego::FINAL) exit(0);

			if (siguiente == EstadoJuego::DESTINO) {
				pantallaDestino.reiniciar(configuracion);
				if (!pTablero) { // SOLO CREA EL TABLERO UNA VEZ
					pTablero = new Tablero();
					pTablerogl = new Tablerogl(pTablero);
					DibujaTablero::tablero_init();
					pTablerogl->setBatalla((int)configuracion.batalla); // ASIGNA BATALLA AL TABLERO
					BandoPieza bandoInicial = (configuracion.turno1 == BandoJugador::MUSULMAN) ? bando_rival : bando_local;//FIJAMOS QUIEN EMPIEZA SEGÚN LA BATALLA SELECCIONADA
					pTablerogl->setBandoInicial(bandoInicial);

					gestorInput.setTablerogl(pTablerogl); // ASIGNA TABLEROGL AL GESTOR

					// NOMBRES DE LOS JUGADORES PARA EL HUD
					pTablerogl->nombre_j1 = configuracion.nombre_j1;
					pTablerogl->nombre_j2 = configuracion.nombre_j2;

					pGestorHechizos = new GestorHechizos(*pTablero,
						dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_local)),
						dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_rival)));
				}
			}
			if (siguiente == EstadoJuego::RANKING)
			{
				_rankingTop10 = GestorRanking::cargar();
				_rankingGanador = ""; // Limpiar para que no muestre resultado anterior
			}
			estado = siguiente;
		}
		break;

	case EstadoJuego::DESTINO:
		// NUEVO MOTOR GRAFICO CENTRALIZADO GESTIONA ESTA TRANSICION AL COMPLETO
		DibujaMenu::destino_dibujar(pantallaDestino, _anchoVentana, _altoVentana); // PINTA EFECTOS, FONDOS Y PARTICULAS

		if (pantallaDestino.terminado()) {
			estado = EstadoJuego::TABLERO;
			ETSIDI::playMusica("sonidos/TABLERO.mp3", true);
		}
		break;

	case EstadoJuego::TABLERO:
		if (pTablerogl) {
			// NO DIBUJA EL TABLERO MIENTRAS LA IA ESTÁ CALCULANDO (evita corrupción visual)
			if (_iaCalculando) {
				// DIBUJA EL TABLERO NORMAL PERO SIN ACTUALIZARLO (solo lectura visual)
				DibujaTablero::tablero_dibujar(*pTablerogl);

				// OVERLAY SEMITRANSPARENTE CON MENSAJE
				Dibuja::util_entrar2D(_anchoVentana, _altoVentana);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0.0f, 0.0f, 0.0f, 0.45f); // OSCURECE UN POCO
				glBegin(GL_QUADS);
				glVertex2f(0, 0); glVertex2f(_anchoVentana, 0);
				glVertex2f(_anchoVentana, _altoVentana); glVertex2f(0, _altoVentana);
				glEnd();
				glDisable(GL_BLEND);
				ETSIDI::setFont("fuentes/ARIALNBI.ttf", 28);
				ETSIDI::setTextColor(0.9f, 0.8f, 0.2f, 1.0f);
				ETSIDI::printxy("La IA esta pensando...", _anchoVentana / 2 - 160, _altoVentana / 2);
				Dibuja::util_salir2D();
				break;
			}

			if (_necesitaRecargarGraficos && _framesCargando <= 0)
			{
				DibujaTablero::tablero_init(); // Recarga texturas
				_necesitaRecargarGraficos = false;
			}

			if (!_necesitaRecargarGraficos && _framesCargando <= 0)
			{
				ResultadoVictoria rv = gestorVictoria.comprobarVictoria(*pTablero);

			}

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_COLOR_MATERIAL);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(40.0, (float)_anchoVentana / (float)_altoVentana, 0.1, 150.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// LLAMADA CORRECTA AL MOTOR GRÁFICO (SIN FLECHITAS)
			if (_framesCargando > 0) {
				_framesCargando--;
				break;  // no dibuja nada hasta que pasen los frames
			}
			DibujaTablero::tablero_dibujar(*pTablerogl);


			if (pTablerogl->huboColision())
			{
				_pAtacanteCombate = pTablerogl->getPiezaAtacante();
				_pDefensoraCombate = pTablerogl->getPiezaDefensora();

				_filaAtacante = _pAtacanteCombate->getFila();
				_colAtacante = _pAtacanteCombate->getColumna();
				_filaDefensora = _pDefensoraCombate->getFila();
				_colDefensora = _pDefensoraCombate->getColumna();

				// P1 = SIEMPRE EL JUGADOR LOCAL, P2 = SIEMPRE EL RIVAL/IA
				bool atacanteEsLocal = (_pAtacanteCombate->getBando() == Bando::CRISTIANO);

				Pieza* pJugador = atacanteEsLocal ? _pAtacanteCombate : _pDefensoraCombate;
				Pieza* pRival = atacanteEsLocal ? _pDefensoraCombate : _pAtacanteCombate;

				_arena.iniciarCombate(*pJugador, *pRival,
					configuracion.modo,
					pTablerogl->getVentajaTerrenoCombate());

				ETSIDI::stopMusica();
				ETSIDI::play("sonidos/ARENA.mp3");
				DibujaArena::arena_configurar_vista(_anchoVentana, _altoVentana);
				DibujaArena::resetFlash();
				pTablerogl->limpiarCombate();
				pTablerogl->gestorTurnos.terminarTurno();
				estado = EstadoJuego::ARENA;
			}
		}
		break;

	case EstadoJuego::ARENA:
		DibujaArena::arena_dibujar(_arena, configuracion.batalla);
		break;

	case EstadoJuego::GUARDANDO:
		if (pTablerogl)
			DibujaTablero::tablero_guardando(*pTablerogl, _anchoVentana, _altoVentana, _tiempoGuardado);
		break;

	case EstadoJuego::CARGANDO:
		// Cargamos la partida y volvemos al tablero
		if (!pTablero) {
			pTablero = new Tablero();
			pTablerogl = new Tablerogl(pTablero);
			DibujaTablero::tablero_init();
			gestorInput.setTablerogl(pTablerogl);
			pGestorHechizos = new GestorHechizos(*pTablero,
				dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_local)),
				dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_rival)));
		}

		GestorPartida::cargar(*pTablero, pTablerogl->gestorTurnos, configuracion);
		pTablerogl->setBatalla((int)configuracion.batalla);
		pTablerogl->nombre_j1 = configuracion.nombre_j1; // RESTAURA NOMBRES AL CARGAR
		pTablerogl->nombre_j2 = configuracion.nombre_j2;

		// Recarga texturas después de cargar piezas
		_necesitaRecargarGraficos = true;

		ETSIDI::playMusica("sonidos/TABLERO.mp3", true);
		_framesCargando = 5;
		estado = EstadoJuego::TABLERO;
		break;

	case EstadoJuego::VICTORIA:
		DibujaMenu::victoria_dibujar(_anchoVentana, _altoVentana,
			_rankingGanador, _rankingBatalla, _rankingGanaJ1, _tiempoVictoria);
		break;

	case EstadoJuego::RANKING:
		DibujaMenu::ranking_dibujar(
			_anchoVentana, _altoVentana,
			_rankingGanador,
			_rankingBatalla,
			_rankingTurnos,
			_rankingPiezasLocal,
			_rankingPiezasRival,
			_rankingTop10,
			_rankingGanaJ1
		);
		break;

	case EstadoJuego::FINAL:
		if (pTablerogl) {
			DibujaTablero::tablero_dibujar(*pTablerogl);
			Dibuja::util_entrar2D(_anchoVentana, _altoVentana);
			DibujaTablero::tablero_victoria(*pTablerogl);
			Dibuja::util_salir2D();
		}
		break;

	case EstadoJuego::AYUDA:
		DibujaMenu::ayuda_dibujar(_ayudaSeleccion, _ayudaSeccion, _anchoVentana, _altoVentana);
		break;

	default: break;
	}
}

void Coordinador::tecla(unsigned char key)
{
	switch (estado) {

	case EstadoJuego::INTRO:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino);
		break;

	case EstadoJuego::MENU:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino);
		break;

	case EstadoJuego::DESTINO:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino);
		break;

	case EstadoJuego::TABLERO:
		if (key == 27) {
			if (pTablerogl->_conjuroElegido) {                // NIVEL 1: CANCELA CONJURO ELEGIDO
				pTablerogl->_conjuroElegido = false;
				pTablerogl->_esperandoDestino = false;
				pTablerogl->_mensajeInvalido = "";
				pTablerogl->_tiempoMensajeInvalido = 0.0f;
				std::cout << "[Hechizos] Conjuro cancelado. Elige otro 1-4.\n";
			}
			else if (pTablerogl->_modoHechizo) {            // NIVEL 2: CANCELA MODO HECHIZO
				pTablerogl->_modoHechizo = false;
				pTablerogl->_mensajeInvalido = "";
				pTablerogl->_tiempoMensajeInvalido = 0.0f;
				std::cout << "[Hechizos] Modo hechizo cancelado.\n";
			}
			else if (pTablerogl->piezaSeleccionada) {       // NIVEL 3: DESELECCIONA PIEZA
				pTablerogl->piezaSeleccionada = false;
				pTablerogl->fromFila = pTablerogl->fromCol = -1;
			}
			else {                                          // NIVEL 4: ABRE MENU PAUSA
				_tiempoGuardado = 10.0f;
				estado = EstadoJuego::GUARDANDO;
			}
			break;
		}
		gestorInput.teclaTablero(key, estado);
		break;

	case EstadoJuego::GUARDANDO:
		gestorInput.teclaGuardando(key, estado);
		break;

	case EstadoJuego::ARENA:
		gestorInput.teclaArena(key);
		break;

	case EstadoJuego::VICTORIA:
		_rankingTop10 = GestorRanking::cargar();
		estado = EstadoJuego::RANKING;
		break;

	case EstadoJuego::RANKING:
		if (key == 27) {
			//ETSIDI::stopMusica();
			ETSIDI::playMusica("sonidos/MENU.mp3", true);
			reiniciarTablero();
			estado = EstadoJuego::MENU;
		}
		break;

	case EstadoJuego::FINAL:
		// FINAL no tiene interacción de tecla, cae al default
		break;

	case EstadoJuego::AYUDA:
		if (key == 27) {
			if (_ayudaSeccion == -1)
				estado = EstadoJuego::GUARDANDO; // Vuelve a pausa
			else
				_ayudaSeccion = -1; // Vuelve al menu de ayuda
		}
		if (key == 13 && _ayudaSeccion == -1) {
			_ayudaSeccion = _ayudaSeleccion; // Entra en controles o normas
		}
		break;

	default:
		if (key == 27) {
			//ETSIDI::stopMusica();
			ETSIDI::playMusica("sonidos/MENU.mp3", true);
			menuPrincipal.reiniciar();
			reiniciarTablero();
		}
		estado = EstadoJuego::MENU;
		break;
	}

	glutPostRedisplay();
}

void Coordinador::tecla_up(unsigned char key)
{
	if (estado == EstadoJuego::ARENA)
		gestorInput.teclaUpArena(key);
}

void Coordinador::tecla_especial(int key)
{
	switch (estado) {
	case EstadoJuego::MENU:
		gestorInput.teclaEspecialMenu(key, estado, menuPrincipal);
		break;
	case EstadoJuego::TABLERO:
		gestorInput.teclaEspecialTablero(key);
		break;
	case EstadoJuego::ARENA:
		gestorInput.teclaEspecialArena(key);
		break;
	case EstadoJuego::GUARDANDO:
		gestorInput.teclaEspecialGuardando(key, estado);
		break;
	case EstadoJuego::AYUDA:
		if (_ayudaSeccion == -1)
		{
			if (key == GLUT_KEY_UP)   _ayudaSeleccion = (_ayudaSeleccion - 1 + 2) % 2;
			if (key == GLUT_KEY_DOWN) _ayudaSeleccion = (_ayudaSeleccion + 1) % 2;
		}
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::tecla_especial_up(int key)
{
	if (estado == EstadoJuego::ARENA)
		gestorInput.teclaEspecialUpArena(key);
}

void Coordinador::mueve(double dt)
{
	if (estado == EstadoJuego::TABLERO && pTablero && pTablerogl) {

		if (_framesCargando > 0) _framesCargando--;

		// IA: MUEVE SOLO UNA VEZ POR TURNO
		if (configuracion.modo == ModoJuego::JVIA &&
			pTablerogl->gestorTurnos.getBandoActual() == bando_rival &&
			!pTablerogl->huboColision() &&
			!_iaYaMovio) {

			int turnoActual = pTablerogl->gestorTurnos.getNumeroTurno();
			if (turnoActual != _turnoAnteriorIA) {
				_turnoAnteriorIA = turnoActual;
				_tiempoEsperaIA = 3.0f; // ESPERA 3 SEGUNDOS ANTES DE PENSAR
			}

			if (_tiempoEsperaIA > 0.0f) {
				_tiempoEsperaIA -= (float)dt; // CUENTA ATRÁS VISUAL
			}
			else if (!_iaCalculando) {
				// ── LANZA EL CÁLCULO EN UN HILO PARA NO CONGELAR EL JUEGO ──
				_iaCalculando = true;
				_iaTerminada = false;

				if (_hiloIA.joinable()) _hiloIA.join(); // LIMPIA HILO ANTERIOR

				_hiloIA = std::thread([this]() {
					// ESTE CÓDIGO CORRE EN PARALELO — EL RENDER SIGUE FUNCIONANDO
					MovimientoIA mov = _minimax.calcularMejorMovimiento(*pTablero);
					{
						std::lock_guard<std::mutex> lock(_mutexMovIA);
						_movimientoIA = mov; // GUARDA EL RESULTADO DE FORMA SEGURA
					}
					_iaTerminada = true; // SEÑALIZA QUE YA TERMINÓ
					});
			}

			// CUANDO EL HILO TERMINA, RECOGE EL RESULTADO Y EJECUTA EL MOVIMIENTO
			if (_iaCalculando && _iaTerminada) {
				if (_hiloIA.joinable()) _hiloIA.join();

				MovimientoIA mov;
				{
					std::lock_guard<std::mutex> lock(_mutexMovIA);
					mov = _movimientoIA; // LEE EL MOVIMIENTO CALCULADO
				}

				_iaCalculando = false;
				_iaYaMovio = true;

				if (mov.filaOrigen >= 0) {
					// CANCELA SI LA IA INTENTA PISAR A UN ALIADO (no debería pasar, pero por seguridad)
					if (pTablero->getCasilla(mov.filaDestino, mov.colDestino).bando == bando_rival) {
						std::cout << "[IA] Destino ocupado por aliado, cancelando.\n";
						_iaYaMovio = false;
					}
					else {
						Pieza* pieza = pTablero->getCasilla(mov.filaOrigen, mov.colOrigen).obj;
						if (pieza) {
							std::cout << "[IA] Moviendo (" << mov.filaOrigen << ","
								<< mov.colOrigen << ") -> (" << mov.filaDestino << ","
								<< mov.colDestino << ")\n";
							pTablerogl->Filacursor[1] = mov.filaDestino;
							pTablerogl->Colcursor[1] = mov.colDestino;
							pTablerogl->fromFila = mov.filaOrigen;
							pTablerogl->fromCol = mov.colOrigen;
							pTablerogl->fromBando = bando_rival;
							pTablerogl->piezaSeleccionada = true;
							pTablerogl->trySelectorMove(bando_rival); // EJECUTA EL MOVIMIENTO
						}
					}
				}
			}
		}

		// RESETEA CUANDO VUELVE EL TURNO LOCAL
		if (pTablerogl->gestorTurnos.getBandoActual() == bando_local) {
			_iaYaMovio = false;
			_iaCalculando = false; // POR SI EL TURNO CAMBIA MIENTRAS LA IA AÚN CALCULA
		}

		// ACTUALIZA TURNOS Y ANIMACIONES
		// SI NO HAY COLISIÓN Y LA PIEZA NO ESTÁ VIAJANDO GRÁFICAMENTE, AVANZAMOS EL TURNO DE VERDAD
		if (!pTablerogl->huboColision() && !pTablerogl->piezaSeleccionada) {

			// BLOQUEA INPUT DEL JUGADOR DURANTE EL TURNO DE LA IA
			if (configuracion.modo == ModoJuego::JVIA &&
				pTablerogl->gestorTurnos.getBandoActual() == bando_rival) {
				pTablerogl->piezaSeleccionada = false; // CANCELA CUALQUIER SELECCIÓN DEL JUGADOR
			}

			// SI NO HAY COLISIÓN Y LA PIEZA NO ESTÁ VIAJANDO GRÁFICAMENTE, AVANZAMOS EL TURNO DE VERDAD
			if (!pTablerogl->huboColision() && !pTablerogl->piezaSeleccionada) {
				pTablerogl->gestorTurnos.update(dt);
			}

		}
		// EL TABLEROGL Y LOS MENSAJES SÍ DEBEN ACTUALIZARSE SIEMPRE PARA DIBUJAR LA ANIMACIÓN CORRECTAMENTE
		pTablerogl->updateMensaje(dt);
		pTablerogl->update(dt);

		int turno = pTablerogl->gestorTurnos.getNumeroTurno();
		if (turno != pTablerogl->_turnosJugados &&
			turno > 1 && turno % Tablerogl::TURNOS_DINAMICOS == 0) {
			pTablerogl->_turnosJugados = turno;
			pTablerogl->aplicarCambiosDinamicos();
		}

		// COMPRUEBA VICTORIA
		if (!_necesitaRecargarGraficos)
		{
			ResultadoVictoria rv = gestorVictoria.comprobarVictoria(*pTablero);
			if (rv != ResultadoVictoria::SIN_GANADOR) {
				ETSIDI::stopMusica();

				// Ranking
				_rankingGanador = (rv == ResultadoVictoria::GANA_LOCAL) ? configuracion.nombre_j1 :
					(rv == ResultadoVictoria::GANA_RIVAL) ? configuracion.nombre_j2 : "Empate";
				_rankingGanaJ1 = (rv == ResultadoVictoria::GANA_LOCAL);
				_rankingBatalla = nombreBatalla(configuracion.batalla);
				_rankingTurnos = pTablerogl->gestorTurnos.getNumeroTurno();
				_rankingPiezasLocal = 16 - gestorVictoria.piezasVivas(*pTablero, bando_local);
				_rankingPiezasRival = 16 - gestorVictoria.piezasVivas(*pTablero, bando_rival);

				// Guardar en el Ranking
				GestorRanking::guardar(
					_rankingGanador,
					_rankingTurnos,
					_rankingBatalla,
					_rankingPiezasLocal + _rankingPiezasRival // total piezas eliminadas
				);

				// VICTORIA EN TABLEROGL (DE GABRI)
				if (rv == ResultadoVictoria::GANA_LOCAL)
					pTablerogl->setVictoria(bando_local);
				else if (rv == ResultadoVictoria::GANA_RIVAL)
					pTablerogl->setVictoria(bando_rival);

				_rankingTop10 = GestorRanking::cargar();
				_tiempoVictoria = 10.0f;
				estado = EstadoJuego::VICTORIA;
			}
		}
	}

	if (estado == EstadoJuego::GUARDANDO) {
		//_tiempoGuardado -= (float)dt;
		if (_tiempoGuardado <= 0.0f) {
			ETSIDI::stopMusica();
			reiniciarTablero();
			estado = EstadoJuego::MENU;
		}
	}

	if (estado == EstadoJuego::VICTORIA) {
		_tiempoVictoria -= (float)dt;
		if (_tiempoVictoria <= 0.0f) {
			_rankingTop10 = GestorRanking::cargar();
			estado = EstadoJuego::RANKING;
		}
	}

	if (estado == EstadoJuego::ARENA) {
		_arena.actualizar((float)dt, _input);
		DibujaArena::arena_update((float)dt);
	}
}

void Coordinador::raton(int boton, int state, int x, int y)
{
	bool pulsado = (state == GLUT_DOWN);
	if (!pulsado) return;

	switch (estado) {
	case EstadoJuego::INTRO:
	case EstadoJuego::DESTINO:
	case EstadoJuego::MENU:
		gestorInput.ratonMenu(boton, state, x, y, estado, pantallaIntro, menuPrincipal, pantallaDestino);
		break;
	case EstadoJuego::TABLERO:
	{
		int button;
		if (boton == GLUT_LEFT_BUTTON)       button = MOUSE_LEFT_BUTTON;
		else if (boton == GLUT_RIGHT_BUTTON) button = MOUSE_RIGHT_BUTTON;
		else                                 button = MOUSE_MIDDLE_BUTTON;

		int specialKey = glutGetModifiers();
		bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
		bool shiftKey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

		gestorInput.ratonTablero(x, y, button, (state == GLUT_DOWN), shiftKey, ctrlKey);
	}
	break;
	case EstadoJuego::GUARDANDO:
		gestorInput.ratonGuardando(x, y, true, estado);
		break;
	case EstadoJuego::AYUDA:
		gestorInput.ratonAyuda(boton, state, x, y, estado);
		/*if (boton == GLUT_LEFT_BUTTON && _ayudaSeccion == -1)
		{
			// Detecta clic en botones Controles/Normas
			int gy = _altoVentana - y;
			float btnW = 260, btnH = 48;
			float btnX = _anchoVentana / 2.0f - btnW / 2.0f;
			float btnY[2] = { _altoVentana / 2.0f + 20, _altoVentana / 2.0f - 50 };
			for (int i = 0; i < 2; i++)
			{
				if (gy >= btnY[i] && gy <= btnY[i] + btnH && x >= btnX && x <= btnX + btnW)
				{
					_ayudaSeccion = i;
				}
			}
		}*/
		
		break;

	default: break;

	}
	glutPostRedisplay();
}

void Coordinador::ratonMovido(int x, int y)
{
	gestorInput.setVentana(_anchoVentana, _altoVentana);
	gestorInput.ratonMovidoMenu(x, y, estado, menuPrincipal);
	if (estado == EstadoJuego::GUARDANDO)
		gestorInput.ratonMovidoGuardando(x, y); // HOVER PAUSA
	else if (estado == EstadoJuego::AYUDA)
		gestorInput.ratonMovidoAyuda(x, y, estado); // HOVER AYUDA
	glutPostRedisplay();
}

void Coordinador::redimensionar(int ancho, int alto)
{
	_anchoVentana = ancho;
	_altoVentana = (alto == 0) ? 1 : alto;
	glViewport(0, 0, _anchoVentana, _altoVentana);
	Tablerogl::setVentana(_anchoVentana, _altoVentana);
	DibujaArena::arena_configurar_vista(_anchoVentana, _altoVentana);
}

void Coordinador::reiniciarTablero()
{
	delete pTablerogl;
	delete pTablero;
	pTablero = nullptr;
	pTablerogl = nullptr;
	menuPrincipal.reiniciar();
}