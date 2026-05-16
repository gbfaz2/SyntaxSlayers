#pragma once
#include "Arena.h"

// Se encarga de dibujar la arena en 3D usando OpenGL y freeglut
// Es el unico sitio donde va a aparecer OpenGL

// La camara va a mirar al centro de la arena desde arriba y estraara
// ligeramente incrlinada como en el Archon Ultra

class ArenaRenderer
{
	// Dimensiones de la ventana
	static int _anchoVentana;
	static int _altoVentana;

	// Helpers internos
	static void configurarLuz();
	static void dibujarSuelo(float ancho, float profundo);
	static void dibujarCubo3D(float x, float y, float z, float lado, float r, float g, float b);
	static void dibujarHitbox(float x, float z, float orientacion, float alcance);
	static void dibujarHUD(const Arena& arena); // Dibuja la barra de vida y el resultado del combate (Heads up Display)
	static void dibujarTexto(float x, float y, const char* texto, float r = 1.0f, float g = 1.0f, float b = 1.0f); // Dibuja texto en pantalla usando glutBitmapCharacter
	static void dibujarBarraVida(float x, float y, float ancho, float alto, float fraccion, float r, float g, float b); // Dibuja una barra de vida con el porcentaje dado

public:
	// Configura la proyeccion 3D, se llama al inicio y en cada cambio de tamaño de ventana
	static void configurarVista(int anchoVentana, int altoVentana);

	// Dibuja un frame completo (suelo, combatientes, HUD y mensajes de fin)
	static void dibujar(const Arena& arena);
};