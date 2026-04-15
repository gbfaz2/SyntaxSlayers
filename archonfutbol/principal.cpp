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
			ETSIDI::setFont("arial.ttf", 13);
			ETSIDI::setTextColor(1.0f, 0.4f, 0.4f);
			ETSIDI::printxy("Los dos equipos no pueden ser el mismo", 360, 360);
		}else{
			//Indica que faltan elecciones 
			ETSIDI::setFont("arial.ttf", 13);
			ETSIDI::setTextColor(0.6f, 0.6f, 0.6f);
			if (!localElegido && !visitanteElegido)
				ETSIDI::printxy("Elige ambos equipos para continuar", 360, 360);
			else if (!localElegido)
				ETSIDI::printxy("Falta elegir el equipo LOCAL [1-BARSA / 2-ATLETICO /3-MADRID]", 360, 360);
			else
				ETSIDI::printxy("Falta elegir el equipo VISITANTE [4-BARSA / 5-ATLETICO /6-MADRID]", 360, 360);
		}

		//ENTER solo estará disponible cuando ambos equipos estan elegidos y son distintos
		bool puedeEmpezar = localElegido && visitanteElegido && idLocal != idVisitante;
		if (puedeEmpezar) {
			ETSIDI::setFont("arial.ttf", 18);
			ETSIDI::setTextColor(0.20f, 0.90f, 0.30f);
			ETSIDI::printxy("[ENTER] Empezar partida", 360, 100);
		}
		else {
			ETSIDI::setFont("arial.ttf", 18);
			ETSIDI::setTextColor(0.35f, 0.35f, 0.35f);//gris:significa que no esta disponible aun
			ETSIDI::printxy("[ENTER] Empezar partida", 360, 100);
		}
	}

	
}

void OnDibujar(void)
{
	if (fase == SELECCIONANDO)
		dibujarSelector();
	else
		escena->Dibujar();

	glutSwapBuffers();
}

void OnTeclado(unsigned char tecla, int x, int y)
{
	if (fase == SELECCIONANDO) {
		//Teclas 1-3: eligen el equipo local
		if (tecla == '1') { idLocal = BARSA; localElegido = true; }
		if (tecla == '2') { idLocal = ATLETICO; localElegido = true; }
		if (tecla == '3') { idLocal = MADRID; localElegido = true; }

		//Teclas 4-6: eligen el equipo visitante
		if (tecla == '4') { idLocal = BARSA; visitanteElegido = true; }
		if (tecla == '5') { idLocal = ATLETICO; visitanteElegido = true; }
		if (tecla == '6') { idLocal = MADRID; visitanteElegido = true; }

		//ENTER: solo si ambos estan elegidos y son distintos
		if (tecla == 13 && localElegido && visitanteElegido && idLocal != idVisitante) {
			tablero = new Tablero(idLocal, idVisitante);
			escena = new TableroGL(tablero);
			escena->init();

			//Piezas de prueba hasta que se implemente la clase Pieza
			for (int fila = 0; fila < TAMANO_TABLERO; fila++) {
				if (fila % 2 == 0) {
					tablero->establecerOcupante(fila, 0, LOCAL);
					tablero->establecerOcupante(fila, 8, VISITANTE);
				}
			}
			fase = JUGANDO;

		}
	}
	else {
		escena->KeyDown(tecla);
		
		if (tecla == 27) { //ESC para volver al selector
			delete escena;
			delete tablero;
			escena = nullptr;
			tablero = nullptr;
			localElegido = false;
			visitanteElegido = false;
			fase = SELECCIONANDO;
		}
	}
	glutPostRedisplay();
}

void OnRaton(int boton, int estado, int x, int y)
{
	if (fase != JUGANDO || !escena)return;

	bool pulsado = (estado == GLUT_DOWN);
	int btn;
	if (boton == GLUT_LEFT_BUTTON)btn = BOTON_IZQ;
	else if (boton == GLUT_RIGHT_BUTTON)btn = BOTON_DER;
	else btn = BOTON_MED;

	int specialKey = glutGetModifiers();
	bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
	bool sKey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

	escena->BotonRaton(x, y, btn, pulsado, sKey, ctrlKey);
	glutPostRedisplay();
}
