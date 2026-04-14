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

	float tamTablero = N * anchoCasilla;
	dibujarRectangulo(0, tamTablero, tamTablero, 40, COLOR_NEGRO, 0.85f);

	ETSIDI::setFont("arial.ttf", 140);
	ETSIDI::setJustificacion(ETSIDI::IZQUIERDA, ETSIDI::MEDIO);
	ETSIDI::setTextColor(colorTexto.getRf(), colorTexto.getGf(), colorTexto.getBf());

	string hud = "Turno" + to_string(turno) + "| Juega: " + nombreEquipo;
	ETSIDI::printxy(hud.c_str(), 10, (int)(tamTablero + 20));
}


void TableroGL::BotonRaton(int x, int y, int boton, int pulsado, bool shiftKey, bool ctrlKey)
{
	if (pulsado) {
		controlKey = ctrlKey;
		this->shiftKey = shiftKey;
	}
	else controlKey = this->shiftKey = false;

	if (boton == BOTON_IZQ) botonIzq = pulsado;
	else if (boton == BOTON_DER)botonDer = pulsado;
	else if (boton == BOTON_MED) botonMed = pulsado;

	pantalla2casilla(x, y, filaRaton, colRaton);

	if (!pulsado || boton != BOTON_IZQ)return;
	if (!m_tablero->enTablero(filaRaton, colRaton))return;

	cout << "Casillla:(" << filaRaton << "," << colRaton << ")" << endl;

	Casilla& destino = m_tablero->getTabla()[filaRaton][colRaton];

	if (!m_tablero->haySeleccion()) {
		//primera pulsacion:seleccionar pieza propia
		if (destino.getOcupadaPor() == turnoDeQuien)
			m_tablero->seleccionar(filaRaton, colRaton);
	}
	else {
		int filaOrig = m_tablero->getFilaSelec();
		int colOrig = m_tablero->geggtColSelec();
		if (filaRaton == filaOrig && colRaton == colOrig) m_tablero->limipiarSeleccion();
		else if (destino.getOcupadaPor() == NADIE) {
			//movemos pieza a casilla libre
			m_tablero->getTabla()[filaOrig][colOrig].setOcupadaPor(NADIE);
			destino.getOcupadaPor();
			m_tablero->limipiarSeleccion();

			if (m_tablero->controlCincoPuntos(turnoDeQuien))
				cout << "VICTORIA de " << m_tablero->getEquipoLocal().getNombre() << endl;

			turnoActual++;
			m_tablero->avanzarOscilacion(turnoActual, 3);
			turnoDeQuien = (turnoDeQuien == LOCAL) ? VISITANTE : LOCAL;
		}else if (destino.getOcupadaPor() != turnoDeQuien) {
			//casilla enemiga, por lo que comienza el combate y se abre el campo 
			cout << "Combate en (" << filaRaton << "," << colRaton << ")" << endl;
			m_tablero->limipiarSeleccion();
		}
		else {
			//casilla aliada->cambiar seleccion
			m_tablero->seleccionar(filaRaton, colRaton);
		}
	}

}

void TableroGL::KeyDown(unsigned char tecla)
{
	cout << "Tecla: " << tecla << endl;
}

void TableroGL::dibujarRectangulo(float x, float y, float ancho, float alto, const Color3f& color, float alfa)
{
	glColor4f(color.getRf(), color.getGf(), color.getBf(), alfa);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + ancho, y);
	glVertex2f(x + ancho, y + alto);
	glVertex2f(x, y + alto);
	glEnd();
}

void TableroGL::dibujarBorde(float x, float y, float ancho, float alto, const Color3f& color, float grosor)
{
	glColor3f(color.getRf(), color.getGf(), color.getBf());
	glLineWidth(grosor);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + ancho, y);
	glVertex2f(x + ancho, y + alto);
	glVertex2f(x, y + alto);
	glEnd();
	glLineWidth(1.0f);
}

void TableroGL::dibujarCirculo(float cx, float cy, float radio, const Color3f& color, float alfa, int seg)
{
	glColor4f(color.getRf(), color.getGf(), color.getBf(), alfa);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= seg; i++) {
		float ang = 2.0f * 3.14159265f * (float)i / (float)seg;
		glVertex2f(cx + radio * cosf(ang), cy + radio * sinf(ang));
	}
	glEnd();
}

void TableroGL::dibujarMarcadorVIP(float cx, float cy, float radio)
{
	dibujarCirculo(cx, cy, radio, COLOR_ORO, 0.45f);
	dibujarCirculo(cx, cy, radio*0.55f, COLOR_BLANCO_VIP, 0.85f);
	dibujarCirculo(cx, cy, radio*0.18f, COLOR_ORO, 1.00f);
}



