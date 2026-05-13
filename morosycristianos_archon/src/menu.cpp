
// Autor: Ines Alcérreca Sánchez
// Implementación de las pantallas de introducción y menú principal

#include "menu.h"
#include "freeglut.h"
#include <cmath>
#include <string>

//  Funciones auxiliares de dibujo
void entrar2D(int ancho, int alto) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ancho, 0, alto);  // (0,0) = esquina inferior izquierda
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void salir2D() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void dibujarTexto(const std::string& texto, float x, float y,
                  float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : texto)
        glutBitmapCharacter(fuente, c);
}

void dibujarTextoCentrado(const std::string& texto, float cx, float y,
                           float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    int ancho = glutBitmapLength((unsigned char*)fuente,
                                 (const unsigned char*)texto.c_str());
    dibujarTexto(texto, cx - ancho / 2.0f, y, r, g, b, fuente);
}

void dibujarRectangulo(float x, float y, float ancho, float alto,
                       float r, float g, float b, float alfa) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, alfa);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + ancho, y);
        glVertex2f(x + ancho, y + alto);
        glVertex2f(x, y + alto);
    glEnd();
    glDisable(GL_BLEND);
}

void dibujarBorde(float x, float y, float ancho, float alto,
                  float r, float g, float b, float grosor) {
    glLineWidth(grosor);
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + ancho, y);
        glVertex2f(x + ancho, y + alto);
        glVertex2f(x, y + alto);
    glEnd();
    glLineWidth(1.0f);
}

//PANTALLA INTRO 
PantallaIntro::PantallaIntro() { reiniciar(); }

// Dibujo principal
void PantallaIntro::reiniciar() {
    m_fotograma = 0;
    m_terminado = false;
}

// Permite saltar la intro al pulsar cualquier tecla
void PantallaIntro::saltar() {
    m_terminado = true;
}

// Dibuja el fondo negro
void PantallaIntro::dibujar(int ancho, int alto) {
    m_fotograma++;
    if (m_fotograma >= DURACION) { m_terminado = true; return; }

    // Calculo del alfa para fade-in y fade-out
    float t = m_fotograma / (float)DURACION;
    float alfa = 1.0f;
    if (t < 0.25f)      alfa = t / 0.25f;
    else if (t > 0.75f) alfa = (1.0f - t) / 0.25f;

    entrar2D(ancho, alto);

    // Fondo negro
    dibujarRectangulo(0, 0, (float)ancho, (float)alto, 0, 0, 0, 1.0f);

    // Titulo principal
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::string titulo = "MOROS Y CRISTIANOS";
    int tw = glutBitmapLength((unsigned char*)GLUT_BITMAP_TIMES_ROMAN_24,
                              (const unsigned char*)titulo.c_str());
    glColor4f(0.85f * alfa, 0.70f * alfa, 0.10f * alfa, alfa);
    glRasterPos2f(ancho / 2.0f - tw / 2.0f, alto / 2.0f + 30);
    for (char c : titulo)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

    // Subtitulo
    std::string sub = "La Reconquista";
    int sw = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_18,
                              (const unsigned char*)sub.c_str());
    glColor4f(0.85f * alfa, 0.85f * alfa, 0.85f * alfa, alfa);
    glRasterPos2f(ancho / 2.0f - sw / 2.0f, alto / 2.0f);
    for (char c : sub)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    // Texto de skip (parpadea en la segunda mitad)
    if (m_fotograma > DURACION / 2) {
        float parpadeo = (sinf(m_fotograma * 0.1f) + 1.0f) * 0.5f * alfa; //si quiero q parpadee más rápido aumento la frecuencia del seno
        std::string skip = "Pulsa cualquier tecla para continuar";
        int pk = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_12,
                                  (const unsigned char*)skip.c_str());
        glColor4f(0.7f, 0.7f, 0.7f, parpadeo);
        glRasterPos2f(ancho / 2.0f - pk / 2.0f, alto / 4.0f);
        for (char c : skip)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glDisable(GL_BLEND);
    salir2D();
}


//  PANTALLA MENU PRINCIPAL

MenuPrincipal::MenuPrincipal() { reiniciar(); }

void MenuPrincipal::reiniciar() {
	m_paso = 0; // 0=modo, 1=bando, 2=confirmar
	m_seleccion = 0; // opcion resaltada
	m_fotograma = 0; // para animaciones
	m_terminado = false; // cuando el jugador confirma, se marca como terminado para que el main cambie de estado
	m_siguiente = EstadoJuego::MENU; // el estado al que se pasará cuando termine (puede ser MENU, TABLERO, ARENA, RANKING...)
	m_cfg = ConfigPartida(); // configuración de partida (modo, bando, batalla, turno1)
}

// Dibujo principal 
void MenuPrincipal::dibujar(int ancho, int alto) {
    m_fotograma++;
    entrar2D(ancho, alto);
    dibujarFondo(ancho, alto);
    dibujarTitulo(ancho, alto);
    switch (m_paso) {
        case 0: dibujarPaso0(ancho, alto); break;
        case 1: dibujarPaso1(ancho, alto); break;
        case 2: dibujarPaso2(ancho, alto); break;
    }
    dibujarPie(ancho, alto); 
    salir2D();
}

//Fondo degradado
void MenuPrincipal::dibujarFondo(int ancho, int alto) {
    glBegin(GL_QUADS);
        glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(0, alto);
        glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(ancho, alto);
        glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(ancho, 0);
        glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(0, 0);
    glEnd();
    // Linea dorada separadora
    glLineWidth(2.0f);
    glColor3f(0.85f, 0.70f, 0.10f);
    glBegin(GL_LINES);
        glVertex2f(ancho * 0.05f, alto * 0.70f);
        glVertex2f(ancho * 0.95f, alto * 0.70f);
    glEnd();
    glLineWidth(1.0f);
}

// Titulo con brillo animado
void MenuPrincipal::dibujarTitulo(int ancho, int alto) {
    float brillo = (sinf(m_fotograma * 0.04f) + 1.0f) * 0.12f;
    dibujarTextoCentrado("MOROS Y CRISTIANOS", ancho / 2.0f, alto - 70,
                         0.85f + brillo, 0.70f + brillo * 0.5f, 0.10f,
                         GLUT_BITMAP_TIMES_ROMAN_24);
    dibujarTextoCentrado("- La Reconquista -", ancho / 2.0f, alto - 100,
                         0.75f, 0.75f, 0.75f,
                         GLUT_BITMAP_HELVETICA_12);
}

// Paso 0: seleccion de modo 
void MenuPrincipal::dibujarPaso0(int ancho, int alto) {
    dibujarTextoCentrado("Selecciona el modo de juego:", ancho / 2.0f, alto - 135,
                         0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);

    float aw = 280, ah = 44, separacion = 14;
    float sx = ancho / 2.0f - aw / 2.0f;
    float sy = alto  / 2.0f + 60;

    const char* opciones[] = {
        "  Jugador vs Jugador",
        "  Jugador vs IA",
        "  Ranking",
        "  Salir"
    };
    for (int i = 0; i < 4; i++)
        dibujarOpcion(opciones[i], sx, sy - i * (ah + separacion),
                      aw, ah, m_seleccion == i);
}

// Paso 1: seleccion de bando
void MenuPrincipal::dibujarPaso1(int ancho, int alto) {
    std::string modotxt = (m_cfg.modo == ModoJuego::JVJ) ?
                          "Modo: Jugador vs Jugador" : "Modo: Jugador vs IA";
    dibujarTextoCentrado(modotxt, ancho / 2.0f, alto - 135,
                         0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
    dibujarTextoCentrado("Elige tu bando:", ancho / 2.0f, alto - 155,
                         0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_12);

	float aw = 240, ah = 55, separacion = 30; // tamaño de las cajas y separación entre ellas
	float sy = alto / 2.0f + 10; // coordenada y común a ambas opciones

    dibujarOpcion("  CRISTIANO ",
                  ancho / 2.0f - aw - separacion / 2.0f, sy,
                  aw, ah, m_seleccion == 0);
    dibujarOpcion("  MUSULMAN ",
                  ancho / 2.0f + separacion / 2.0f, sy,
                  aw, ah, m_seleccion == 1);
}

// Paso 2: confirmacion 
void MenuPrincipal::dibujarPaso2(int ancho, int alto) {
    std::string modo  = (m_cfg.modo  == ModoJuego::JVJ) ?
                        "Jugador vs Jugador" : "Jugador vs IA";
    std::string bando = (m_cfg.bando == Bando::CRISTIANO) ?
                        "Cristiano" : "Musulman";

    dibujarTextoCentrado("Modo:  " + modo,  ancho / 2.0f, alto / 2.0f + 70,
                         0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    dibujarTextoCentrado("Bando: " + bando, ancho / 2.0f, alto / 2.0f + 40,
                         0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float aw = 180, ah = 44, separacion = 30;
    float cy = alto / 2.0f - 20;
    dibujarOpcion("  JUGAR",
                  ancho / 2.0f - aw - separacion / 2.0f, cy,
                  aw, ah, m_seleccion == 0);
    dibujarOpcion("  VOLVER",
                  ancho / 2.0f + separacion / 2.0f, cy,
                  aw, ah, m_seleccion == 1);
}

// Pie de pagina 
void MenuPrincipal::dibujarPie(int ancho, int alto) {
    dibujarTextoCentrado(
        "[Flechas/Raton] Navegar       [ENTER/Click] Seleccionar       [ESC] Volver",
        ancho / 2.0f, 12, 0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
}

//  Caja de opcion
void MenuPrincipal::dibujarOpcion(const std::string& texto,
                                   float x, float y, float aw, float ah,
                                   bool seleccionada) {
    if (seleccionada)
        dibujarRectangulo(x, y, aw, ah, 0.85f, 0.70f, 0.10f, 0.25f);
    else
        dibujarRectangulo(x, y, aw, ah, 0.08f, 0.08f, 0.08f, 0.50f);

    if (seleccionada)
        dibujarBorde(x, y, aw, ah, 0.95f, 0.80f, 0.10f, 2.0f);
    else
        dibujarBorde(x, y, aw, ah, 0.35f, 0.35f, 0.35f, 1.0f);

    if (seleccionada)
        dibujarTexto(texto, x + 12, y + ah / 2.0f - 6,
                     1.0f, 1.0f, 0.55f, GLUT_BITMAP_HELVETICA_18);
    else
        dibujarTexto(texto, x + 12, y + ah / 2.0f - 6,
                     0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
}

// Input: teclado
void MenuPrincipal::teclaPulsada(unsigned char tecla) {
    if (tecla == 27) {  // TECLA ESC
        if (m_paso > 0) { m_paso--; m_seleccion = 0; }
        else { m_siguiente = EstadoJuego::INTRO; m_terminado = true; }
        return;
    }
    if (tecla == 13 || tecla == ' ') { confirmar(); return; }
}

void MenuPrincipal::teclaEspecial(int tecla) {
    if (tecla == GLUT_KEY_UP)
        m_seleccion = (m_seleccion - 1 + maxOpciones()) % maxOpciones();
    if (tecla == GLUT_KEY_DOWN)
        m_seleccion = (m_seleccion + 1) % maxOpciones();
    if (tecla == GLUT_KEY_LEFT  && m_paso == 1) m_seleccion = 0;
    if (tecla == GLUT_KEY_RIGHT && m_paso == 1) m_seleccion = 1;
}

// Input: raton 
void MenuPrincipal::ratonMovido(int mx, int my, int ancho, int alto) {
    int gy = alto - my;  // GLUT da y desde arriba, OpenGL desde abajo
    auto enCaja = [&](float x, float y, float aw, float ah) {
        return mx >= x && mx <= x + aw && gy >= y && gy <= y + ah;
    };
    if (m_paso == 0) {
        float aw = 280, ah = 44, sep = 14;
        float sx = ancho / 2.0f - aw / 2.0f, sy = alto / 2.0f + 60;
        for (int i = 0; i < 4; i++)
            if (enCaja(sx, sy - i * (ah + sep), aw, ah)) m_seleccion = i;
    } else if (m_paso == 1) {
        float aw = 240, ah = 55, sep = 30, sy = alto / 2.0f + 10;
        if (enCaja(ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah)) m_seleccion = 0;
        if (enCaja(ancho / 2.0f + sep / 2.0f,       sy, aw, ah)) m_seleccion = 1;
    } else if (m_paso == 2) {
        float aw = 180, ah = 44, sep = 30, cy = alto / 2.0f - 20;
        if (enCaja(ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah)) m_seleccion = 0;
        if (enCaja(ancho / 2.0f + sep / 2.0f,       cy, aw, ah)) m_seleccion = 1;
    }
}

void MenuPrincipal::ratonPulsado(int mx, int my, int ancho, int alto) {
    ratonMovido(mx, my, ancho, alto);
    confirmar();
}

// Confirmar seleccion y avanzar al siguiente paso o estado
void MenuPrincipal::confirmar() {
    if (m_paso == 0) {
        switch (m_seleccion) {
		case 0: m_cfg.modo = ModoJuego::JVJ;  m_paso = 1; m_seleccion = 0; break; // modo jugador vs jugador
		case 1: m_cfg.modo = ModoJuego::JVIA; m_paso = 1; m_seleccion = 0; break; // modo jugador vs IA
		case 2: m_siguiente = EstadoJuego::RANKING; m_terminado = true; break; // modo ranking
        case 3: m_siguiente = EstadoJuego::FINAL;   m_terminado = true; break; // modo salir 
        }
    } else if (m_paso == 1) {
        m_cfg.bando = (m_seleccion == 0) ? Bando::CRISTIANO : Bando::MUSULMAN;
        m_paso = 2; m_seleccion = 0;
    } else if (m_paso == 2) {
        if (m_seleccion == 0) {
            // Sortear la batalla al confirmar JUGAR
            m_cfg.batalla = sortearBatalla();
            m_cfg.turno1  = iniciativa(m_cfg.batalla);
            m_siguiente   = EstadoJuego::DESTINO;
            m_terminado   = true;
        } else {
            m_paso = 1; m_seleccion = 0;
        }
    }
}
