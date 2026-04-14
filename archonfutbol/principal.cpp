// archonfutbol.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//controles pantalla de seleccion: 
// 1/2/3 se elige el equipo LOCAL (Barsa / Atletico / Madrid)
// 4/5/6 equipo VISITANTE
// ENTER empezar la partida

//Controles de la partida:
//  Clic izq: seleccionar pieza / moverla
// ESC:  volver a selección
#include <iostream>
#include "Tablerogl.h"
#include "ETSIDI.h"
#include <glut.h>
using namespace std;

void OnDibujar(void);
void OnTeclado(unsigned char tecla, int x, int y);
void OnRaton(int boton, int estado, int x, int y);

//el estado global
enum FaseJuego{SELECCIONANDO, JUGANDO};

//ENTER solo funciona si ambos flags son true y los equipos son distintos
FaseJuego fase = SELECCIONANDO;
bool localElegido = false;//true cuando el jugador pulsa 1,2 o 3
bool visitanteElegido = false;//true cuando el jugador pulsa 4,5 o 6
TipoEquipo idLocal;//no se inicializa hasta que el jugador elige
TipoEquipo idVisitante;//no se inicializa hasta que el jugador elige

//punteros al tablero y la escena que se crean al pulsar ENTER
Tablero* tablero = nullptr;
Tablero* escena = nullptr;

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(720, 760);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Archon: Los Clasicos");

	glutDisplayFunc(OnDibujar);
	glutKeyboardFunc(OnTeclado);
	glutMouseFunc(OnRaton);

	glutMainLoop();
	return 0;
}

//Pantalla de seleccion de equipos
void dibujarBotonEquipo(int cx, int cy, const char* tecla, ConfigEquipo cfg, bool seleccionado) {
	Color3f c = cfg.getColorCasilla();

	if (seleccionado) {
		glColor4f(c.getRf(), c.getGf(), c.getBf(), 0.28f);
		glBegin(GL_QUADS);
		glVertex2f(cx - 95, cy - 18); glVertex2f(cx + 95, cy - 18);
		glVertex2f(cx + 95, cy + 18); glVertex2f(cx - 95, cy + 18);
		glEnd();
		glColor4f(c.getRf(), c.getGf(), c.getBf(), 1.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(cx - 95, cy - 18); glVertex2f(cx + 95, cy - 18);
		glVertex2f(cx + 95, cy + 18); glVertex2f(cx - 95, cy + 18);
		glEnd();
		glLineWidth(1.0f);
	}

	ETSIDI::setFont("arial.ttf", 14);
	ETSIDI::setJustificacion(ETSIDI::CENTRO, ETSIDI::MEDIO);
	ETSIDI::setTextColor(c.getRf(), c.getGf(), c.getBf());
	string txt = string("[") + tecla + "]" + cfg.getNombre();
	ETSIDI::printxy(txt.c_str(), cx, cy);
}

void dibujarSelector() {
	glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 720, 0, 760, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//EL TITULO
	ETSIDI::setFont("arial.ttf", 28);
	ETSIDI::setTextColor(1, 1, 1);
	ETSIDI::setJustificacion(ETSIDI::CENTRO, ETSIDI::MEDIO);
	ETSIDI::printxy("ARCHON: LOS CLASICOS FUTBOLEROS", 360, 710);

	ETSIDI::setFont("arial.ttf", 14);
	ETSIDI::setTextColor(0.75f, 0.75f, 0.75f);
	ETSIDI::printxy("Elige los equipos y pulsa ENTER", 360, 675);

	//EQUIPO LOCAL
	ETSIDI::setFont("arial.ttf", 16);
	ETSIDI::setTextColor(0.40f, 0.90f, 0.50f);
	ETSIDI::printxy("EQUIPO LOCAL: [1-BARSA / 2-ATLETICO /3-MADRID]", 360, 360);

	//el tercer argumento (seleccionado) solo es true si el jugador ya ha elegido y el equipo elegido coincide con el de ese boton
	dibujarBotonEquipo(150, 580, "1", ConfigEquipo::crear(BARSA), localElegido && idLocal == BARSA);
	dibujarBotonEquipo(360, 580, "2", ConfigEquipo::crear(ATLETICO), localElegido && idLocal == ATLETICO);
	dibujarBotonEquipo(570, 580, "3", ConfigEquipo::crear(MADRID), localElegido && idLocal == MADRID);

	//EQUIPO VISITANTE
	ETSIDI::setFont("arial.ttf", 16);
	ETSIDI::setTextColor(0.90f, 0.40f, 0.40f);
	ETSIDI::printxy("EQUIPO LOCAL: [4-BARSA / 5-ATLETICO /6-MADRID]", 360, 490);

	//el tercer argumento (seleccionado) solo es true si el jugador ya ha elegido y el equipo elegido coincide con el de ese boton
	dibujarBotonEquipo(150, 450, "4", ConfigEquipo::crear(BARSA), visitanteElegido && idVisitante == BARSA);
	dibujarBotonEquipo(360, 450, "5", ConfigEquipo::crear(ATLETICO), visitanteElegido && idVisitante == ATLETICO);
	dibujarBotonEquipo(570, 450, "6", ConfigEquipo::crear(MADRID), visitanteElegido && idVisitante == MADRID);

	//Resumen que solo se muestra cuando ambos han elegido
	if (localElegido && visitanteElegido) {
		string resumen = ConfigEquipo::crear(idLocal).getNombre() + " vs " + ConfigEquipo::crear(idVisitante).getNombre();

		ETSIDI::setFont("arial.ttf", 15);
		ETSIDI::setTextColor(1.0f, 1.0f, 0.5f);
		ETSIDI::printxy(resumen.c_str(),360,360);

		//Aviso de que se han elegido los dos equipos iguales 
		if (idLocal == idVisitante) {

		}
	}
}
