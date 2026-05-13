
//Autor: María Heredero

//aquí se va a dibujar el tablero. 
//Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
//Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include <iostream>
#include"freeglut.h"
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

void Tablerogl::Dibuja()//se llama cada frame desde Ondraw()
{
	//Recalculamos el centro. Por si N cambiara
	centro_x = N * ancho / 2.0f;
	centro_y = -N * ancho / 2.0f;


	// Debug temporal — bórralo cuando funcione
	cout << "[Draw] center=(" << centro_x << "," << centro_y
		<< ") dist=" << dist << " N=" << N << " width=" << ancho << endl;
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

	//primero elegimos el par de colores según la zona y luego cuák de los dos usar según dark de dibujarcasillas
	switch (tipo) {
	case Casilla_local:
		//morado claro u oscuro
		if (!dark) glColor3f(0.55f, 0.20f, 0.65f);//morado medio
		else glColor3f(0.30f, 0.05f, 0.40f);//morado oscuro
		break;
	case Casilla_rival:
		//rojo claro u oscuro
		if (!dark)
			glColor3f(0.75f, 0.15f, 0.15f);//rojo medio
		else
			glColor3f(0.45f, 0.05f, 0.05f);//rojo oscuro
		break;
	case Casilla_dinamica: {
		//interpolación lineal entre color oscuro y claro según la fase de esta casilla
		float fase = m_tablero->getCasilla(fila, col).fase;

		float r_dark, g_dark, b_dark;//color cuando fase es 0
		float r_light, g_light, b_light;//color cuando fase es 1

		if (!dark) {

			//casilla "clara" oscila entre azul medio y casi blanco
			r_dark = 0.25f; g_dark = 0.45f; b_dark = 0.75f;
			r_light = 0.75f; g_light = 0.88f; b_light = 1.00f;
		}
		else {
			//casilla "oscura" que oscila entre azul oscuro y azul medio
			r_dark = 0.10f; g_dark = 0.20f; b_dark = 0.50f;
			r_light = 0.45f; g_light = 0.65f; b_light = 0.90f;
		}

		//interpolo cada canal RGB por separado
		float r = r_dark + fase * (r_light - r_dark);
		float g = g_dark + fase * (g_light - g_dark);
		float b = b_dark + fase * (b_light - b_dark);

		glColor3f(r, g, b);
		break;
	}
	case Casilla_poder:
		if (!dark) glColor3f(0.40f, 0.60f, 0.80f);
		else glColor3f(0.20f, 0.35f, 0.60f);
		break;

	default:
		glColor3f(1.0f, 1.0f, 1.00f);//ponemos un blanco por seguridad
		break;
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
	glColor3f(1.0f, 1.0f, 1.0f); // blanco

	float R = size;           // radio del círculo exterior (la luna)
	float r = size * 0.75f;   // radio del círculo interior (el "mordisco")
	float dx = size * 0.35f;  // desplazamiento horizontal del círculo interior

	glBegin(GL_TRIANGLE_FAN);
	// Centro de la luna (para TRIANGLE_FAN)
	glVertex3f(cx, cy, 0.002f);

	for (int i = 0; i <= CIRCLE_SEGS; i++) {
		float angulo = 2.0f * (float)3.14159265f * i / CIRCLE_SEGS;
		float px = cx + R * cos(angulo); // punto en el círculo exterior
		float py = cy + R * sin(angulo);

		// ¿Está este punto fuera del círculo interior desplazado?
		float distInterior = sqrt((px - (cx + dx)) * (px - (cx + dx)) + (py - cy) * (py - cy));

		if (distInterior >= r) {
			// Sí: este punto pertenece a la media luna entonces lo añadimos
			glVertex3f(px, py, 0.002f);
		}
		else {
			// No: el círculo interior lo cubre entonces ponemos el borde interior
			// para cerrar el contorno de la media luna
			float ang2 = atan2(py - cy, px - (cx + dx));
			glVertex3f(cx + dx + r * cos(ang2), cy + r * sin(ang2), 0.002f);
		}
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

	//coordenadas mundo->casilla(fila, columna)
	world2cell(posX, posY, xcasilla_sel, ycasilla_sel);

	//Estado de botones y teclas modificadoras
	if (down) { controlKey = ctrlKey; this->shiftKey = shiftKey; }
	else { controlKey = this->shiftKey = false; }

	if (button == MOUSE_LEFT_BUTTON)leftButton = down;
	if (button == MOUSE_RIGHT_BUTTON)rightButton = down;
	if (button == MOUSE_MIDDLE_BUTTON)midButton = down;

	//Imprimimos la casilla por consola
	if (down && xcasilla_sel >= 0 && xcasilla_sel < N && ycasilla_sel >= 0 && ycasilla_sel < N)
		cout << "Casilla: (" << xcasilla_sel << "," << ycasilla_sel << ")" << endl;
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

