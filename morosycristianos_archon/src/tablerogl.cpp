//aquí se va a dibujar el tablero. 
//Se recorren las 81 casillas del tablero y por cada una decide si es clara(par) u oscura (impar), según el tipo de casillas elige el par de colores concreto y luego dibuja un rectángulo relleno con el color
//Por último se dibuja la cuadrícula negra encima

#include "tablerogl.h"
#include <iostream>
#include"freeglut.h"
using namespace std;

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

void Tablerogl::DibujaCuadricula()
{

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
	
	//puntos de poder de momento serán siempre dorados
	if (tipo == Casilla_poder) {
		glColor3f(1.0f, 0.82f, 0.10f);
		return;
	}

	//para el resto, primero elegimos el par de colores según la zona y luego cuák de los dos usar según dark de dibujarcasillas
	switch (tipo) {
	case Casilla_local:
	case Casilla_rival:
		if (!dark)
			glColor3f(0.45f, 0.65f, 0.85f); 
		else
			glColor3f(0.15f, 0.30f, 0.55f);
		break;
	case Casilla_dinamica:
		if (!dark)
			glColor3f(0.75f, 0.88f, 1.00f);
		else
			glColor3f(0.50f, 0.72f, 0.90f);
		break;
	default:
		glColor3f(1.0f, 1.0f, 1.00f);//ponemos un blanco por seguridad
		break;
	}

}

void Tablerogl::KeyDown(unsigned char key)
{
}

void Tablerogl::MouseButton(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey)
{
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

