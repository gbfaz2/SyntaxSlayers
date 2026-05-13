//es nuestro punto de entrada. Ahora primero queremos dibujar el tablero vacío
#include "tablerogl.h"
#include "freeglut.h"
#include <iostream>
using namespace std;

Tablero tablero; //centralizamos la lógica, que crea los 9x9 tipos de casilla
Tablerogl scene(&tablero);//recibe puntero al tablero

//los callback, funciones que seran llamadas automaticamente por la glut
//cuando sucedan eventos
//NO HACE FALTA LLAMARLAS EXPLICITAMENTE
void OnDraw(void); //esta funcion sera llamada para dibujar
void OnTimer(int value); //esta funcion sera llamada cuando transcurra una temporizacion
void OnKeyboardDown(unsigned char key, int x, int y); //cuando se pulse una tecla	
void OnMouseClick(int b, int state, int x, int y);

//void OnSpecialKeyboardDown(int key, int x, int y);

int main(int argc, char* argv[])
{
	//Inicializar el gestor de ventanas GLUT
	//y crear la ventana
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Moros y Cristianos");

	//habilitar luces y definir perspectiva

	//Registrar los callbacks
	glutDisplayFunc(OnDraw);
	glutTimerFunc(25, OnTimer, 0);//le decimos que dentro de 25ms llame 1 vez a la funcion OnTimer()
	glutKeyboardFunc(OnKeyboardDown);
	glutMouseFunc(OnMouseClick);
	//glutSpecialFunc(OnSpecialKeyboardDown); //gestion de los cursores

	scene.init();

	//pasarle el control a GLUT,que llamara a los callbacks
	glutMainLoop();

	return 0;
}

/*void OnSpecialKeyboardDown(int key, int x, int y)
{
	mundo.tecla_especial(key);
}
*/
void OnDraw(void)
{
	//Borrado de la pantalla	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	scene.Dibuja();

	//no borrar esta linea ni poner nada despues
	glutSwapBuffers();
}
void OnKeyboardDown(unsigned char key, int x_t, int y_t)
{
	//poner aqui el código de teclado
	scene.KeyDown(key);

	glutPostRedisplay();
}
void OnMouseClick(int b, int state, int x, int y)
{
	bool down = (state == GLUT_DOWN);

	//convertimos la constante de glut al enum nuestro
	int button;
	if (b == GLUT_LEFT_BUTTON) button = MOUSE_LEFT_BUTTON;
	else if (b == GLUT_RIGHT_BUTTON) button = MOUSE_RIGHT_BUTTON;
	else button = MOUSE_MIDDLE_BUTTON;

	//Detectamos ctrl y shift
	int specialKey = glutGetModifiers();
	bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
	bool shiftkey = (specialKey & GLUT_ACTIVE_SHIFT) ? true : false;

	scene.MouseButton(x, y, button, down, shiftkey, ctrlKey);
	glutPostRedisplay();
}

#include <chrono>
using namespace std::chrono;

void OnTimer(int value)
{
	// calcular dt real entre llamadas usando std::chrono
	static auto last = high_resolution_clock::now();//Variable estatica que es global que se iniciializa la primera vez que se pasa por ese espacio se rellena con now cuando la hace automaticamente. Será siempre la misma last 
	auto now = high_resolution_clock::now();//now siempre se ejecuta y se destruye y así vemos el tiempo que pasa
	duration<double> elapsed = now - last;
	double dt = elapsed.count();//nos lo da en milisegundos o micro segundos y ya lo sabemos nosotros
	last = now;
	std::cout << "dt" << dt << std::endl;//para que en la consola aparezca el tiempo
	// código de animacion usando dt en segundos
	//mundo.mueve(dt);
	// no borrar estas líneas.indicamos que se vuelva a dibujar la
	//pantalla, para que se vean los cambios
	glutPostRedisplay();
	// recursivamente, le decimos que dentro de 25ms vuelva a llamar a
	//esta funcion, para que se siga animando
	glutTimerFunc(10, OnTimer, 0);
}