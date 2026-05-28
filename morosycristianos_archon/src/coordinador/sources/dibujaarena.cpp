// DIBUJAARENA — IMPLEMENTACIÓN DE GRÁFICOS DEL COMBATE

#include "dibujaarena.h"
#include<iostream>
#include "freeglut.h"
#include "ETSIDI.h"
#include <cstdio> 
#include <cstring> 
#include <string>


dibujapersonajes DibujaArena::_personajes;

float DibujaArena::_tiempoFlash = 0.0f;
bool  DibujaArena::_flashActivo = true;
float DibujaArena::_duracionFlash = 3.0f;
float DibujaArena::_duracionFade = 0.6f;

// ORQUESTADORES PRINCIPALES

void DibujaArena::arena_configurar_vista(int anchoVentana, int altoVentana) 
{
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
    arena_suelo(arena.ancho(), arena.profundo(), batalla);

    //investigar que significa bien lo de lambda (aparece aqui y en dibujatablero)
    // Lambda que dibuja cualquier combatiente con sprite
    auto dibujarCombatiente = [&](const Combatiente& c, int indice) {

        //std::cout << "nombre='" << c.nombre() << "'" << std::endl;


        // Proyectamos posicion 3D a coordenadas de pantalla
        GLdouble winX, winY, winZ;
        GLdouble model[16], proj[16];
        GLint view[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        gluProject(c.x(), c.y(), c.z(), model, proj, view,
            &winX, &winY, &winZ);


        //definición de estados según reciba daño o este atacando
        EstadoPersonaje estado;
        if (c.recibioDanio()) estado = EstadoPersonaje::HURT;
        else if (c.atacando())     estado = EstadoPersonaje::ATTACK;
        else                       estado = EstadoPersonaje::IDLE;

        // Tipo de personaje según nombre
        //FALTAN PERSONAJES
        //añadir el resto cuando los tengamos

        bool voltear = false; 
        TipoPersonaje tipo = TipoPersonaje::MILICIANO;

        if (c.nombre() == "Miliciano") {
            tipo = TipoPersonaje::MILICIANO;
            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Rey") {
            tipo = TipoPersonaje::REY;
            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }

        else if (c.nombre() == "Asesino de Elite") {
            tipo = TipoPersonaje::ASESINO_DE_ELITE;
            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());


            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Guardia Negra") {
            tipo = TipoPersonaje::GUARDIA_NEGRA;
            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Jinete Bereber") {
            tipo = TipoPersonaje::JINETE_BEREBER;
            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;

        }
        else if (c.nombre() == "Emir") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::EMIR;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Infanteria") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::INFANTERIA;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Infiltrado") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::INFILTRADO;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Caballeria Pesada") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::CABALLERIA_PESADA;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Ballestero") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::BALLESTERO;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;

        }
        else if (c.nombre() == "Ghazi") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::GHAZI;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Caballeria Ligera") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::CABALLERIA_LIGERA;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Caballeria Acorazada") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::CABALLERIA_ACORAZADA;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        else if (c.nombre() == "Arquero") {
            //devuelve el tipo de personaje EMIR
            tipo = TipoPersonaje::ARQUERO_GHAZI;

            //si es atacante (P1 SIMEPRE ATACA) devuelve true
            bool esAtacante = (&c == &arena.p1());

            //si no es atacante, voltea (va a estar en el lado derecho de la arena, necesita girarse)
            voltear = !esAtacante;
        }
        float size = _anchoVentana * 0.10f;

        //convertir a 2d
        util_entrar2D(_anchoVentana, _altoVentana);
        glDisable(GL_LIGHTING);


        //dibjar con funcion de dibujappersonajes teniendo en cuenta el nombre del personaje y el estado
        _personajes.dibujar(tipo, (float)winX, (float)winY, size,
            estado, indice, c.enMovimiento(), voltear);
        util_salir2D();
    };


    float r, g, b;

    // P1 (ATACANTE)
    
    arena.p1().color(r, g, b);
    if (arena.p1().atacando())
        arena_hitbox(arena.p1().x() + arena.p1().lado() * 0.5f, arena.p1().z(), 1.0f, arena.p1().alcanceAtaque());
    //funcion dibujar combatiente
    dibujarCombatiente(arena.p1(), 0);

    

    // P2 (ATACADO)
    arena.p2().color(r, g, b);
    if (arena.p2().atacando())
        arena_hitbox(arena.p2().x() - arena.p2().lado() * 0.5f, arena.p2().z(), -1.0f, arena.p2().alcanceAtaque());
    dibujarCombatiente(arena.p2(), 1);

  
    // HUD ENCIMA DE TODO
    arena_hud(arena, batalla);

    // FLASH DE INICIO (encima del HUD)
    util_entrar2D(_anchoVentana, _altoVentana);
    glDisable(GL_LIGHTING);
    arena_flash_inicio(batalla);
    util_salir2D();
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

void DibujaArena::arena_suelo(float ancho, float profundo, Batalla batalla) 
{
    float halfX = ancho * 0.5f;
    float halfZ = profundo * 0.5f;

	// SUELO (con diferente textura segun la batalla)
    const char* ruta = "";
    switch (batalla) {
    case Batalla::GUADALETE:    ruta = "imagenes/suelo_guadalete.png"; break;
    case Batalla::ALARCOS:      ruta = "imagenes/suelo_alarcos.png";   break;
    case Batalla::NAVAS_TOLOSA: ruta = "imagenes/suelo_navas.png";     break;
    case Batalla::GRANADA:      ruta = "imagenes/suelo_granada.png";   break;
    }

    auto tex = ETSIDI::getTexture(ruta);

    glDisable(GL_LIGHTING);
    if (tex.id != 0) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glColor3f(1, 1, 1);
    }
    else 
    {
        glColor3f(0.85f, 0.78f, 0.62f);
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0, 0); glVertex3f(-halfX, 0.0f, -halfZ);
    glTexCoord2f(4, 0); glVertex3f(halfX, 0.0f, -halfZ);
    glTexCoord2f(4, 4); glVertex3f(halfX, 0.0f, halfZ);
    glTexCoord2f(0, 4); glVertex3f(-halfX, 0.0f, halfZ);
    glEnd();

    if (tex.id != 0) glDisable(GL_TEXTURE_2D);

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

void DibujaArena::arena_barra_vida(float x, float y, float ancho, float alto, float fraccion, bool invertir) {
    // COLOR DINÁMICO según fracción de vida
    float r, g, b;
    if (fraccion > 0.6f) {
        // Verde → Amarillo  (de 1.0 a 0.6)
        float t = (fraccion - 0.6f) / 0.4f;   // 1 en llena, 0 en 60%
        r = 1.0f - t;   // sube el rojo
        g = 0.82f;
        b = 0.1f;
    }
    else if (fraccion > 0.3f) {
        // Amarillo → Naranja  (de 0.6 a 0.3)
        float t = (fraccion - 0.3f) / 0.3f;
        r = 1.0f;
        g = 0.82f * t;
        b = 0.0f;
    }
    else {
        // Naranja → Rojo  (de 0.3 a 0.0)
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
    }

    // FONDO OSCURO
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);        glVertex2f(x + ancho, y);
    glVertex2f(x + ancho, y + alto); glVertex2f(x, y + alto);
    glEnd();

    // RELLENO (orientación según jugador)
    float xIni, xFin;
    if (!invertir) {
        xIni = x;
        xFin = x + ancho * fraccion;
    }
    else {
        xIni = x + ancho * (1.0f - fraccion);
        xFin = x + ancho;
    }

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(xIni, y);        glVertex2f(xFin, y);
    glVertex2f(xFin, y + alto); glVertex2f(xIni, y + alto);
    glEnd();

    // BORDE
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);        glVertex2f(x + ancho, y);
    glVertex2f(x + ancho, y + alto); glVertex2f(x, y + alto);
    glEnd();
}

void DibujaArena::arena_hud(const Arena& arena, Batalla batalla) {
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

    // P1, hacia la derecha, no se invierte
    float fracP1 = arena.p1().vida() / arena.p1().vidaMax();
    arena_barra_vida(margen, margen, anchoBarra, altoBarra, fracP1, false);
    std::string etiquetaP1 = arena.p1().nombre() + " - WASD + F";
    arena_texto(margen, margen + altoBarra + 16, etiquetaP1.c_str(), 1.0f, 1.0f, 1.0f);

    // P2, hacia la izquierda, si se invierte
    float fracP2 = arena.p2().vida() / arena.p2().vidaMax();
    float xP2 = _anchoVentana - margen - anchoBarra;
    arena_barra_vida(xP2, margen, anchoBarra, altoBarra, fracP2, true);
    std::string etiquetaP2 = arena.p2().nombre() + (arena.iaActiva() ? "- IA" : "- Flechas + L ");
    arena_texto(xP2, margen + altoBarra + 16, etiquetaP2.c_str(), 1.0f, 1.0f, 1.0f);

    // MENSAJE DE FIN
    if (arena.resultado() != ResultadoCombate::EnCurso) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f((float)_anchoVentana, 0);
        glVertex2f((float)_anchoVentana, (float)_altoVentana); glVertex2f(0, (float)_altoVentana);
        glEnd();

        //const char* msg = "";
        std::string msgStr = "";
        if (arena.resultado() == ResultadoCombate::GanaP1) msgStr = "Gana " + arena.p1().nombre() + "!  -  ENTER para volver al tablero";
        else if (arena.resultado() == ResultadoCombate::GanaP2) msgStr = "Gana " + arena.p2().nombre() + "!- ENTER para volver al tablero";
        else msgStr = "Empate!  -  ENTER para volver al tablero";

        float xMsg = _anchoVentana * 0.5f - strlen(msgStr.c_str()) * 4.5f;
        arena_texto(xMsg, _altoVentana * 0.5f, msgStr.c_str(), 1.0f, 1.0f, 1.0f);
    }

    // Panel nombre de la batalla
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(_anchoVentana - 280, _altoVentana - 140);
    glVertex2f(_anchoVentana, _altoVentana - 140);
    glVertex2f(_anchoVentana, _altoVentana);
    glVertex2f(_anchoVentana - 280, _altoVentana);
    glEnd();
    glDisable(GL_BLEND);

    ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
    ETSIDI::printxy("  QUE COMIENCE LA BATALLA!", _anchoVentana - 250, _altoVentana - 95);
    ETSIDI::printxy("    mucha suerte combatientes", _anchoVentana - 250, _altoVentana - 40);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void DibujaArena::arena_init() 
{
    _personajes.init();
}

void DibujaArena::arena_flash_inicio(Batalla batalla)
{
    if (!_flashActivo) return;

    // Calcular alpha según fase del fade
    float alpha = 1.0f;
    if (_tiempoFlash < _duracionFade) {
        // Fade IN
        alpha = _tiempoFlash / _duracionFade;
    }
    else if (_tiempoFlash > _duracionFlash - _duracionFade) {
        // Fade OUT
        alpha = (_duracionFlash - _tiempoFlash) / _duracionFade;
    }
    alpha = std::max(0.0f, std::min(1.0f, alpha)); // clamp

    // Nombre de la batalla
    const char* nombreBatalla = "";
    switch (batalla) {
    case Batalla::GUADALETE:    nombreBatalla = "BATALLA DE GUADALETE - 711 d.C.";    break;
    case Batalla::ALARCOS:      nombreBatalla = "BATALLA DE ALARCOS - 1195 d.C.";     break;
    case Batalla::NAVAS_TOLOSA: nombreBatalla = "BATALLA DE LAS NAVAS - 1212 d.C.";  break;
    case Batalla::GRANADA:      nombreBatalla = "BATALLA DE GRANADA - 1492 d.C.";     break;
    }

    float cx = _anchoVentana * 0.5f;
    float cy = _altoVentana * 0.5f;
    float anchoPanel = 550.0f;
    float altoPanel = 100.0f;

    // Fondo semitransparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.72f * alpha);
    glBegin(GL_QUADS);
    glVertex2f(cx - anchoPanel * 0.5f, cy - altoPanel * 0.5f);
    glVertex2f(cx + anchoPanel * 0.5f, cy - altoPanel * 0.5f);
    glVertex2f(cx + anchoPanel * 0.5f, cy + altoPanel * 0.5f);
    glVertex2f(cx - anchoPanel * 0.5f, cy + altoPanel * 0.5f);
    glEnd();

    // Borde dorado
    glColor4f(0.85f, 0.70f, 0.25f, alpha);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(cx - anchoPanel * 0.5f, cy - altoPanel * 0.5f);
    glVertex2f(cx + anchoPanel * 0.5f, cy - altoPanel * 0.5f);
    glVertex2f(cx + anchoPanel * 0.5f, cy + altoPanel * 0.5f);
    glVertex2f(cx - anchoPanel * 0.5f, cy + altoPanel * 0.5f);
    glEnd();
    glDisable(GL_BLEND);

    // Texto: nombre de la batalla (dorado)
    float xNombre = cx - strlen(nombreBatalla) * 4.5f;
    arena_texto(xNombre, cy - 12.0f, nombreBatalla, 0.85f, 0.70f, 0.25f);

    // Texto: A COMBATIR (blanco)
    const char* combatir = " ¡A COMBATIR! ";
    float xCombatir = cx - strlen(combatir) * 4.5f;
    arena_texto(xCombatir, cy + 18.0f, combatir, 1.0f, 1.0f, 1.0f);
}

void DibujaArena::arena_update(float dt) 
{
    _personajes.update();
    if (_flashActivo) {
        _tiempoFlash += dt;
        if (_tiempoFlash >= _duracionFlash)
            _flashActivo = false;
    }
}