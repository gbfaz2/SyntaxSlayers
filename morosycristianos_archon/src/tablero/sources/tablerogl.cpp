
//Autor: María Heredero

//aquí se va a dibujar el tablero. 
//Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
//Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include <iostream>
#include "freeglut.h"
#include "ETSIDI.h"
#include <cmath>

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
	DibujaCursores();//los cursores del teclado
	DibujaSeleccion();//la pieza seleccionada

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
		glColor3f(0.50f, 0.04f, 0.04f);// rojo oscuro 
		return;
	case Casilla_rival:
		glColor3f(0.55f, 0.10f, 0.65f);// morado
		return;
	case Casilla_dinamica:
		glColor3f(0.35f, 0.35f, 0.35f); // gris
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

	case pieza_esfera:
		// Esfera grande: el líder (Rey / Emir)
		glutSolidSphere(escala * 1.0f, 16, 16);
		break;

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
	int idx = (bando == bando_local) ? 0 : 1;
	int cr = Filacursor[idx];
	int cc = Colcursor[idx];

	if (!piezaSeleccionada) {
		// Intentamos seleccionar una pieza propia
		const Casilla& c = m_tablero->getCasilla(cr, cc);
		if (c.pieza != pieza_nada && c.bando == bando) {
			fromFila = cr;
			fromCol = cc;
			fromBando = bando;
			piezaSeleccionada = true;
			cout << "[" << (bando == bando_local ? "LOCAL" : "RIVAL")
				<< "] Seleccionada en (" << cr << "," << cc << ")" << endl;
		}
		else {
			cout << "[" << (bando == bando_local ? "LOCAL" : "RIVAL")
				<< "] No hay pieza propia en (" << cr << "," << cc << ")" << endl;
		}

	}
	else {
		// Ya hay pieza seleccionada: este clic es el destino

		// Solo puede mover el equipo que seleccionó la pieza
		if (fromBando != bando) return;

		if (cr == fromFila && cc == fromCol) {
			// Clic en la misma casilla → cancelar selección
			piezaSeleccionada = false;
			fromFila = fromCol = -1;
			cout << "Selección cancelada." << endl;
			return;
		}

		if (m_tablero->puedeMover(fromFila, fromCol, cr, cc)) {
			bool combat = m_tablero->muevePieza(fromFila, fromCol, cr, cc);
			if (combat)
				cout << "¡COMBATE! (pantalla de combate pendiente)" << endl;
		}
		else {
			cout << "Movimiento inválido: casilla ocupada por aliado." << endl;
		}

		// En cualquier caso, deseleccionamos
		piezaSeleccionada = false;
		fromFila = fromCol = -1;
	}
}

void Tablerogl::KeyDown(unsigned char key)
{
	if (key == 27) {
		if (piezaSeleccionada) { piezaSeleccionada = false; fromFila = fromCol = -1; }
		else exit(0);
		return;
	}
	if (key == 'q' || key == 'Q') { exit(0); return; }

	// WASD: cursor LOCAL
	int& rL = Filacursor[0]; int& cL = Colcursor[0];
	if (key == 'w' || key == 'W') { if (rL > 0)   rL--; }
	if (key == 's' || key == 'S') { if (rL < N - 1) rL++; }
	if (key == 'a' || key == 'A') { if (cL > 0)   cL--; }
	if (key == 'd' || key == 'D') { if (cL < N - 1) cL++; }

	if (key == ' ')  trySelectorMove(bando_local);
	if (key == 13)   trySelectorMove(bando_rival); // Enter
}

void Tablerogl::SpecialKey(int key)
{
	int& rR = Filacursor[1]; int& cR = Colcursor[1];
	if (key == GLUT_KEY_UP && rR > 0)   rR--;
	if (key == GLUT_KEY_DOWN && rR < N - 1) rR++;
	if (key == GLUT_KEY_LEFT && cR > 0)   cR--;
	if (key == GLUT_KEY_RIGHT && cR < N - 1) cR++;
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

	if (!piezaSeleccionada) {
		if (clicked.pieza != pieza_nada) {
			int idx = (clicked.bando == bando_local) ? 0 : 1;
			Filacursor[idx] = clickFila; Colcursor[idx] = clickCol;
			trySelectorMove(clicked.bando);
		}
	}
	else {
		int idx = (fromBando == bando_local) ? 0 : 1;
		Filacursor[idx] = clickFila; Colcursor[idx] = clickCol;
		trySelectorMove(fromBando);
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
