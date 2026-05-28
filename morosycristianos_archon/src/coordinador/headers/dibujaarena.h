// DIBUJAARENA — MOTOR GRÁFICO EXCLUSIVO PARA RENDERIZAR EL COMBATE 3D

#pragma once
#include "dibuja.h"
#include "Arena.h"
#include "estadojuego.h"
#include "dibujapersonajes.h"

class DibujaArena : public Dibuja 
{
    static dibujapersonajes _personajes;

    // CAPAS BASE DE LA ARENA 
    static void arena_configurar_luz();
    static void arena_suelo(float ancho, float profundo, Batalla batalla);
    static void arena_fondo(Batalla batalla);

    // PRIMITIVAS 3D Y DE COMBATE
    static void arena_cubo3d(float x, float y, float z, float lado, float r, float g, float b);
    static void arena_hitbox(float x, float z, float orientacion, float alcance);

    // HUD Y TEXTOS EN 2D
    static void arena_hud(const Arena& arena, Batalla batalla);
    static void arena_texto(float x, float y, const char* texto, float r = 1.0f, float g = 1.0f, float b = 1.0f);
    static void arena_barra_vida(float x, float y, float ancho, float alto, float fraccion, float r, float g, float b);

public:
    // ORQUESTADORES PRINCIPALES
    static void arena_configurar_vista(int anchoVentana, int altoVentana);
    static void arena_dibujar(const Arena& arena, Batalla batalla);
    static void arena_init();
	static void arena_update(float dt);
};