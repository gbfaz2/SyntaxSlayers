#include "ArenaRenderer.h"
#include "freeglut.h"
#include <cstdio> // Para sprintf (formatear texto)
#include <cstring> // Para strlen (calcular longitud de texto)

// Inicializacion de variables estaticas
int ArenaRenderer::_anchoVentana = 900;
int ArenaRenderer::_altoVentana = 600;

void ArenaRenderer::configurarVista(int anchoVentana, int altoVentana)
{
	_anchoVentana = anchoVentana;
	_altoVentana = (altoVentana > 0) ? altoVentana : 1; // Evitar division por cero

	glViewport(0, 0, _anchoVentana, _altoVentana);

	// Proyeccin 3D con perspectiva
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = (float)_anchoVentana / (float)_altoVentana;
	gluPerspective(45.0, aspect, 0.5, 100.0); //45 grados campo de vision, plano cercano 0.5, plano lejano 100

	glMatrixMode(GL_MODELVIEW);

	// Configuracion inicial de OpenGL
	glEnable(GL_DEPTH_TEST); // Habilitar prueba de profundidad para ocultar objetos detras de otros
	glEnable(GL_BLEND); // Habilitar blending para transparencia
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Para transparencias
	glShadeModel(GL_SMOOTH); // Sombreado sueave
	glEnable(GL_COLOR_MATERIAL); // Permitir que el color afecte a los materiales
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void ArenaRenderer::configurarLuz()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Luz suave para que no se vea todo negro
	GLfloat ambiente[] = { 0.35f, 0.35f, 0.35f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

	// Luz direccional desde arriba
	GLfloat posicion[] = { 0.4f, 1.0f, 0.6f, 0.0f };
	GLfloat difusa[] = { 0.9f, 0.9f, 0.85f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, posicion);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
}

void ArenaRenderer::dibujarSuelo(float ancho, float profundo) 
{
	float halfX = ancho * 0.5f;
	float halfZ = profundo * 0.5f;

	// Suelo color tierra
	glColor3f(0.85f, 0.78f, 0.62f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f); // Normal apuntando hacia arriba
	glVertex3f(-halfX, 0.0f, -halfZ);
	glVertex3f(halfX, 0.0f, -halfZ);
	glVertex3f(halfX, 0.0f, halfZ);
	glVertex3f(-halfX, 0.0f, halfZ);
	glEnd();

	// Cuadricula encima del suelo
	glDisable(GL_LIGHTING); // Deshabilitar luz para que la cuadricula se vea siempre
	glColor3f(0.5f, 0.45f, 0.35f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (float xi = -halfX; xi <= halfX + 0.001f; xi += 1.0f) 
	{
		glVertex3f(xi, 0.01f, -halfZ);
		glVertex3f(xi, 0.01f, halfZ);
	}
	for (float zi = -halfZ; zi <= halfZ + 0.001f; zi += 1.0f)
	{
		glVertex3f(-halfX, 0.01f, zi);
		glVertex3f(halfX, 0.01f, zi);
	}
	glEnd();

	// Borde de la arena
	glColor3f(0.35f, 0.25f, 0.15f);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-halfX, 0.02f, -halfZ);
	glVertex3f(halfX, 0.02f, -halfZ);
	glVertex3f(halfX, 0.02f, halfZ);
	glVertex3f(-halfX, 0.02f, halfZ);
	glEnd();
	glEnable(GL_LIGHTING); // Volver a habilitar luz
}

void ArenaRenderer::dibujarCubo3D(float x, float y, float z, float lado, float r, float g, float b)
{
	float h = lado * 0.5f; // La funcion glutSolidCube dibuja un cubo centrado en el origen, por lo que para posicionarlo correctamente hay que elevarlo la mitad de su lado
	glPushMatrix();
	glTranslatef(x, y + h, z); // Elevamos el cubo para que su base toque el suelo
	glColor3f(r, g, b);

	// Las 6 caras del cubo con sus normales para que la luz las ilumine correctamente
	glBegin(GL_QUADS);
	// cara superior
	glNormal3f(0, 1, 0);
	glVertex3f(-h, h, -h); glVertex3f(-h, h, h);
	glVertex3f(h, h, h); glVertex3f(h, h, -h);
	// cara inferior
	glNormal3f(0, -1, 0);
	glVertex3f(-h, -h, -h); glVertex3f(h, -h, -h);
	glVertex3f(h, -h, h); glVertex3f(-h, -h, h);
	// cara frontal
	glNormal3f(0, 0, 1);
	glVertex3f(-h, -h, h); glVertex3f(h, -h, h);
	glVertex3f(h, h, h); glVertex3f(-h, h, h);
	// cara trasera
	glNormal3f(0, 0, -1);
	glVertex3f(-h, -h, -h); glVertex3f(-h, h, -h);
	glVertex3f(h, h, -h); glVertex3f(h, -h, -h);
	// cara derecha
	glNormal3f(1, 0, 0);
	glVertex3f(h, -h, -h); glVertex3f(h, h, -h);
	glVertex3f(h, h, h); glVertex3f(h, -h, h);
	// cara izquierda
	glNormal3f(-1, 0, 0);
	glVertex3f(-h, -h, -h); glVertex3f(-h, -h, h);
	glVertex3f(-h, h, h); glVertex3f(-h, h, -h);
	glEnd();

	// Aristas el cubo
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glutWireCube(lado);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void ArenaRenderer::dibujarHitbox(float x, float z, float orientacion, float alcance) 
{
	// Blanco traslucido que muestra el area de ataque
	float anchoHit = alcance;
	float profHit = 0.8f;
	float xIni = (orientacion > 0) ? x : x - anchoHit;

	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 0.55f); //blanco transparente
	glBegin(GL_QUADS);
	glVertex3f(xIni, 0.05f, z - profHit * 0.5f);
	glVertex3f(xIni + anchoHit, 0.05f, z - profHit * 0.5f);
	glVertex3f(xIni+ anchoHit, 0.05f, z + profHit * 0.5f);
	glVertex3f(xIni, 0.05f, z + profHit * 0.5f);
	glEnd();
	glEnable(GL_LIGHTING);
}

void ArenaRenderer::dibujarTexto(float x, float y, const char* texto, float r, float g, float b) 
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y); // Posicion en coordenadas 2D de la pantalla
	for (const char* c = texto; *c != '\0'; ++c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void ArenaRenderer::dibujarBarraVida(float x, float y, float ancho, float alto, float fraccion, float r, float g, float b)
{
	// Fondo gris oscuro
	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + ancho, y);
	glVertex2f(x + ancho, y + alto);
	glVertex2f(x, y + alto);
	glEnd();

	// Vida actual (fraccion de 0 a 1)
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + ancho * fraccion, y);
	glVertex2f(x + ancho * fraccion, y + alto);
	glVertex2f(x, y + alto);
	glEnd();

	// Borde negro 
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + ancho, y);
	glVertex2f(x + ancho, y + alto);
	glVertex2f(x, y + alto);
	glEnd();
}

void ArenaRenderer::dibujarHUD(const Arena& arena)
{
	// Cambiamos a proyeccion 2D para dibujar encima de la escena 3D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, _anchoVentana, _altoVentana, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// Barras de vida
	float anchoBarra = 260.0f;
	float altoBarra = 18.0f;
	float margen = 20.0f;

	// P1 (Cristiano, rojo, arriba a la izquierda)
	float fracP1 = arena.p1().vida() / arena.p1().vidaMax();
	dibujarBarraVida(margen, margen, anchoBarra, altoBarra,
		fracP1, 0.89f, 0.29f, 0.29f);
	dibujarTexto(margen, margen + altoBarra + 16,
		"P1 Cristiano - WASD + F", 1.0f, 1.0f, 1.0f);

	// P2 (Andalusi, verde) - arriba derecha
	float fracP2 = arena.p2().vida() / arena.p2().vidaMax();
	float xP2 = _anchoVentana - margen - anchoBarra;
	dibujarBarraVida(xP2, margen, anchoBarra, altoBarra,
		fracP2, 0.39f, 0.60f, 0.13f);
	dibujarTexto(xP2, margen + altoBarra + 16,
		"P2 Andalusi - Flechas + L", 1.0f, 1.0f, 1.0f);

	// Mensaje de fin de combate
	if (arena.resultado() != ResultadoCombate::EnCurso) {
		// Velo oscuro sobre toda la pantalla
		glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
		glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f((float)_anchoVentana, 0);
		glVertex2f((float)_anchoVentana, (float)_altoVentana);
		glVertex2f(0, (float)_altoVentana);
		glEnd();

		const char* msg = "";
		if (arena.resultado() == ResultadoCombate::GanaP1)
			msg = "Gana el Cristiano!  -  R para reiniciar";
		else if (arena.resultado() == ResultadoCombate::GanaP2)
			msg = "Gana el Andalusi!  -  R para reiniciar";
		else
			msg = "Empate!  -  R para reiniciar";

		// Centrar el texto en pantalla
		float xMsg = _anchoVentana * 0.5f - strlen(msg) * 4.5f;
		dibujarTexto(xMsg, _altoVentana * 0.5f, msg, 1.0f, 1.0f, 1.0f);
	}

	// Restaurar estado 3D
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void ArenaRenderer::dibujar(const Arena& arena)
{
	// Fondo (azul cielo)
	glClearColor(0.55f, 0.65f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// Camara aerea-oblicua: elevada e inclinada mirando al centro
	gluLookAt(
		0.0, 9.0, 11.0,  // posicion de la camara (ojo)
		0.0, 0.0, 0.0,  // punto al que mira (centro de la arena)
		0.0, 1.0, 0.0   // vector "arriba" del mundo
	);

	configurarLuz();
	dibujarSuelo(arena.ancho(), arena.profundo());

	// Dibujar los combatientes
	float r, g, b;

	// P1
	arena.p1().color(r, g, b);
	if (arena.p1().atacando())
		dibujarHitbox(arena.p1().x() + arena.p1().lado() * 0.5f,
			arena.p1().z(), 1.0f, arena.p1().alcanceAtaque());
	dibujarCubo3D(arena.p1().x(), arena.p1().y(), arena.p1().z(),
		arena.p1().lado(), r, g, b);

	// P2
	arena.p2().color(r, g, b);
	if (arena.p2().atacando())
		dibujarHitbox(arena.p2().x() - arena.p2().lado() * 0.5f,
			arena.p2().z(), -1.0f, arena.p2().alcanceAtaque());
	dibujarCubo3D(arena.p2().x(), arena.p2().y(), arena.p2().z(),
		arena.p2().lado(), r, g, b);

	// HUD encima de todo
	dibujarHUD(arena);

	glutSwapBuffers(); // mostrar el frame en pantalla
}