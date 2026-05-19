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
	gestorInput.setCoordinador(this); // ASIGNA COORDINADOR AL GESTOR
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
		gestorInput.teclaArena(key); // GESTOR TECLAS ARENA
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
	if (estado == EstadoJuego::ARENA)
		gestorInput.teclaUpArena(key); // GESTOR TECLAS UP ARENA
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
		gestorInput.teclaEspecialArena(key); // GESTOR FLECHAS ARENA
		break;
	default: break;
	}
	glutPostRedisplay();
}

void Coordinador::tecla_especial_up(int key)
{
	if (estado == EstadoJuego::ARENA)
		gestorInput.teclaEspecialUpArena(key); // GESTOR FLECHAS UP ARENA
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