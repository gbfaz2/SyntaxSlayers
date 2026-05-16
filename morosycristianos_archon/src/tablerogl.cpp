//aquí se va a dibujar el tablero. 
//Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
//Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include <iostream>
#include "freeglut.h"
#include "ETSIDI.h"
#include <cmath>
#include <string>

using namespace std;

static const int CIRCLE_SEGS = 32;//numero de segmentos para dibujar los circulos(usado para dibujar la media luna)
Tablerogl::Tablerogl(Tablero* pb):m_tablero(pb)
{
	N = pb->getSize();//Siempre va a ser nueve, pero para asegurarnos mejor leerlo directamente de nuestra clase tablero
	ancho = 0.13f;//ancho de una casilla en unidades Opengl
	dist = 1.8;//distancia que hay de la cámara al tablero
	//el centro del tablero es donde va a apuntar la cámara
	//x positivo crece hacia la derecha, y negativo crece hacia abajo
	centro_x = N * ancho / 2.0;
	centro_y = -N * ancho / 2.0;
	centro_z = 0.0;

	xcasilla_sel = -1;//todavía no hay casilla seleccionada
	ycasilla_sel = -1;

	fromFila = fromCol = -1;
	piezaSeleccionada = false;//no hay pieza seleccionada

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
}

void Tablerogl::Dibuja()//se llama cada frame desde Ondraw(). Orden: fondo-casillas-símbolos-casilla seleccionada-piezas-cuadrícula-quad transparente para el ratón
{
	//Recalculamos el centro. Por si N cambiara
	centro_x = N * ancho / 2.0f;
	centro_y = -N * ancho / 2.0f;

	//ponemos un color de fondo
	glClearColor(0.18f, 0.08f, 0.04f, 1.0f);
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

	DibujaCasillas();
	DibujaSimbolos();
	DibujaCasSelec();

	glEnable(GL_LIGHTING);
	DibujaPiezas();
	glDisable(GL_LIGHTING);

	DibujaCuadricula();

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
	glEnable(GL_LIGHTING);//lo restauramos para las piezas 3D
}

void Tablerogl::DibujaCasillas()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//¿es una casilla clara u oscura?
			bool dark = (fila + col) % 2 == 1;//es impar
			//elegimos el color en función del tipo
			setCasillaColor(fila,col,dark);

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

void Tablerogl::setCasillaColor(int fila, int col, bool dark)
{
	TipoCasilla tipo = m_tablero->getCasilla(fila, col).tipo;
	//Colores fijos (LOCAL Y RIVAL no oscilan)
	if (tipo == Casilla_local) {
		if (!dark)glColor3f(0.60f, 0.15f, 0.70f); //morado medio
		else glColor3f(0.30f, 0.05f, 0.40f);//morado oscuro
		return;
	}
	if (tipo == Casilla_rival) {
		if (!dark)glColor3f(0.75f, 0.10f, 0.70f); //rojo medio
		else glColor3f(0.45f, 0.03f, 0.03f);//rojo oscuro
		return;
	}
	
	//casillas dinámicas y puntos de poder: interpolamos según la fase
	float fase = m_tablero->getCasilla(fila, col).fase;
	float rL, gL, bL; // color cuando fase=0.0 (ventaja local, morado)
	float rN, gN, bN; // color cuando fase=0.5 (neutro, gris)
	float rR, gR, bR; // color cuando fase=1.0 (ventaja rival, rojo)

	if (!dark) {
		rL = 0.60f; gL = 0.15f; bL = 0.70f; // morado medio
		rN = 0.55f; gN = 0.55f; bN = 0.55f; // gris medio
		rR = 0.75f; gR = 0.10f; bR = 0.10f; // rojo medio
	}
	else {
		rL = 0.30f; gL = 0.05f; bL = 0.40f; // morado oscuro
		rN = 0.30f; gN = 0.30f; bN = 0.30f; // gris oscuro
		rR = 0.45f; gR = 0.03f; bR = 0.03f; // rojo oscuro
	}

	float r, g, b;
	if (fase <= 0.5f) {
		float t = fase / 0.5f; // 0→0.5 se convierte en 0→1
		r = rL + t * (rN - rL);
		g = gL + t * (gN - gL);
		b = bL + t * (bN - bL);
	}
	else {
		// Tramo 2: gris → rojo (phase 0.5 a 1.0)
		float t = (fase - 0.5f) / 0.5f; // 0.5→1.0 se convierte en 0→1
		r = rN + t * (rR - rN);
		g = gN + t * (gR - gN);
		b = bN + t * (bR - bN);
	}

	glColor3f(r, g, b);
}

void Tablerogl::DibujaSimbolos()
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			//centro de estas casillas en coordenadas opengl
			float cx, cy;
			cell2center(fila, col, cx, cy);

			//tamaño del simbolo menos que la casilla
			float size = ancho * 0.38f;

			TipoCasilla tipo = m_tablero->getCasilla(fila, col).tipo;

			if (tipo == Casilla_local) {
				DibujaCruz(cx, cy, size);
			}
			else if (tipo == Casilla_rival) {
				DibujaLuna(cx, cy, size);
			}
			else if (tipo == Casilla_poder) {
				DibujaPuntoPoder(cx, cy, size);
			}
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

	// Primero: disco blanco completo
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(cx, cy, 0.002f); // centro
	for (int i = 0; i <= CIRCLE_SEGS; i++) {
		float a = 2.0f * 3.14159265f * i / CIRCLE_SEGS;
		glVertex3f(cx + size * cosf(a), cy + size * sinf(a), 0.002f);
	}
	glEnd();

	// Luego disco del color de la casilla, desplazado 
	// El color debe coincidir con el fondo de la casilla (dark o no)
	bool dark = false; // las casillas rival pares son claras (row+col par)
	glColor3f(0.60f, 0.07f, 0.07f); // rojo medio (fondo de la zona)

	float dx = size * 0.40f;  // desplazamiento a la derecha
	float rInner = size * 0.80f; // radio del disco que "tapa"
	float ox = cx + dx;       // centro del disco tapador

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(ox, cy, 0.002f); // Z=0.003 para estar delante del blanco
	for (int i = 0; i <= CIRCLE_SEGS; i++) {
		float a = 2.0f * 3.14159265f * i / CIRCLE_SEGS;
		glVertex3f(ox + rInner * cosf(a),cy + rInner * sinf(a), 0.002f);
	}
	glEnd();
}

void Tablerogl::DibujaPuntoPoder(float cx, float cy, float size)
{

	glColor3f(1.0f, 0.90f, 0.0f); // amarillo dorado

	// círculo exterior (solo contorno)
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < CIRCLE_SEGS; i++) {
		float angulo = 2.0f * (float)3.14159265f * i / CIRCLE_SEGS;
		glVertex3f(cx + size * cos(angulo),cy + size * sin(angulo),0.002f);
	}
	glEnd();
	glLineWidth(1.0f);

	// punto central sólido (disco pequeño)
	float r = size * 0.30f; // radio del punto: 30% del radio del círculo
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(cx, cy, 0.002f); // centro del abanico
	for (int i = 0; i <= CIRCLE_SEGS; i++) {
		float angulo = 2.0f * (float)3.14159265f * i / CIRCLE_SEGS;
		glVertex3f(cx + r * cos(angulo),cy + r * sin(angulo),0.002f);
	}
	glEnd();
}

void Tablerogl::DibujaCasSelec()//para resaltar en amarillo la casilla seleccionada y la casilla bajo el cursor se resalta en blanco
{
	//Casilla bajo el cursor 
	if (xcasilla_sel >= 0 && xcasilla_sel < N && ycasilla_sel >= 0 && ycasilla_sel < N) {
		float x0 = ycasilla_sel * ancho, x1 = x0 + ancho;
		float y0 = -xcasilla_sel * ancho, y1 = y0 - ancho;
		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x0 + 0.003f, y0 - 0.003f, 0.005f);
		glVertex3f(x1 - 0.003f, y0 - 0.003f, 0.005f);
		glVertex3f(x1 - 0.003f, y1 + 0.003f, 0.005f);
		glVertex3f(x0 + 0.003f, y1 + 0.003f, 0.005f);
		glEnd();
		glLineWidth(1.0f);
	}

	//Pieza seleccionada (origen del movimiento): amarillo
	if (piezaSeleccionada && fromFila >= 0 && fromCol >= 0) {
		float x0 = fromCol * ancho, x1 = x0 + ancho;
		float y0 = -fromFila * ancho, y1 = y0 - ancho;
		glColor3f(1.0f, 1.0f, 0.0f); // amarillo brillante
		glLineWidth(3.5f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(x0 + 0.004f, y0 - 0.004f, 0.005f);
		glVertex3f(x1 - 0.004f, y0 - 0.004f, 0.005f);
		glVertex3f(x1 - 0.004f, y1 + 0.004f, 0.005f);
		glVertex3f(x0 + 0.004f, y1 + 0.004f, 0.005f);
		glEnd();
		glLineWidth(1.0f);
	}
}

void Tablerogl::DibujaPiezas()//va a recorrer todo el tablero y dibuja la pieza de la casilla que corresponda
{
	for (int fila = 0; fila < N; fila++) {
		for (int col = 0; col < N; col++) {
			if (m_tablero->getCasilla(fila, col).pieza != pieza_nada)
				DibujaPieza(fila,col);
		}
	}
}

void Tablerogl::DibujaPieza(int fil, int col)
{
	const Casilla& casilla = m_tablero->getCasilla(fil, col);

	//centro de la casilla en opengl
	float cx, cy;
	cell2center(fil, col, cx, cy);
	// Color según bando
	if (casilla.bando == bando_local)
		glColor3f(1.00f, 0.95f, 0.85f); // blanco crema
	else
		glColor3f(0.85f, 0.55f, 0.20f); // naranja arena

	float escala_pieza = ancho * 0.35f; // escala base de la pieza

	glPushMatrix();
	glTranslatef(cx, cy, escala_pieza); // elevamos la pieza sobre el tablero

	switch (casilla.pieza) {

	case pieza_capitan:
		// Esfera grande: el capitán es el más visible
		glutSolidSphere(escala_pieza * 1.0f, 16, 16);
		break;

	case pieza_soldado:
		// Cubo pequeño: tropa básica
		glutSolidCube(escala_pieza * 1.2f);
		break;

	case pieza_caballero:
		// Pirámide: base cuadrada + 4 triángulos laterales
		// La dibujamos manualmente con GL_TRIANGLES y GL_QUADS
	{
		float altura = escala_pieza * 1.4f; // altura de la pirámide
		float base = escala_pieza * 0.7f; // semilado de la base

		// Base cuadrada (GL_QUADS)
		glBegin(GL_QUADS);
		glNormal3f(0, 0, -1);
		glVertex3f(-base, -base, -escala_pieza); glVertex3f(base, -base, -escala_pieza);
		glVertex3f(base, base, -escala_pieza); glVertex3f(-base, base, -escala_pieza);
		glEnd();

		// 4 caras laterales (GL_TRIANGLES)
		// Cara frontal
		glBegin(GL_TRIANGLES);
		glNormal3f(0, -1, 0.5f);
		glVertex3f(-base, -base, -escala_pieza); glVertex3f(base, -base, -escala_pieza);
		glVertex3f(0, 0, altura - escala_pieza);
		// Cara trasera
		glNormal3f(0, 1, 0.5f);
		glVertex3f(base, base, -escala_pieza); glVertex3f(-base, base, -escala_pieza);
		glVertex3f(0, 0, altura - escala_pieza);
		// Cara derecha
		glNormal3f(1, 0, 0.5f);
		glVertex3f(base, -base, -escala_pieza); glVertex3f(base, base, -escala_pieza);
		glVertex3f(0, 0, altura - escala_pieza);
		// Cara izquierda
		glNormal3f(-1, 0, 0.5f);
		glVertex3f(-base, base, -escala_pieza); glVertex3f(-base, -base, -escala_pieza);
		glVertex3f(0, 0, altura - escala_pieza);
		glEnd();
	}
	break;

	case pieza_arquero:
		// Cilindro: lo dibujamos con gluCylinder
	{
		GLUquadric* q = gluNewQuadric();
		glRotatef(-90, 1, 0, 0); // lo ponemos vertical
		gluCylinder(q, escala_pieza * 0.4f, escala_pieza * 0.4f, escala_pieza * 1.2f, 12, 1);
		// Tapas del cilindro
		gluDisk(q, 0, escala_pieza * 0.4f, 12, 1); // tapa inferior
		glTranslatef(0, 0, escala_pieza * 1.2f);
		gluDisk(q, 0, escala_pieza * 0.4f, 12, 1); // tapa superior
		gluDeleteQuadric(q);
	}
	break;

	default:
		break;
	}

	glPopMatrix();
}

void Tablerogl::DibujaCuadricula()//para delimitar el tablero del fondo
{
	glColor3f(0.0f, 0.0f, 0.0f);//ponemos un color negro para todas las líneas
	float total = N * ancho;//longitud todal del tablero

	for (int i = 0; i <= N; i++) {
		//hacemos un borde exterior más grueso
		if (i % N == 0) glLineWidth(4.0f);//primera y última línea
		else glLineWidth(1.0f);//líneas interiores

		glBegin(GL_LINES);
		//Línea vertical: x = i*ancho, va de arriba (y=0) a abajo (y=-total)
		glVertex3f(i * ancho, 0.0f, 0.0f);
		glVertex3f(i * ancho, -total, 0.0f);

		//Linea horizontal: y = -i*ancho, va de izq (x=0) a derecha (x=total)
		glVertex3f(0.0f, -i * ancho, 0.0f);
		glVertex3f(total, -i * ancho, 0.0f);
		glEnd();
	}
	glLineWidth(1.0f);//restauramos el grosor por defecto
}

void Tablerogl::KeyDown(unsigned char key)
{
	if (key == 27 || key == 'q' || key == 'Q') {
		cout << "[Board] cerrando." << endl;
		exit(0);
	}

	// ESC también cancela la selección actual
	if (key == 27) {
		piezaSeleccionada = false;
		fromFila = fromCol = -1;
	}
}

void Tablerogl::MouseButton(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey)//convierte el clic del ratón en coordenadas de casilla
{
	//leemos las matrices actuales de opengl
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	//en opengl y=0 está abajo; en la ventana arriba por lo que lo invertimos
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;

	//leemos la profundidad del píxel bajo el cursor
	glReadPixels(x, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	//proyeccion inversa: pixel->coordenadas 3D del mundo
	gluUnProject(winX, winY, winZ,
		modelview, projection, viewport,
		&posX, &posY, &posZ);

	int clickFila, clickCol;
	//coordenadas mundo->casilla(fila, columna)
	world2cell(posX, posY, clickFila, clickCol);

	// Actualizamos casilla bajo el cursor
	xcasilla_sel = clickFila;
	ycasilla_sel = clickCol;

	//Estado de botones y teclas modificadoras
	if (down) { controlKey = ctrlKey; this->shiftKey = shiftKey; }
	else { controlKey = this->shiftKey = false; }

	if (button == MOUSE_LEFT_BUTTON)leftButton = down;
	if (button == MOUSE_RIGHT_BUTTON)rightButton = down;
	if (button == MOUSE_MIDDLE_BUTTON)midButton = down;

	// Solo procesamos el momento de pulsar (down), no el de soltar
	if (!down) return;

	// Clic derecho: cancela selección
	if (button == MOUSE_RIGHT_BUTTON) {
		piezaSeleccionada = false;
		fromFila = fromCol = -1;
		return;
	}

	// Solo procesamos clic izquierdo a partir de aquí
	if (button != MOUSE_LEFT_BUTTON) return;

	// Fuera del tablero: ignoramos
	if (clickFila < 0 || clickFila >= N || clickCol < 0 || clickCol >= N) return;

	const Casilla& clicked = m_tablero->getCasilla(clickFila, clickCol);


	//la logica de puede mover o puede no mover ya la tengo yo implementada en cada pieza, cambiar necesario CUIDADO
	if (!piezaSeleccionada) {
		// cancelar si clicka la misma casilla
		/*if (clickFila == fromFila && clickCol == fromCol) {
			piezaSeleccionada = false;
			fromFila = fromCol = -1;
			return;
		}

		// primero: ¿es tu turno?
		if (!gestorTurnos.esDelBandoActual(*m_tablero, fromFila, fromCol)) {
			cout << "[GestorTurnos] No es tu turno!" << endl;
			piezaSeleccionada = false;
			return;
		}

		// segundo: ¿puede moverse ahí?
		if (m_tablero->puedeMover(fromFila, fromCol, clickFila, clickCol)) {
			bool batalla = m_tablero->muevePieza(fromFila, fromCol, clickFila, clickCol);
			gestorTurnos.terminarTurno();
			if (batalla)
				cout << "[Mouse] Combate!" << endl;*/


		// ── SIN SELECCIÓN: intentamos seleccionar una pieza ──
		if (clicked.pieza != pieza_soldado) {
			// Hay pieza aquí → la seleccionamos
			fromFila = clickFila;
			fromCol = clickCol;
			piezaSeleccionada = true;
			cout << "[Mouse] Pieza seleccionada en ("
				<< fromFila << "," << fromCol << ")" << endl;
		}
		// Si no hay pieza, no hacemos nada

	
		else {
			// ── CON SELECCIÓN: intentamos mover ──

			if (clickFila == fromFila && clickCol == fromCol) {
				// Clic en la misma casilla → cancelar selección
				piezaSeleccionada = false;
				fromFila = fromCol = -1;
				return;
			}

			if (m_tablero->puedeMover(fromFila, fromCol, clickFila, clickCol)) {

				// Movimiento válido
				bool batalla = m_tablero->muevePieza(fromFila, fromCol,clickFila, clickCol);
				if (batalla) {
					// TODO: activar pantalla de combate
					// Por ahora el combate elimina al defensor
					cout << "[Mouse] Combate! (pantalla de combate pendiente)" << endl;
				}
			}
			else {
				cout << "[Mouse] Movimiento inválido: casilla ocupada por aliado." << endl;
			}



			// En cualquier caso, deseleccionamos
			piezaSeleccionada = false;
			fromFila = fromCol = -1;
		}
		return;
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

