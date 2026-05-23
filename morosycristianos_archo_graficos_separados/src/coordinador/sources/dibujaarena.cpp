// DIBUJAARENA — IMPLEMENTACIÓN DE GRÁFICOS DEL COMBATE

#include "dibujaarena.h"
#include "freeglut.h"
#include "ETSIDI.h"
#include <cstdio> 
#include <cstring> 
#include <string>

// ORQUESTADORES PRINCIPALES

void DibujaArena::arena_configurar_vista(int anchoVentana, int altoVentana) {
    _anchoVentana = anchoVentana;
    _altoVentana = (altoVentana > 0) ? altoVentana : 1; // EVITAR DIVISIÓN POR CERO

    glViewport(0, 0, _anchoVentana, _altoVentana);

    // PROYECCIÓN 3D CON PERSPECTIVA
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)_anchoVentana / (float)_altoVentana;
    gluPerspective(45.0, aspect, 0.5, 100.0);

    glMatrixMode(GL_MODELVIEW);

    // CONFIGURACIÓN INICIAL DE OPENGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void DibujaArena::arena_dibujar(const Arena& arena, Batalla batalla) {
    // FONDO (AZUL CIELO)
    glClearColor(0.55f, 0.65f, 0.75f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // CÁMARA AÉREA-OBLICUA
    gluLookAt(
        0.0, 9.0, 11.0,  // POSICIÓN DE LA CÁMARA
        0.0, 0.0, 0.0,   // PUNTO AL QUE MIRA
        0.0, 1.0, 0.0    // VECTOR "ARRIBA"
    );

    arena_fondo(batalla);
    arena_configurar_luz();
    arena_suelo(arena.ancho(), arena.profundo());

    // DIBUJAR LOS COMBATIENTES
    float r, g, b;

    // P1
    arena.p1().color(r, g, b);
    if (arena.p1().atacando())
        arena_hitbox(arena.p1().x() + arena.p1().lado() * 0.5f, arena.p1().z(), 1.0f, arena.p1().alcanceAtaque());
    arena_cubo3d(arena.p1().x(), arena.p1().y(), arena.p1().z(), arena.p1().lado(), r, g, b);

    // P2
    arena.p2().color(r, g, b);
    if (arena.p2().atacando())
        arena_hitbox(arena.p2().x() - arena.p2().lado() * 0.5f, arena.p2().z(), -1.0f, arena.p2().alcanceAtaque());
    arena_cubo3d(arena.p2().x(), arena.p2().y(), arena.p2().z(), arena.p2().lado(), r, g, b);

    // HUD ENCIMA DE TODO
    arena_hud(arena);
}

// CAPAS BASE DE LA ARENA

void DibujaArena::arena_configurar_luz() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambiente[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

    GLfloat posicion[] = { 0.4f, 1.0f, 0.6f, 0.0f };
    GLfloat difusa[] = { 0.9f, 0.9f, 0.85f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, posicion);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
}

void DibujaArena::arena_suelo(float ancho, float profundo) {
    float halfX = ancho * 0.5f;
    float halfZ = profundo * 0.5f;

    // SUELO
    glColor3f(0.85f, 0.78f, 0.62f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-halfX, 0.0f, -halfZ);
    glVertex3f(halfX, 0.0f, -halfZ);
    glVertex3f(halfX, 0.0f, halfZ);
    glVertex3f(-halfX, 0.0f, halfZ);
    glEnd();

    // CUADRÍCULA
    glDisable(GL_LIGHTING);
    glColor3f(0.5f, 0.45f, 0.35f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float xi = -halfX; xi <= halfX + 0.001f; xi += 1.0f) {
        glVertex3f(xi, 0.01f, -halfZ);
        glVertex3f(xi, 0.01f, halfZ);
    }
    for (float zi = -halfZ; zi <= halfZ + 0.001f; zi += 1.0f) {
        glVertex3f(-halfX, 0.01f, zi);
        glVertex3f(halfX, 0.01f, zi);
    }
    glEnd();

    // BORDE DE LA ARENA
    glColor3f(0.35f, 0.25f, 0.15f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-halfX, 0.02f, -halfZ);
    glVertex3f(halfX, 0.02f, -halfZ);
    glVertex3f(halfX, 0.02f, halfZ);
    glVertex3f(-halfX, 0.02f, halfZ);
    glEnd();
    glEnable(GL_LIGHTING);
}

void DibujaArena::arena_fondo(Batalla batalla) {
    const char* ruta = "";
    switch (batalla) {
    case Batalla::GUADALETE:    ruta = "imagenes/fondo_guadalete.png"; break;
    case Batalla::ALARCOS:      ruta = "imagenes/fondo_alarcos.png";   break;
    case Batalla::NAVAS_TOLOSA: ruta = "imagenes/fondo_navas.png";     break;
    case Batalla::GRANADA:      ruta = "imagenes/fondo_granada.png";   break;
    }

    auto tex = ETSIDI::getTexture(ruta);
    if (tex.id == 0) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(1, 0);
    glTexCoord2f(1, 0); glVertex2f(1, 1);
    glTexCoord2f(0, 0); glVertex2f(0, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// PRIMITIVAS 3D Y DE COMBATE

void DibujaArena::arena_cubo3d(float x, float y, float z, float lado, float r, float g, float b) {
    float h = lado * 0.5f;
    glPushMatrix();
    glTranslatef(x, y + h, z);
    glColor3f(r, g, b);

    glBegin(GL_QUADS);
    // SUPERIOR
    glNormal3f(0, 1, 0);
    glVertex3f(-h, h, -h); glVertex3f(-h, h, h);
    glVertex3f(h, h, h); glVertex3f(h, h, -h);
    // INFERIOR
    glNormal3f(0, -1, 0);
    glVertex3f(-h, -h, -h); glVertex3f(h, -h, -h);
    glVertex3f(h, -h, h); glVertex3f(-h, -h, h);
    // FRONTAL
    glNormal3f(0, 0, 1);
    glVertex3f(-h, -h, h); glVertex3f(h, -h, h);
    glVertex3f(h, h, h); glVertex3f(-h, h, h);
    // TRASERA
    glNormal3f(0, 0, -1);
    glVertex3f(-h, -h, -h); glVertex3f(-h, h, -h);
    glVertex3f(h, h, -h); glVertex3f(h, -h, -h);
    // DERECHA
    glNormal3f(1, 0, 0);
    glVertex3f(h, -h, -h); glVertex3f(h, h, -h);
    glVertex3f(h, h, h); glVertex3f(h, -h, h);
    // IZQUIERDA
    glNormal3f(-1, 0, 0);
    glVertex3f(-h, -h, -h); glVertex3f(-h, -h, h);
    glVertex3f(-h, h, h); glVertex3f(-h, h, -h);
    glEnd();

    // ARISTAS
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glutWireCube(lado);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void DibujaArena::arena_hitbox(float x, float z, float orientacion, float alcance) {
    float anchoHit = alcance;
    float profHit = 0.8f;
    float xIni = (orientacion > 0) ? x : x - anchoHit;

    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex3f(xIni, 0.05f, z - profHit * 0.5f);
    glVertex3f(xIni + anchoHit, 0.05f, z - profHit * 0.5f);
    glVertex3f(xIni + anchoHit, 0.05f, z + profHit * 0.5f);
    glVertex3f(xIni, 0.05f, z + profHit * 0.5f);
    glEnd();
    glEnable(GL_LIGHTING);
}

// HUD Y TEXTOS EN 2D

void DibujaArena::arena_texto(float x, float y, const char* texto, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = texto; *c != '\0'; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void DibujaArena::arena_barra_vida(float x, float y, float ancho, float alto, float fraccion, float r, float g, float b) {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + ancho, y);
    glVertex2f(x + ancho, y + alto); glVertex2f(x, y + alto);
    glEnd();

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + ancho * fraccion, y);
    glVertex2f(x + ancho * fraccion, y + alto); glVertex2f(x, y + alto);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + ancho, y);
    glVertex2f(x + ancho, y + alto); glVertex2f(x, y + alto);
    glEnd();
}

void DibujaArena::arena_hud(const Arena& arena) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, _anchoVentana, _altoVentana, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    float anchoBarra = 260.0f;
    float altoBarra = 18.0f;
    float margen = 20.0f;

    // P1
    float fracP1 = arena.p1().vida() / arena.p1().vidaMax();
    arena_barra_vida(margen, margen, anchoBarra, altoBarra, fracP1, 0.89f, 0.29f, 0.29f);
    std::string etiquetaP1 = arena.p1().nombre() + " - WASD + F";
    arena_texto(margen, margen + altoBarra + 16, etiquetaP1.c_str(), 1.0f, 1.0f, 1.0f);

    // P2
    float fracP2 = arena.p2().vida() / arena.p2().vidaMax();
    float xP2 = _anchoVentana - margen - anchoBarra;
    arena_barra_vida(xP2, margen, anchoBarra, altoBarra, fracP2, 0.39f, 0.60f, 0.13f);
    std::string etiquetaP2 = arena.p2().nombre() + (arena.iaActiva() ? "- IA" : "- Flechas + L ");
    arena_texto(xP2, margen + altoBarra + 16, etiquetaP2.c_str(), 1.0f, 1.0f, 1.0f);

    // MENSAJE DE FIN
    if (arena.resultado() != ResultadoCombate::EnCurso) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f((float)_anchoVentana, 0);
        glVertex2f((float)_anchoVentana, (float)_altoVentana); glVertex2f(0, (float)_altoVentana);
        glEnd();

        const char* msg = "";
        if (arena.resultado() == ResultadoCombate::GanaP1) msg = "Gana el Cristiano!  -  ENTER para volver al tablero";
        else if (arena.resultado() == ResultadoCombate::GanaP2) msg = "Gana el Andalusi!  -  ENTER para volver al tablero";
        else msg = "Empate!  -  ENTER para volver al tablero";

        float xMsg = _anchoVentana * 0.5f - strlen(msg) * 4.5f;
        arena_texto(xMsg, _altoVentana * 0.5f, msg, 1.0f, 1.0f, 1.0f);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}