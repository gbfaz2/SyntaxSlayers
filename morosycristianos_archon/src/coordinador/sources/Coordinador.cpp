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


Coordinador::~Coordinador()
{
	delete pTablerogl; // LIBERA TABLEROGL
	delete pTablero;   // LIBERA TABLERO
	delete pGestorHechizos;
}

void Coordinador::inicializa()
{
	srand((unsigned)time(nullptr));
	pantallaIntro.reiniciar();
	menuPrincipal.reiniciar();
	DibujaArena::arena_configurar_vista(_anchoVentana, _altoVentana); // CONFIGURA CAMARA ARENA
	estado = EstadoJuego::INTRO; // ARRANCA EN INTRO
	gestorInput.setCoordinador(this); // ASIGNA COORDINADOR AL GESTOR
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

			if (siguiente == EstadoJuego::FINAL) exit(0);

			if (siguiente == EstadoJuego::DESTINO) {
				pantallaDestino.reiniciar(configuracion);
				if (!pTablero) { // SOLO CREA EL TABLERO UNA VEZ
					pTablero = new Tablero();
					pTablerogl = new Tablerogl(pTablero);
					DibujaTablero::tablero_init();
					pTablerogl->setBatalla((int)configuracion.batalla); // ASIGNA BATALLA AL TABLERO
					BandoPieza bandoInicial = (configuracion.turno1 == BandoJugador::MUSULMAN)? bando_rival : bando_local;//FIJAMOS QUIEN EMPIEZA SEGÚN LA BATALLA SELECCIONADA
					pTablerogl->setBandoInicial(bandoInicial);

					gestorInput.setTablerogl(pTablerogl); // ASIGNA TABLEROGL AL GESTOR

					pGestorHechizos = new GestorHechizos(*pTablero,
						dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_local)),
						dynamic_cast<Hechicero*>(pTablero->buscarPieza(pieza_esfera, bando_rival)));
				}
			}
			estado = siguiente;
		}
		break;

	case EstadoJuego::DESTINO:
		// NUEVO MOTOR GRAFICO CENTRALIZADO GESTIONA ESTA TRANSICION AL COMPLETO
		DibujaMenu::destino_dibujar(pantallaDestino, _anchoVentana, _altoVentana); // PINTA EFECTOS, FONDOS Y PARTICULAS

		if (pantallaDestino.terminado()) {
			estado = EstadoJuego::TABLERO;
			ETSIDI::playMusica("sonidos/sonido_fondo_tablero.wav", true);
		}
		break;

	case EstadoJuego::TABLERO:
		if (pTablerogl) {
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
			DibujaTablero::tablero_dibujar(*pTablerogl);

			if (pTablerogl->huboColision())
			{
				std::cout << "[DEBUG] Colision detectada, entrando a arena\n"; // DEBUG
				
				_pAtacanteCombate = pTablerogl->getPiezaAtacante();
				_pDefensoraCombate = pTablerogl->getPiezaDefensora();

				_arena.iniciarCombate(*pTablerogl->getPiezaAtacante(),
					*pTablerogl->getPiezaDefensora(),
					configuracion.modo);

				ETSIDI::stopMusica(); // DEJA DE SONAR MUSICA TABLERO
				ETSIDI::play("sonidos/sonido_combate_fight.wav");
				DibujaArena::arena_configurar_vista(_anchoVentana, _altoVentana);
				pTablerogl->limpiarCombate();
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
		ETSIDI::playMusica("sonidos/sonido_fondo_tablero.wav", true);
		estado = EstadoJuego::TABLERO;
		break;

	case EstadoJuego::RANKING:
		break;

	case EstadoJuego::FINAL:
		if (pTablerogl) {
			DibujaTablero::tablero_dibujar(*pTablerogl);
			Dibuja::util_entrar2D(_anchoVentana, _altoVentana);
			DibujaTablero::tablero_victoria(*pTablerogl);
			Dibuja::util_salir2D();
		}
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
				std::cout << "[Hechizos] Conjuro cancelado. Elige otro 1-7.\n";
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
		if (key == 'g' || key == 'G') {
			GestorPartida::guardar(*pTablero, pTablerogl->gestorTurnos, configuracion);
			ETSIDI::stopMusica();
			reiniciarTablero();
			estado = EstadoJuego::MENU;
		}
		if (key == 27) {
			estado = EstadoJuego::TABLERO;
		}
		break;

	case EstadoJuego::ARENA:
		gestorInput.teclaArena(key);
		break;

	default:
		if (key == 27) {
			ETSIDI::stopMusica();
			menuPrincipal.reiniciar();
			reiniciarTablero();
		}
		estado = EstadoJuego::MENU; break;
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
	if (estado == EstadoJuego::TABLERO && pTablero) {
		//USAMOS EL GESTORTURNOS DENTRO DE PTABLEROGL
		if (pTablerogl) {
			pTablerogl->gestorTurnos.update(dt);
			pTablerogl->updateMensaje(dt);
			int turno = pTablerogl->gestorTurnos.getNumeroTurno();
			if (turno != pTablerogl->_turnosJugados &&
				turno > 1 && turno % Tablerogl::TURNOS_DINAMICOS == 0) {
				pTablerogl->_turnosJugados = turno;
				pTablerogl->aplicarCambiosDinamicos();
			}
		}
		//gestorTurnos.update(dt);

		//ACTUALIZACIÓN DEL TEMPORIZADOR DE MOVIMIENTO INVÁLIDO
		//if (pTablerogl)pTablerogl->updateMensaje(dt);
		//CASILLAS DINÁMICAS
		/*if (pTablerogl) {
			int turnoActual = pTablerogl->gestorTurnos.getNumeroTurno();
			if (turnoActual != pTablerogl->_turnosJugados &&
				turnoActual > 1 &&
				turnoActual % Tablerogl::TURNOS_DINAMICOS == 0) {
				pTablerogl->_turnosJugados = turnoActual;
				pTablerogl->aplicarCambiosDinamicos();
			}
		}*/

		if (pTablero == nullptr) return;

		ResultadoVictoria rv = gestorVictoria.comprobarVictoria(*pTablero);
		if (rv != ResultadoVictoria::SIN_GANADOR) {
			ETSIDI::stopMusica();
			if (pTablerogl) {
				if (rv == ResultadoVictoria::GANA_LOCAL)
					pTablerogl->setVictoria(bando_local);
				else if (rv == ResultadoVictoria::GANA_RIVAL)
					pTablerogl->setVictoria(bando_rival);
			}
			estado = EstadoJuego::FINAL;
		}
	}

	if (estado == EstadoJuego::GUARDANDO) 
	{
		_tiempoGuardado -= (float)dt;
		if (_tiempoGuardado <= 0.0f) {
			ETSIDI::stopMusica();
			reiniciarTablero();
			estado = EstadoJuego::MENU;
		}
	}

	_spriteReyLocal.update(dt);

	if (_spriteReyLocal.animacionTerminada() && _spriteReyLocal.getEstado() != EstadoRey::DEATH)
		_spriteReyLocal.setEstado(EstadoRey::IDLE);

	if (estado == EstadoJuego::ARENA)
		_arena.actualizar((float)dt, _input);
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
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::ratonMovido(int x, int y)
{
	gestorInput.setVentana(_anchoVentana, _altoVentana);
	gestorInput.ratonMovidoMenu(x, y, estado, menuPrincipal);
	glutPostRedisplay();
}

void Coordinador::redimensionar(int ancho, int alto)
{
	_anchoVentana = ancho;
	_altoVentana = (alto == 0) ? 1 : alto;
	glViewport(0, 0, _anchoVentana, _altoVentana);
	Tablerogl::setVentana(_anchoVentana, _altoVentana);
}

void Coordinador::reiniciarTablero()
{
	delete pTablerogl;
	delete pTablero;
	pTablero = nullptr;
	pTablerogl = nullptr;
	menuPrincipal.reiniciar();
}