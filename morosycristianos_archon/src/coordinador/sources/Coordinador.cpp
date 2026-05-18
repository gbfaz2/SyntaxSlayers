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

		if (pantallaDestino.terminado())
			estado = EstadoJuego::TABLERO;
		    ETSIDI::playMusica("sonido_fondo_tablero.wav", true);//bucle sonido de fondo durante tablero
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
			    _arena.iniciarCombate(*pTablerogl->getPiezaAtacante(), *pTablerogl->getPiezaDefensora());
				ETSIDI::stopMusica(); //colision: deja de sonar musica tablero
				ArenaRenderer::configurarVista(anchoVentana, altoVentana);
				pTablerogl->limpiarCombate();
				estado = EstadoJuego::ARENA;
			}
		}
		break;

	case EstadoJuego::ARENA:
		ArenaRenderer::dibujar(_arena); // PINTA EL FRAME DE LA ARENA
		if (_arena.resultado() != ResultadoCombate::EnCurso)
			estado = EstadoJuego::TABLERO; // VUELVE AL TABLERO AL TERMINAR
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
		pantallaIntro.saltar();
		break;
	case EstadoJuego::MENU:
		menuPrincipal.teclaPulsada(key);
		break;
	case EstadoJuego::DESTINO:
		pantallaDestino.avanzar();
		break;
	case EstadoJuego::TABLERO:
		if (key == 27) { 
			ETSIDI::stopMusica(); 
			menuPrincipal.reiniciar(); 
			estado = EstadoJuego::MENU; break; 
		}
		if (pTablerogl) pTablerogl->KeyDown(key);
		break;
	case EstadoJuego::ARENA:
		if (key == 'r' || key == 'R') _arena.reiniciar(); // REINICIA COMBATE
		// CONTROLES P1: WASD + F
		if (key == 'w' || key == 'W') _input.p1.delante = true;
		if (key == 's' || key == 'S') _input.p1.atras = true;
		if (key == 'a' || key == 'A') _input.p1.izquierda = true;
		if (key == 'd' || key == 'D') _input.p1.derecha = true;
		if (key == 'f' || key == 'F') _input.p1.atacar = true;
		break;

	default:
		if (key == 27) { ETSIDI::stopMusica(); menuPrincipal.reiniciar(); estado = EstadoJuego::MENU; }
		break;
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
		menuPrincipal.teclaEspecial(key);
		break;
	case EstadoJuego::TABLERO:
		if (pTablerogl) pTablerogl->SpecialKey(key);
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::tecla_especial_up(int key)
{
	// VACIO: P2 LO CONTROLA LA IA
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
		pantallaIntro.saltar();
		break;
	case EstadoJuego::DESTINO:
		pantallaDestino.avanzar();
		break;
	case EstadoJuego::MENU:
		if (boton == GLUT_LEFT_BUTTON)
			menuPrincipal.ratonPulsado(x, y, anchoVentana, altoVentana);
		break;
	case EstadoJuego::TABLERO:
		if (pTablerogl) {
			int button;
			if (boton == GLUT_LEFT_BUTTON)       button = MOUSE_LEFT_BUTTON;
			else if (boton == GLUT_RIGHT_BUTTON) button = MOUSE_RIGHT_BUTTON;
			else                                 button = MOUSE_MIDDLE_BUTTON;

			int specialKey = glutGetModifiers();
			bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
			bool shiftKey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

			pTablerogl->MouseButton(x, y, button, true, shiftKey, ctrlKey);
		}
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::ratonMovido(int x, int y)
{
	if (estado == EstadoJuego::MENU)
		menuPrincipal.ratonMovido(x, y, anchoVentana, altoVentana);
	glutPostRedisplay();
}

void Coordinador::redimensionar(int ancho, int alto)
{
	anchoVentana = ancho;
	altoVentana = (alto == 0) ? 1 : alto; // EVITA DIVISION POR CERO
	glViewport(0, 0, anchoVentana, altoVentana);
}