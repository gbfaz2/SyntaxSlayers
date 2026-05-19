#include "Coordinador.h"
#include "freeglut.h"
#include <ctime>
#include <cstdlib>

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
	ArenaRenderer::configurarVista(anchoVentana, altoVentana); // CONFIGURA CAMARA ARENA
	estado = EstadoJuego::INTRO; // ARRANCA EN INTRO
}

void Coordinador::dibuja()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (estado) {

	case EstadoJuego::INTRO:
		entrar2D(anchoVentana, altoVentana);
		pantallaIntro.dibujar(anchoVentana, altoVentana);
		salir2D();

		if (pantallaIntro.terminado()) {
			pantallaIntro.reiniciar();
			menuPrincipal.reiniciar();
			estado = EstadoJuego::MENU;
		}
		break;

	case EstadoJuego::MENU:
		entrar2D(anchoVentana, altoVentana);
		menuPrincipal.dibujar(anchoVentana, altoVentana);
		salir2D();

		if (menuPrincipal.terminado()) {
			EstadoJuego siguiente = menuPrincipal.siguienteEstado();
			configuracion = menuPrincipal.getConfiguracion();

			if (siguiente == EstadoJuego::FINAL) exit(0);

			if (siguiente == EstadoJuego::DESTINO) {
				pantallaDestino.reiniciar(configuracion);
				if (!pTablero) { // SOLO CREA EL TABLERO UNA VEZ
					pTablero = new Tablero();
					pTablerogl = new Tablerogl(pTablero);
					pTablerogl->init();
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
		entrar2D(anchoVentana, altoVentana);
		pantallaDestino.dibujar(anchoVentana, altoVentana);
		salir2D();

		if (pantallaDestino.terminado()) {
			estado = EstadoJuego::TABLERO;
			ETSIDI::playMusica("sonido_fondo_tablero.wav", true);
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
			gluPerspective(40.0, (float)anchoVentana / (float)altoVentana, 0.1, 150.0);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			pTablerogl->Dibuja();

			// TODO: CUANDO MARIA IMPLEMENTE huboColision()
			if (pTablerogl->huboColision()) 
			{
				_arena.iniciarCombate(*pTablerogl->getPiezaAtacante(),
					*pTablerogl->getPiezaDefensora(),
					configuracion.modo); // INICIA COMBATE CON LAS PIEZAS SELECCIONADAS EN EL MENU Y EL MODO DE JUEGO (JVJ O JVIA)

				ETSIDI::stopMusica(); //colision: deja de sonar musica tablero
				ETSIDI::play("sonido_combate_fight.wav");
				ArenaRenderer::configurarVista(anchoVentana, altoVentana);
				pTablerogl->limpiarCombate();
				estado = EstadoJuego::ARENA;
			}
		}
		break;

	case EstadoJuego::ARENA:
		ArenaRenderer::dibujar(_arena);
			// SOLO VOLVER AL TABLERO CUANDO EL JUGADOR TOQUE UNA TECLA, PARA DAR TIEMPO A VER EL RESULTADO DEL COMBATE
			// NO HACER NADA AQUÍ, SOLO ESPERAR A QUE EL JUGADOR PULSE UNA TECLA PARA VOLVER AL TABLERO
		break;

	case EstadoJuego::RANKING:
		break;

	case EstadoJuego::FINAL:
		break;

	default: break;
	}
}

void Coordinador::tecla(unsigned char key)
{
	switch (estado) {
	case EstadoJuego::INTRO:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino); // GESTOR INTRO
		break;
	case EstadoJuego::MENU:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino); // GESTOR MENU
		break;
	case EstadoJuego::DESTINO:
		gestorInput.teclaMenu(key, estado, pantallaIntro, menuPrincipal, pantallaDestino); // GESTOR DESTINO
		break;
	case EstadoJuego::TABLERO:
		if (key == 27) {
			ETSIDI::stopMusica();
			reiniciarTablero();
			estado = EstadoJuego::MENU;
			break;
		}
		gestorInput.teclaTablero(key, estado); // GESTOR TECLAS TABLERO
		break;
	case EstadoJuego::ARENA:

		// CONTROLES P1: WASD + F
		if (key == 'w' || key == 'W') _input.p1.delante = true;
		if (key == 's' || key == 'S') _input.p1.atras = true;
		if (key == 'a' || key == 'A') _input.p1.izquierda = true;
		if (key == 'd' || key == 'D') _input.p1.derecha = true;
		if (key == 'f' || key == 'F') _input.p1.atacar = true;

		// CONTROLES P2 (solo en JvJ): flechas + L
		if (configuracion.modo == ModoJuego::JVJ) {
			// Las flechas van por tecla_especial, aquí solo el ataque
			if (key == 'l' || key == 'L') _input.p2.atacar = true;
		}

		// ENTER o ESC VUELVE AL TABLERO DESDE LA ARENA CUANDO EL COMBATE HAYA TERMINADO (GANE P1, GANE P2 O EMPATE)
		if ((key == 13 || key == 27) && _arena.resultado() != ResultadoCombate::EnCurso) {
			ETSIDI::playMusica("sonido_fondo_tablero.wav", true);
			estado = EstadoJuego::TABLERO;
		}
		break;

	default:
		if (key == 27) {
			ETSIDI::stopMusica();
			menuPrincipal.reiniciar();
			reiniciarTablero(); // REINICIA TABLERO AL VOLVER AL MENU
		}
			estado = EstadoJuego::MENU; break;
	}
	glutPostRedisplay();
}

void Coordinador::tecla_up(unsigned char key)
{
	if (estado == EstadoJuego::ARENA) {
		// SUELTA TECLAS P1
		if (key == 'w' || key == 'W') _input.p1.delante = false;
		if (key == 's' || key == 'S') _input.p1.atras = false;
		if (key == 'a' || key == 'A') _input.p1.izquierda = false;
		if (key == 'd' || key == 'D') _input.p1.derecha = false;
	}
}

void Coordinador::tecla_especial(int key)
{
	switch (estado) {
	case EstadoJuego::MENU:
		gestorInput.teclaEspecialMenu(key, estado, menuPrincipal); // GESTOR FLECHAS MENU
		break;
	case EstadoJuego::TABLERO:
		gestorInput.teclaEspecialTablero(key); // GESTOR FLECHAS TABLERO
		break;
	case EstadoJuego::ARENA:
		if (configuracion.modo == ModoJuego::JVJ) {
			if (key == GLUT_KEY_UP)    _input.p2.delante = true;
			if (key == GLUT_KEY_DOWN)  _input.p2.atras = true;
			if (key == GLUT_KEY_LEFT)  _input.p2.izquierda = true;
			if (key == GLUT_KEY_RIGHT) _input.p2.derecha = true;
		}
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::tecla_especial_up(int key)
{
	if (estado == EstadoJuego::ARENA && configuracion.modo == ModoJuego::JVJ) {
		if (key == GLUT_KEY_UP)    _input.p2.delante = false;
		if (key == GLUT_KEY_DOWN)  _input.p2.atras = false;
		if (key == GLUT_KEY_LEFT)  _input.p2.izquierda = false;
		if (key == GLUT_KEY_RIGHT) _input.p2.derecha = false;
	}
}

void Coordinador::mueve(double dt)
{
	//actualiza gestor turnos por dt
	if (estado == EstadoJuego::TABLERO && pTablero) {
		// Actualiza el cronómetro del turno
		gestorTurnos.update(dt);

		// Comprueba si alguien ha ganado
		ResultadoVictoria rv = gestorVictoria.comprobarVictoria(*pTablero);
		if (rv != ResultadoVictoria::SIN_GANADOR) {
			ETSIDI::stopMusica();
			estado = EstadoJuego::FINAL;
		}
	}
	
	if (estado == EstadoJuego::ARENA)
		_arena.actualizar((float)dt, _input); // AVANZA LA LOGICA DE LA ARENA
}

void Coordinador::raton(int boton, int state, int x, int y)
{
	bool pulsado = (state == GLUT_DOWN);
	if (!pulsado) return;

	switch (estado) {
	case EstadoJuego::INTRO:
	case EstadoJuego::DESTINO:
	case EstadoJuego::MENU:
		gestorInput.ratonMenu(boton, state, x, y, estado, pantallaIntro, menuPrincipal, pantallaDestino); // GESTOR RATON
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

		gestorInput.ratonTablero(x, y, button, (state == GLUT_DOWN), shiftKey, ctrlKey); // GESTOR RATON TABLERO
	}
	break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::ratonMovido(int x, int y)
{
	gestorInput.setVentana(anchoVentana, altoVentana); // ACTUALIZA TAMAÑO
	gestorInput.ratonMovidoMenu(x, y, estado, menuPrincipal); // GESTOR HOVER
	glutPostRedisplay();
}

void Coordinador::redimensionar(int ancho, int alto)
{
	anchoVentana = ancho;
	altoVentana = (alto == 0) ? 1 : alto; // EVITA DIVISION POR CERO
	glViewport(0, 0, anchoVentana, altoVentana);
}

void Coordinador::reiniciarTablero()
{
	delete pTablerogl; // DESTRUYE TABLEROGL
	delete pTablero;   // DESTRUYE TABLERO
	pTablero = nullptr;   // RESETEA PUNTERO
	pTablerogl = nullptr; // RESETEA PUNTERO
	menuPrincipal.reiniciar(); // RESETEA EL MENU TAMBIEN
}