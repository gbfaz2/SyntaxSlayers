
//Autor: María Heredero

//aquí se va a dibujar el tablero. 
//Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
//Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include <iostream>
#include "freeglut.h"
#include "ETSIDI.h"
#include <cmath>
#include "menu.h"
#include"coordinador.h"

using namespace std;

static const int SEGS = 24;//numero de segmentos para dibujar los circulos(usado para dibujar la media luna)
Tablerogl::Tablerogl(Tablero* pb) :m_tablero(pb)
{
	N = pb->getSize();//Siempre va a ser nueve, pero para asegurarnos mejor leerlo directamente de nuestra clase tablero
	ancho = 0.13f;//ancho de una casilla en unidades Opengl
	dist = 1.8;//distancia que hay de la cámara al tablero
	//el centro del tablero es donde va a apuntar la cámara
	//x positivo crece hacia la derecha, y negativo crece hacia abajo
	centro_x = N * ancho / 2.0;
	centro_y = -N * ancho / 2.0;
	centro_z = 0.0;

	Filacursor[0] = 0; Colcursor[0] = 1; //Cursor local
	Filacursor[1] = 0; Colcursor[1] = 7;//Cursor rival

	xcasilla_sel = -1;//todavía no hay casilla seleccionada
	ycasilla_sel = -1;

	fromFila = fromCol = -1;
	fromBando = bando_nada;
	piezaSeleccionada = false;//no hay pieza seleccionada

	victoria_ = bando_nada;//la partida sigue en curso, nadie a ganado

	leftButton = rightButton = midButton = false;
	controlKey = shiftKey = false;
}

void Tablerogl::init()
{
	glEnable(GL_LIGHT0);//activo la fuente de luz
	glEnable(GL_LIGHTING);//activa el sistema de iluminación de Opengl
	glEnable(GL_DEPTH_TEST);//si dos objetos se solapan, solo se ve el que está más cerca de la cámara;
	glEnable(GL_COLOR_MATERIAL);//para poder usar glColor3f incluso con la iluminación activa

	glMatrixMode(GL_PROJECTION);//matriz de proyección
	gluPerspective(40.0, 800 / 600.0f, 0.1, 150);

	//carga textura rey
	SpriteRey::cargarTextura("imagenes\\IMAGEN_REY_CRISTIANO.png");
}


void Tablerogl::Dibuja()//se llama cada frame desde Ondraw(). Orden: fondo-casillas-símbolos-casilla seleccionada-piezas-cuadrícula-quad transparente para el ratón
{
	glEnable(GL_DEPTH_TEST);    // Que las piezas no se solapen mal
	glEnable(GL_LIGHTING);      // Encender las luces
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION); // Seleccionar la lente de la cámara
	glLoadIdentity();
	gluPerspective(40.0, 1024.0 / 768.0, 0.1, 150.0);
	//Recalculamos el centro. Por si N cambiara
	centro_x = N * ancho / 2.0f;
	centro_y = -N * ancho / 2.0f;

	//ponemos un color de fondo
	glClearColor(0.10f, 0.06f, 0.03f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//colocamos la cámara
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//reseteamos la matriz antes de colocar la cámara

	gluLookAt(
		centro_x, centro_y, dist,//posición del ojo
		centro_x, centro_y, centro_z,//punto al que mira
		0.0, 1.0, 0.0//vector "arriba"
	);

	glDisable(GL_LIGHTING);

	DibujaFondo();//fondo png detrás de todo
	glClear(GL_DEPTH_BUFFER_BIT);
	DibujaCasillas();
	DibujaSimbolos();

	// PELICULA SEMITRANSPARENTE PARA ATENUAR EL TABLERO
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.35f); // NEGRO AL 35% DE OPACIDAD, AJUSTAR A GUSTO
	//float mitad = N * ancho / 2.0f;
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.003f);
	glVertex3f(N * ancho, 0.0f, 0.003f);
	glVertex3f(N * ancho, -N * ancho, 0.003f);
	glVertex3f(0.0f, -N * ancho, 0.003f);
	glEnd();
	glDisable(GL_BLEND);

	DibujaCursores();//los cursores del teclado
	DibujaSeleccion();//la pieza seleccionada
	DibujaMovimientosValidos();

	glEnable(GL_LIGHTING);
	DibujaPiezas();
	glDisable(GL_LIGHTING);

	//DibujaCuadricula();

	//hacemos un rectángulo transparente para captura del ratón, para garantizar que siempre hay algo en z=0 bajo el cursor.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.0f);//apha=0: completamente invisible

	glPushMatrix();
	glTranslatef((float)centro_x, (float)centro_y, 0.0f);
	float mitad = N * ancho / 2.0f;
	glRectf(-mitad, mitad, mitad, -mitad);//quad centrado en el tablero
	glPopMatrix();

	glDisable(GL_BLEND);
	//glEnable(GL_LIGHTING);//lo restauramos para las piezas 3D

}

void Tablerogl::DibujaFondo()
{
	//intentamos cargar la textura y si no existiera el archivo, ETSIDI devuelve id=0 y no pasa nada
	auto tex = ETSIDI::getTexture("imagenes/fondo.png");
	if (tex.id == 0)return;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	// Quad grande centrado en el tablero, muy por detrás en Z.
	float cx = (float)centro_x;
	float cy = (float)centro_y;
	float medio = 2.5f;

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(cx - medio, cy - medio, -0.5f);
	glTexCoord2f(1, 0); glVertex3f(cx + medio, cy - medio, -0.5f);
	glTexCoord2f(1, 1); glVertex3f(cx + medio, cy + medio, -0.5f);
	glTexCoord2f(0, 1); glVertex3f(cx - medio, cy + medio, -0.5f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void Tablerogl::DibujaCasillas()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//elegimos el color en función de su posición
			setCasillaColor(fila, col);

			//Dibujamos la casilla
			DibujaCasilla(fila, col);
		}
	}

}

void Tablerogl::DibujaCasilla(int fila, int col)
{
	//como la cuadrícula la vamos a dibujar z=0, las casillas las vamos a dibujar un pelín por detrás
	float x0 = col * ancho;//borde izq
	float x1 = x0 + ancho;//borde derecho
	float y0 = -fila * ancho;//borde superior(y negativa porque crecce abajo)
	float y1 = y0 - ancho;

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);//apunta hacia la cámara

	glVertex3f(x0, y0, -0.001f);//superior izq
	glVertex3f(x1, y0, -0.001f);//superior dcha
	glVertex3f(x1, y1, -0.001f);//inferior dcha
	glVertex3f(x0, y1, -0.001f);//inferior izq

	glEnd();
}

void Tablerogl::setCasillaColor(int fila, int col)
{
	switch (m_tablero->getCasilla(fila, col).tipo) {
	case Casilla_local:
		glColor3f(0.75f, 0.15f, 0.15f); // ROJO MAS CLARO 
		return;
	case Casilla_rival:
		glColor3f(0.70f, 0.30f, 0.85f); // MORADO MAS CLARO
		return;
	case Casilla_dinamica:
		glColor3f(0.50f, 0.50f, 0.50f); // GRIS MAS CLARO
		return;
	case Casilla_poder:
		// El power point usa el color de la zona en que está
		// (para que encaje visualmente) y encima dibujamos el círculo
		if (col < 4)       glColor3f(0.55f, 0.10f, 0.65f); // zona morada
		else if (col == 4) glColor3f(0.35f, 0.35f, 0.35f); // zona gris
		else               glColor3f(0.50f, 0.04f, 0.04f); // zona roja
		return;
	default:
		glColor3f(0.5f, 0.5f, 0.5f);
	}
}

void Tablerogl::DibujaSimbolos()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//centro de estas casillas en coordenadas opengl
			float cx, cy;
			cell2center(fila, col, cx, cy);

			//tamaño del simbolo menos que la casilla
			float size = ancho * 0.34f;

			TipoCasilla tipo = m_tablero->getCasilla(fila, col).tipo;

			if (tipo == Casilla_local) DibujaCruz(cx, cy, size);

			else if (tipo == Casilla_rival) DibujaLuna(cx, cy, size);
			else if (tipo == Casilla_poder) DibujaPuntoPoder(cx, cy, size);
		}
	}
}

void Tablerogl::DibujaCruz(float cx, float cy, float size)
{
	glColor3f(1.0f, 1.0f, 1.0f); // blanco

	float brazo = size;          // longitud del brazo (desde el centro)
	float mitad_brazo = size * 0.30f;  // semiancho del brazo

	glBegin(GL_QUADS);
	// Brazo vertical (de arriba a abajo)
	glVertex3f(cx - mitad_brazo, cy + brazo, 0.002f);//usamos z=0.002 para que quede delante de los rectángulos de casilla que están en -0.001
	glVertex3f(cx + mitad_brazo, cy + brazo, 0.002f);
	glVertex3f(cx + mitad_brazo, cy - brazo, 0.002f);
	glVertex3f(cx - mitad_brazo, cy - brazo, 0.002f);

	// Brazo horizontal (de izquierda a derecha)
	glVertex3f(cx - brazo, cy + mitad_brazo, 0.002f);
	glVertex3f(cx + brazo, cy + mitad_brazo, 0.002f);
	glVertex3f(cx + brazo, cy - mitad_brazo, 0.002f);
	glVertex3f(cx - brazo, cy - mitad_brazo, 0.002f);
	glEnd();
}

void Tablerogl::DibujaLuna(float cx, float cy, float size)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(cx, cy, 0.002f); // centro del abanico
	// Arco de 90° a 270° (mitad izquierda del círculo)
	// Con 90°=arriba y 270°=abajo dibujamos la mitad derecha visible
	for (int i = 0; i <= SEGS; i++) {
		// Mapeamos i de 0..SEGS a ángulo de -90° a +90°
		// (parte derecha del círculo, que en pantalla queda a la izq
		//  porque el tablero rival está a la derecha)
		float a = -3.14159265f / 2.0f - 3.14159265f * i / SEGS;
		glVertex3f(cx + size * cosf(a), cy - size * sinf(a), 0.002f);
	}
	glEnd();
}

void Tablerogl::DibujaPuntoPoder(float cx, float cy, float size)
{

	glColor3f(1.0f, 0.90f, 0.0f); // amarillo dorado

	// círculo exterior (solo contorno)
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < SEGS; i++) {
		float angulo = 2.0f * (float)3.14159265f * i / SEGS;
		glVertex3f(cx + size * cos(angulo), cy + size * sin(angulo), 0.002f);
	}
	glEnd();
	glLineWidth(1.0f);

	// punto central sólido (disco pequeño)
	float r = size * 0.30f; // radio del punto: 30% del radio del círculo
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(cx, cy, 0.002f); // centro del abanico
	for (int i = 0; i <= SEGS; i++) {
		float angulo = 2.0f * (float)3.14159265f * i / SEGS;
		glVertex3f(cx + r * cos(angulo), cy + r * sin(angulo), 0.002f);
	}
	glEnd();
}

void Tablerogl::DibujaCasSelec(int fila, int col, float r, float g, float b, float lw, float z)//para resaltar en amarillo la casilla seleccionada y la casilla bajo el cursor se resalta en blanco
{
	float x0 = col * ancho + 0.004f, x1 = (col + 1) * ancho - 0.004f;
	float y0 = -fila * ancho - 0.004f, y1 = -(fila + 1) * ancho + 0.004f;
	glColor3f(r, g, b);
	glLineWidth(lw);
	glBegin(GL_LINE_LOOP);
	glVertex3f(x0, y0, z); glVertex3f(x1, y0, z);
	glVertex3f(x1, y1, z); glVertex3f(x0, y1, z);
	glEnd();
	glLineWidth(1.0f);
}
//Para el bando local[0]-> amarillo (WASD+ESPACIO) y para el rival [1]->cian (FLECHAS + ENTER)
void Tablerogl::DibujaCursores()
{
	DibujaCasSelec(Filacursor[0], Colcursor[0], 1.0f, 1.0f, 0.0f, 3.0f, 0.005f);
	DibujaCasSelec(Filacursor[1], Colcursor[1], 0.0f, 1.0f, 1.0f, 3.0f, 0.005f);
}

void Tablerogl::DibujaSeleccion()
{
	if (!piezaSeleccionada || fromFila < 0)return;
	DibujaCasSelec(fromFila, fromCol, 1.0f, 1.0f, 1.0f, 4.0f, 0.006f);
}

void Tablerogl::DibujaMovimientosValidos()
{
	if (!piezaSeleccionada || fromFila < 0)return;

	auto validas = m_tablero->casillasValidas(fromFila, fromCol);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (const auto& pos : validas) {
		float x0 = pos.col * ancho;
		float x1 = x0 + ancho;
		float y0 = -pos.fila * ancho;
		float y1 = y0 - ancho;

		//relleno amarillo semitransparente
		glColor4f(1.0f, 1.0f, 0.0f, 0.25f);
		glBegin(GL_QUADS);
		glVertex3f(x0, y0, 0.004f); glVertex3f(x1, y0, 0.004f);
		glVertex3f(x1, y1, 0.004f); glVertex3f(x0, y1, 0.004f);
		glEnd();

		// Contorno amarillo sólido
		glColor4f(1.0f, 1.0f, 0.0f, 0.85f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x0 + 0.003f, y0 - 0.003f, 0.005f);
		glVertex3f(x1 - 0.003f, y0 - 0.003f, 0.005f);
		glVertex3f(x1 - 0.003f, y1 + 0.003f, 0.005f);
		glVertex3f(x0 + 0.003f, y1 + 0.003f, 0.005f);
		glEnd();
		glLineWidth(1.0f);
	}

	glDisable(GL_BLEND);
}

void Tablerogl::DibujaVictoria()
{
	//lo dibujamos en coordenadas mundo, centrado en el tablero
	float cx = (float)centro_x;
	float cy = (float)centro_y;
	float hw = N * ancho * 0.45f;//semiancho del cartel
	float hh = N * ancho * 0.15f;//semialto

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
	glBegin(GL_QUADS);
	glVertex3f(cx - hw, cy + hh, 0.02f);
	glVertex3f(cx + hw, cy + hh, 0.02f);
	glVertex3f(cx + hw, cy - hh, 0.02f);
	glVertex3f(cx - hw, cy - hh, 0.02f);
	glEnd();
	glDisable(GL_BLEND);

	//Textos de victoria que aparecen si victoria_!=bando_nada
	if (victoria_ == bando_local) {
		ETSIDI::setTextColor(1.0f, 0.85f, 0.10f, 1.0f); // dorado
		ETSIDI::setFont("fuentes/nuevafuente.ttf", 36);
		ETSIDI::printxy("VICTORIA CRISTIANA", 300, 400);
	}
	else {
		ETSIDI::setTextColor(0.75f, 0.20f, 0.90f, 1.0f); // morado
		ETSIDI::setFont("fuentes/nuevafuente.ttf", 36);
		ETSIDI::printxy("VICTORIA AL-ANDALUS", 280, 400);
	}

	ETSIDI::setTextColor(0.80f, 0.80f, 0.80f, 1.0f);
	ETSIDI::setFont("fuentes/nuevafuente.ttf", 18);
	ETSIDI::printxy("Pulsa ESC para volver al menu", 310, 360);

	glEnable(GL_DEPTH_TEST);
}



void Tablerogl::DibujaPiezas()//va a recorrer todo el tablero y dibuja la pieza de la casilla que corresponda
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			if (m_tablero->getCasilla(fila, col).pieza != pieza_nada)
				DibujaPieza(fila, col);
		}
	}
}

void Tablerogl::DibujaPieza(int fil, int col)
{
	const Casilla& casilla = m_tablero->getCasilla(fil, col);
	float cx, cy;
	cell2center(fil, col, cx, cy);

	// Color del equipo
	if (casilla.bando == bando_local)
		glColor3f(1.00f, 0.95f, 0.85f); // blanco crema
	else
		glColor3f(0.90f, 0.55f, 0.15f); // naranja

	float escala = ancho * 0.30f; // escala base

	glPushMatrix();
	glTranslatef(cx, cy, escala + 0.01f); // elevamos sobre el tablero

	switch (casilla.pieza) {

	case pieza_esfera: {
		glPopMatrix();

		if (casilla.bando != bando_local) {
			return;  // el rival no se dibuja todavía
		}
			//_spriteReyLocal;
		//SpriteRey& sprite = (casilla.bando == bando_local) _spriteReyLocal;

		float tableroAncho = N * ancho;
		float tableroAlto = N * ancho;

		// Porcentaje de posición dentro del tablero (0 a 1)
		float nx = cx / tableroAncho;
		float ny = (-cy) / tableroAlto;

		// Convertir a píxeles teniendo en cuenta el margen
		// El tablero ocupa aproximadamente el 60% del ancho de ventana centrado
		float margenX = _anchoVentana * 0.18f;  // margen izquierdo aproximado
		float margenY = _altoVentana * 0.08f;  // margen superior aproximado
		float anchoTablero = _anchoVentana * 0.60f;
		float altoTablero = _altoVentana * 0.80f;

		float px = margenX + nx * anchoTablero;
		float py = _altoVentana - (margenY + ny * altoTablero);

		float size = (anchoTablero / N) * 1.3f;  // tamaño de una casilla

		entrar2D(_anchoVentana, _altoVentana);
		glDisable(GL_LIGHTING);
		_spriteReyLocal.dibujar(px, py, size);
		salir2D();
		return;
	}
	case pieza_dodecaedro:
		// Dodecaedro: Infiltrado / Asesino de Élite
		// glutSolidDodecahedron tiene radio ~1, escalamos
		glScalef(escala * 0.55f, escala * 0.55f, escala * 0.55f);
		glutSolidDodecahedron();
		break;

	case pieza_icosaedro:
		// Icosaedro: Almogávar / Arquero a Caballo
		glScalef(escala * 0.65f, escala * 0.65f, escala * 0.65f);
		glutSolidIcosahedron();
		break;

	case pieza_tetraedro:
		// Tetraedro: Caballería Ligera / Jinete Bereber
		glScalef(escala * 0.70f, escala * 0.70f, escala * 0.70f);
		glutSolidTetrahedron();
		break;

	case pieza_cubog:
		// Cubo grande: Infantería Pesada / Guardia Negra
		glutSolidCube(escala * 1.1f);
		break;

	case pieza_cono:
		// Cono: Caballería Pesada / Caballería Acorazada
	{
		GLUquadric* q = gluNewQuadric();
		glRotatef(90, 1, 0, 0);            // lo ponemos apuntando arriba
		glTranslatef(0, 0, -escala * 0.7f);     // lo centramos
		gluCylinder(q, escala * 0.5f, 0.0f, escala * 1.2f, 12, 1); // base→punta
		gluDisk(q, 0, escala * 0.5f, 12, 1);      // tapa de la base
		gluDeleteQuadric(q);
	}
	break;

	case pieza_cilindro:
		// Cilindro: Ballestero / Arquero Ghazí
	{
		GLUquadric* q = gluNewQuadric();
		glTranslatef(0, 0, -escala * 0.5f);
		gluCylinder(q, escala * 0.35f, escala * 0.35f, escala * 1.0f, 12, 1);
		gluDisk(q, 0, escala * 0.35f, 12, 1);     // tapa inferior
		glTranslatef(0, 0, escala * 1.0f);
		gluDisk(q, 0, escala * 0.35f, 12, 1);     // tapa superior
		gluDeleteQuadric(q);
	}
	break;

	case pieza_cubo_p:
		// Cubo pequeño: Miliciano / Soldado Ghazí
		glutSolidCube(escala * 0.75f);
		break;
	default: break;
	}

	glPopMatrix();
}

/*void Tablerogl::DibujaCuadricula()
{
	glColor3f(0, 0, 0);
	float total = N * ancho;
	for (int i = 0; i <= N; i++) {
		glLineWidth(i % N == 0 ? 4.0f : 1.0f);
		glBegin(GL_LINES);
		glVertex3f(i * ancho, 0, 0); glVertex3f(i * ancho, -total, 0);
		glVertex3f(0, -i * ancho, 0); glVertex3f(total, -i * ancho, 0);
		glEnd();
	}
	glLineWidth(1.0f);
}*/

void Tablerogl::trySelectorMove(BandoPieza bando)
{
	//Obtenemos las coordenadas a las que apunta el cursor o el ratón en este momento
	int idx = (bando == bando_local) ? 0 : 1;
	int currentFila = Filacursor[idx];
	int currentCol = Colcursor[idx];

	// Si NO hay pieza seleccionada, estamos en FASE DE SELECCIÓN
	if (!piezaSeleccionada) {

		// Comprobamos si la pieza en esa casilla pertenece al bando que tiene el turno
		if (gestorTurnos.esDelBandoActual(*m_tablero, currentFila, currentCol)) {
			// Es válida. Guardamos el origen y marcamos como seleccionada
			fromFila = currentFila;
			fromCol = currentCol;
			fromBando = bando;
			piezaSeleccionada = true;
		}
		// Si no es su turno o la casilla está vacía, no hace nada y sale
		return;
	}

	// Si YA HAY pieza seleccionada, estamos en FASE DE MOVIMIENTO
	else {

		Pieza* pieza = m_tablero->getCasilla(fromFila, fromCol).obj;

		//error pero como gestionar? ayuda
		if (gestorTurnos.getBandoActual() == bando_local) {
			Pieza* p = m_tablero->getCasilla(currentFila, currentCol).obj;
			if (p && (p->getNombre() == "Rey" || p->getNombre() == "Emir"))
				_spriteReyLocal.setEstado(EstadoRey::WALK);
		}

		if (!pieza) {
			piezaSeleccionada = false; // Por seguridad, si la pieza desapareció
			return;
		}

		// Intentamos mover la pieza desde el origen al destino (currentFila, currentCol)
		ResultadoMovimiento resultado = gestorMovimiento.resolverMovimiento(
			pieza, *m_tablero, currentFila, currentCol
		);

		if (resultado == ResultadoMovimiento::MOVIMIENTO_OK) {
			piezaSeleccionada = false;
			fromFila = fromCol = -1; // Reseteamos el origen

			// ¡IMPORTANTE! Aquí debes avisar al gestor de turnos de que el turno ha terminado
			gestorTurnos.terminarTurno();
		}
		else if (resultado == ResultadoMovimiento::COMBATE) {
			piezaSeleccionada = false;
			fromFila = fromCol = -1;
			
			// Avisamos al coordinador de que hay combate pendiente
			Pieza* atacante = gestorMovimiento.getUltimoAtacante();
			Pieza* defensora = gestorMovimiento.getUltimaDefensora();

			if (m_tablero->getCasilla(fromFila, fromCol).bando == bando_local)
				_spriteReyLocal.setEstado(EstadoRey::ATTACK);
			//else
				//_spriteReyRival.setEstado(EstadoRey::ATTACK);


			if (atacante && defensora) {
				_pAtacante = atacante;
				_pDefensora = defensora;
				_combatePendiente = true;
			}

			gestorTurnos.terminarTurno();
		}
		// Si el movimiento es INVÁLIDO o BLOQUEADO, la pieza sigue seleccionada
		// esperando a que elijas un destino válido (o puedes cancelar la selección si prefieres).
	}
}

void Tablerogl::cell2center(int casilla_x, int casilla_y, float& glx, float& gly)
{
	glx = casilla_y * ancho + ancho / 2.0f;
	gly = -casilla_x * ancho - ancho / 2.0f;
}

void Tablerogl::world2cell(double x, double y, int& casilla_x, int& casilla_y)
{
	casilla_x = (int)(fabs(y / ancho));
	casilla_y = (int)(x / ancho);
}

void Tablerogl::limpiarCombate()
{
	_combatePendiente = false;
	_pAtacante = nullptr;
	_pDefensora = nullptr; 

}

void Tablerogl::redimensionar(int ancho, int alto) {
	_anchoVentana = ancho;
	_altoVentana = (alto == 0) ? 1 : alto;

	return;
}