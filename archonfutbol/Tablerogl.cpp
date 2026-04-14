// la implementacion del dibujo del tablero
#include "Tablerogl.h"
#include "ETSIDI.h"
#include <glut.h>
#include<cmath>
#include<string>
using namespace std;

//los colores fijos que no dependen del equipo
static const Color3f COLOR_ORO(242, 204, 13); //dorado VIP
static const Color3f COLOR_BLANCO_VIP(255,255,230);//blanco calido interior VIP
static const Color3f COLOR_SELECCION(26, 255, 102);//verde neon de selección
static const Color3f COLOR_NEGRO(0, 0, 0);
static const Color3f COLOR_BLANCO(242, 242, 242);
static const Color3f COLOR_GRIS_OSC(51, 51, 51); //el indicador de que la casilla, es una casilla dinamica

void TableroGL::init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float tamTotal = N * anchoCasilla;
	glOrtho(0, tamTotal, 0, tamTotal + 40, -1, 1);//+40 para el HUD

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TableroGL::Dibuja()
{
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int fila = 0; fila < N; fila++)
		for (int col = 0; col < N; col++)
			DibujarCasilla(fila, col);
	DibujarCuadricula();
	DibujaHUD(turnoActual, turnoDeQuien);
}

void TableroGL::DibujarCuadricula()
{
	float tamTotal = N * anchoCasilla;
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int i = 0; i <= N; i++) {
		if (i == 0 || i == N) glLineWidth(3.0f);
		else glLineWidth(0.8f);

		float pos = i * anchoCasilla;
		glBegin(GL_LINES);
			glVertex2f(0, pos);
			glVertex2f(tamTotal, pos);
			glVertex2f(pos,0);
			glVertex2f(pos, tamTotal);
		glEnd();
	}
	glLineWidth(1.0f);
}

void TableroGL::DibujarCasilla(int fila, int col)
{
	//vamos a acceder a la casilla por referencia 
	Casilla& casilla = m_tablero->getTabla()[fila][col];
	Color3f colorLocal = m_tablero->getEquipoLocal().getColorCasilla();
	Color3f colorVisitante = m_tablero->getEquipoVisitante().getColorCasilla();

	float px, float py;
	casilla2pixel(fila, col, px, py);

	//primero:color de fondo
	Color3f fondo(0, 0, 0);

	if (casilla.getZona() == PUNTO_PODER) fondo = COLOR_ORO;
	else {
		//vamos a elegir el color según lo que muestra ahora la casilla
		bool esLocal = (casilla.getColorActual() == COLOR_LOCAL);
		Color3f base = esLocal ? colorLocal : colorVisitante;

		//oscurecemos las casillas dinamicas con el metodo de la clase color3f
		fondo = (casilla.getZona() == DINAMICA) ? base.oscurecer(0.72f) : base;
	}

	//La seleccion sobreescribe el color de fondo
	if (casilla.isSeleccionada()) fondo = COLOR_SELECCION;

	dibujarRectangulo(px, py, anchoCasilla, anchoCasilla, fondo);

	//segundo:marcador dorado en zonas VIP
	if (casilla.getZona() == PUNTO_PODER) {
		float cx, cy;
		casilla2centro(fila, col, cx, cy);
		dibujarMarcadorVIP(cx, cy, anchoCasilla * 0.28f);
	}

	//tercero:un cuadradito oscuro en equina de la casilla dinamica para indicarle al jugador que esa casilla va a cambiar de color
	if (casilla.getZona() == DINAMICA && !casilla.isSeleccionada()) {
		dibujarRectangulo(px + anchoCasilla - 11.f, py + anchoCasilla - 11.f, 8.f, 8.f, COLOR_GRIS_OSC, 0.60f);
	}

	//cuarto:pieza encima de la casilla (de momento solo un circulo)
	if (casilla.getOcupadaPor() != NADIE) {
		float cx, cy;
		casilla2centro(fila, col, cx, cy);

		Color3f colorPieza = (casilla.getOcupadaPor() == LOCAL) ? m_tablero->getEquipoLocal().getColorPieza() : m_tablero->getEquipoVisitante().getColorPieza();
		dibujarCirculo(cx, cy, anchoCasilla * 0.30f, colorPieza, 0.92f);
		dibujarBorde(px + anchoCasilla * 0.20f, py + anchoCasilla * 0.20f, anchoCasilla * 0.60f, anchoCasilla * 0.60f, COLOR_BLANCO, 1.5f);
	}

	//quinto: que aparezca un borde verde grueso si esta seleccionada
	if (casilla.isSeleccionada()) {
		//convertimos de 0-255 a 0.0-1.0 para OpenGl
		glColor4f(COLOR_SELECCION.getRf(), COLOR_SELECCION.getGf(), COLOR_SELECCION.getBf(), 1.0f);
		glLineWidth(4.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(px + 2, py + 2);
		glVertex2f(px + anchoCasilla - 2, py + 2);
		glVertex2f(px + anchoCasilla - 2, py + anchoCasilla - 2);
		glVertex2f(px + 2, py + anchoCasilla - 2);
		glEnd();
		glLineWidth(1.0f);
	}
}

void TableroGL::DibujaHUD(int turno, TipoOcupante turnoDeQuien)
{
	Color3f colorTexto=(turnoDeQuien==LOCAL)? m_tablero->getEquipoLocal().getColorCasilla() : m_tablero->getEquipoVisitante().getColorCasilla();
	string nombreEquipo = (turnoDeQuien == LOCAL)? m_tablero->getEquipoLocal().getNombre() : m_tablero->getEquipoVisitante().getNombre();
}







