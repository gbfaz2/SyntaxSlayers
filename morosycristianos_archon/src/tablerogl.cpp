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

}

void Tablerogl::DibujaCasilla(int fila, int col)
{
}

void Tablerogl::setCasillaColor(int fila, int col, bool dark)
{
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

