// Autor: Ines Alcérreca Sánchez
// DibujaMenu — todo el dibujo de PantallaIntro y MenuPrincipal

#include "dibujamenu.h"
#include "freeglut.h"
#include <cmath>
#include <string>
#include "pantallabatalla.h"
#include "ETSIDI.h"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================
// HELPERS LOCALES
// Deben ir ANTES de cualquier función que los use
// ============================================================

// RECTÁNGULO RELLENO SIN ESQUINAS REDONDEADAS (ranking y tablas)
static void menu_rect(float x, float y, float w, float h,
    float r, float g, float b, float a = 1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_BLEND);
}

// BORDE RECTO SIN ESQUINAS REDONDEADAS (ranking y tablas)
static void menu_borde_recto(float x, float y, float w, float h,
    float r, float g, float b, float grosor = 1.0f) {
    glLineWidth(grosor);
    glColor3f(r, g, b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
    glLineWidth(1.0f);
}

// RECTÁNGULO CON ESQUINAS REDONDEADAS (botones del menú)
static void dibujarRectRedondeado(float x, float y, float w, float h, float r,
    float cr, float cg, float cb, float ca) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(cr, cg, cb, ca);
    const int seg = 8;
    glBegin(GL_POLYGON);
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + r + cosf(a) * r, y + r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI * 1.5f + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + w - r + cosf(a) * r, y + r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + w - r + cosf(a) * r, y + h - r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI / 2.0f + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + r + cosf(a) * r, y + h - r + sinf(a) * r); }
    glEnd();
    glDisable(GL_BLEND);
}

// BORDE CON ESQUINAS REDONDEADAS (botones del menú)
static void dibujarBordeRedondeado(float x, float y, float w, float h, float r,
    float cr, float cg, float cb, float grosor) {
    glLineWidth(grosor);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(cr, cg, cb);
    const int seg = 8;
    glBegin(GL_LINE_LOOP);
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + r + cosf(a) * r, y + r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI * 1.5f + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + w - r + cosf(a) * r, y + r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + w - r + cosf(a) * r, y + h - r + sinf(a) * r); }
    for (int i = 0;i <= seg;i++) { float a = (float)M_PI / 2.0f + (i / (float)seg) * (float)M_PI / 2.0f;glVertex2f(x + r + cosf(a) * r, y + h - r + sinf(a) * r); }
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

static ETSIDI::SpriteSequence* s_gifInicio = nullptr;

// ============================================================
// PANTALLA INTRO
// ============================================================

void DibujaMenu::intro_dibujar(PantallaIntro& p, int ancho, int alto) {
    p.m_fotograma++;
    if (p.m_fotograma >= PantallaIntro::DURACION) { p.m_terminado = true; return; }
    float t = p.m_fotograma / (float)PantallaIntro::DURACION;
    float alfa = 1.0f;
    if (t < 0.25f) alfa = t / 0.25f;
    else if (t > 0.75f) alfa = (1.0f - t) / 0.25f;
    util_entrar2D(ancho, alto);
    if (!s_gifInicio)
        s_gifInicio = new ETSIDI::SpriteSequence("imagenes\\GIFT_GOT.png", 5, 9, 80, true, 0, 0, 480, 269);
    s_gifInicio->setPos(200.0f, 50.0f);
    s_gifInicio->setSize((float)ancho * 1.15f, (float)alto * 1.15f);
    s_gifInicio->loop(); s_gifInicio->draw();
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    ETSIDI::setFont("fuentes\\ALGER.TTF", 40);
    ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, alfa);
    ETSIDI::printxy("MOROS Y CRISTIANOS", ancho / 2 - 270, alto / 2 + 30);
    ETSIDI::setFont("fuentes\\BRUSHSCI.TTF", 28);
    ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, alfa);
    ETSIDI::printxy("La Reconquista", ancho / 2 - 110, alto / 2 - 20);
    if (p.m_fotograma > PantallaIntro::DURACION / 2) {
        float parpadeo = (sinf(p.m_fotograma * 0.1f) + 1.0f) * 0.5f * alfa;
        std::string skip = "Pulsa cualquier tecla para continuar";
        int pk = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_12, (const unsigned char*)skip.c_str());
        glColor4f(0.7f, 0.7f, 0.7f, parpadeo);
        glRasterPos2f(ancho / 2.0f - pk / 2.0f, alto / 4.0f);
        for (char c : skip) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    glDisable(GL_BLEND);
    util_salir2D();
}

// ============================================================
// PRIMITIVAS COMPARTIDAS
// ============================================================

void DibujaMenu::menu_texto(const std::string& texto,
    float x, float y, float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : texto) glutBitmapCharacter(fuente, c);
}

void DibujaMenu::menu_textoCentrado(const std::string& texto,
    float cx, float y, float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    int w = glutBitmapLength((unsigned char*)fuente, (const unsigned char*)texto.c_str());
    menu_texto(texto, cx - w / 2.0f, y, r, g, b, fuente);
}

// BOTÓN CON ESQUINAS REDONDEADAS — dorado al hacer hover
void DibujaMenu::menu_opcion(const std::string& texto,
    float x, float y, float ancho, float alto, bool seleccionada) {
    if (seleccionada)
        dibujarRectRedondeado(x, y, ancho, alto, 10.0f, 0.85f, 0.70f, 0.10f, 0.25f);
    else
        dibujarRectRedondeado(x, y, ancho, alto, 10.0f, 0.08f, 0.08f, 0.08f, 0.50f);
    if (seleccionada)
        dibujarBordeRedondeado(x, y, ancho, alto, 10.0f, 0.95f, 0.80f, 0.10f, 2.0f);
    else
        dibujarBordeRedondeado(x, y, ancho, alto, 10.0f, 0.50f, 0.50f, 0.50f, 1.0f);
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 16);
    if (seleccionada) ETSIDI::setTextColor(1.0f, 1.0f, 0.55f, 1.0f);
    else              ETSIDI::setTextColor(0.80f, 0.80f, 0.80f, 1.0f);
    ETSIDI::printxy(texto.c_str(), (int)(x + 12), (int)(y + alto / 2.0f - 8));
}

// ============================================================
// MENÚ PRINCIPAL — ORQUESTADOR
// ============================================================

void DibujaMenu::menu_dibujar(MenuPrincipal& m, int ancho, int alto) {
    m.m_fotograma++;
    util_entrar2D(ancho, alto);
    menu_fondo(ancho, alto);
    menu_titulo(m, ancho, alto);
    switch (m.m_paso) {
<<<<<<< Updated upstream
    case 0: menu_paso0(m, ancho, alto); break;
    case 1: menu_paso_nombre(m, ancho, alto); break;
    case 2: menu_paso1(m, ancho, alto); break;
    case 3: menu_paso_nombre(m, ancho, alto); break;
    case 4: menu_paso2(m, ancho, alto); break;
    case 5: menu_paso3(m, ancho, alto); break;
=======
    case 0: menu_paso0(m, ancho, alto); break; // MENÚ PRINCIPAL
    case 1: menu_paso1(m, ancho, alto); break; // ELEGIR MODO
    case 2: menu_paso2(m, ancho, alto); break; // CONFIGURACIÓN
    case 3: menu_paso3(m, ancho, alto); break; // BATALLA
    case 4: menu_paso4(m, ancho, alto); break; // CONFIRMAR
>>>>>>> Stashed changes
    }
    menu_pie(ancho, alto);
    util_salir2D();
}

// ============================================================
// FONDO Y TÍTULO
// ============================================================

void DibujaMenu::menu_fondo(int ancho, int alto) {
    // DEGRADADO: AZUL OSCURO ARRIBA → ROJO OSCURO ABAJO
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(0, alto);
    glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(ancho, alto);
    glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(ancho, 0);
    glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(0, 0);
    glEnd();
}

void DibujaMenu::menu_titulo(MenuPrincipal& m, int ancho, int alto) {
    float brillo = (sinf(m.m_fotograma * 0.04f) + 1.0f) * 0.15f;
    // TÍTULO CON FUENTE ALGER Y BRILLO ANIMADO
    ETSIDI::setFont("fuentes\\ALGER.TTF", 36);
    ETSIDI::setTextColor(0.85f + brillo, 0.70f + brillo * 0.5f, 0.10f, 1.0f);
    ETSIDI::printxy("+ MOROS Y CRISTIANOS )", ancho / 2 - 280, alto - 60);
    // SUBTÍTULO
    ETSIDI::setFont("fuentes\\BRUSHSCI.TTF", 20);
    ETSIDI::setTextColor(0.75f, 0.75f, 0.75f, 1.0f);
    ETSIDI::printxy("- La Reconquista -", ancho / 2 - 100, alto - 95);
    // LÍNEA DECORATIVA DORADA
    glLineWidth(1.5f);
    glColor3f(0.85f + brillo, 0.70f, 0.10f);
    glBegin(GL_LINES);
    glVertex2f(ancho * 0.05f, alto - 108); glVertex2f(ancho * 0.95f, alto - 108);
    glEnd();
    glLineWidth(1.0f);
}

void DibujaMenu::menu_pie(int ancho, int alto) {
    menu_textoCentrado(
        "[Flechas/Raton] Navegar       [ENTER/Click] Seleccionar       [ESC] Volver",
        ancho / 2.0f, 12, 0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
}

<<<<<<< Updated upstream

// ══════════════════════════════════════════════════════════════════════════
// PRIMITIVAS COMPARTIDAS DEL MENÚ
// ══════════════════════════════════════════════════════════════════════════

void DibujaMenu::menu_paso_nombre(MenuPrincipal& m, int ancho, int alto) {
    bool esJ2 = (m.m_paso == 3);
    std::string prompt = esJ2
        ? "Introduce tu nombre, Jugador 2:"
        : "Introduce tu nombre, Jugador 1:";

    menu_textoCentrado(prompt, ancho / 2.0f, alto / 2.0f + 80,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float bw = 400, bh = 50;
    float bx = ancho / 2.0f - bw / 2.0f;
    float by = alto / 2.0f - bh / 2.0f;
    menu_rectangulo(bx, by, bw, bh, 0.08f, 0.08f, 0.08f, 0.70f);
    menu_borde(bx, by, bw, bh, 0.85f, 0.70f, 0.10f, 2.0f);

    std::string texto = m.m_nombreActual;
    if ((m.m_fotograma / 30) % 2 == 0) texto += "_";
    menu_texto(texto, bx + 12, by + bh / 2.0f - 6,
        1.0f, 1.0f, 1.0f, GLUT_BITMAP_HELVETICA_18);

    menu_textoCentrado("Pulsa ENTER para confirmar",
        ancho / 2.0f, alto / 2.0f - 70,
        0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
}

void DibujaMenu::menu_texto(const std::string& texto,
    float x, float y,
    float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : texto) glutBitmapCharacter(fuente, c);
}

void DibujaMenu::menu_textoCentrado(const std::string& texto,
    float cx, float y,
    float r, float g, float b, void* fuente) {
    if (!fuente) fuente = GLUT_BITMAP_HELVETICA_18;
    int ancho = glutBitmapLength((unsigned char*)fuente,
        (const unsigned char*)texto.c_str());
    menu_texto(texto, cx - ancho / 2.0f, y, r, g, b, fuente);
}

void DibujaMenu::menu_rectangulo(float x, float y,
    float ancho, float alto,
    float r, float g, float b, float alfa) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, alfa);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + ancho, y);
    glVertex2f(x + ancho, y + alto);
    glVertex2f(x, y + alto);
=======
// ============================================================
// PASO 0 — MENÚ PRINCIPAL
// Opciones a la izquierda, derecha reservada para imagen futura
// ============================================================

void DibujaMenu::menu_paso0(MenuPrincipal& m, int ancho, int alto) {
    float aw = 300, ah = 44, sep = 10;
    float sx = ancho * 0.08f;      // PEGADO A LA IZQUIERDA
    float sy = alto / 2.0f + 100; // CENTRADAS VERTICALMENTE

    const char* opciones[] = {
        "  Cargar Partida",
        "  Nueva Partida",
        "  Ayuda",
        "  Ranking",
        "  Salir"
    };
    for (int i = 0; i < 5; i++)
        menu_opcion(opciones[i], sx, sy - i * (ah + sep), aw, ah, m.m_seleccion == i);

    // AVISO SI NO HAY PARTIDA AL SELECCIONAR "CARGAR"
    if (m.m_seleccion == 0 && !GestorPartida::hayPartidaGuardada()) {
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 13);
        ETSIDI::setTextColor(0.9f, 0.3f, 0.3f, 1.0f);
        ETSIDI::printxy("No hay partida guardada", (int)sx, (int)(sy + 22));
    }

    // LÍNEA VERTICAL SEPARADORA
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.85f, 0.70f, 0.10f, 0.3f); glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(ancho * 0.42f, alto * 0.15f); glVertex2f(ancho * 0.42f, alto * 0.85f);
>>>>>>> Stashed changes
    glEnd();
    glDisable(GL_BLEND);
}

// ============================================================
// PASO 1 — ELEGIR MODO (JVJ / JVIA)
// ============================================================

void DibujaMenu::menu_paso1(MenuPrincipal& m, int ancho, int alto) {
    menu_textoCentrado("Selecciona el modo de juego:",
        ancho / 2.0f, alto - 130, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
    float aw = 260, ah = 55, sep = 30;
    float sy = alto / 2.0f + 10;
    menu_opcion("  Jugador vs Jugador", ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah, m.m_seleccion == 0);
    menu_opcion("  Jugador vs IA", ancho / 2.0f + sep / 2.0f, sy, aw, ah, m.m_seleccion == 1);
}

// ============================================================
// PASO 2 — CONFIGURACIÓN UNIFICADA (dos columnas)
//
// JVJ:  J1 (nombre + bando) | J2 (nombre + bando opuesto auto)
// JVIA: J1 (nombre + cristiano fijo) | IA (selección dificultad)
// ============================================================

void DibujaMenu::menu_paso2(MenuPrincipal& m, int ancho, int alto) {
    bool esJVIA = (m.m_cfg.modo == ModoJuego::JVIA);
    float mitad = ancho / 2.0f;
    float colW = mitad * 0.80f;
    float col1X = mitad - colW - 20;
    float col2X = mitad + 20;
    float topY = alto * 0.72f;

    // LÍNEA CENTRAL SEPARADORA
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.85f, 0.70f, 0.10f, 0.6f); glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(mitad, alto * 0.20f); glVertex2f(mitad, alto * 0.85f);
    glEnd();
    glDisable(GL_BLEND);

    // CABECERAS
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 22);
    ETSIDI::setTextColor(1.0f, 0.90f, 0.30f, 1.0f); // DORADO BRILLANTE
    ETSIDI::printxy("JUGADOR 1", (int)col1X, (int)(topY + 10));
    ETSIDI::printxy(esJVIA ? "IA" : "JUGADOR 2", (int)col2X, (int)(topY + 10));

    // LÍNEAS BAJO CABECERAS
    glColor3f(1.0f, 0.85f, 0.20f); glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(col1X, topY - 5); glVertex2f(col1X + colW, topY - 5);
    glVertex2f(col2X, topY - 5); glVertex2f(col2X + colW, topY - 5);
    glEnd();

    float campoY = topY - 50;

    // ── NOMBRE J1 ────────────────────────────────────────────
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.90f, 0.85f, 0.60f, 1.0f); // DORADO SUAVE VISIBLE
    ETSIDI::printxy("Nombre:", (int)col1X, (int)(campoY + 20));

    bool focoJ1 = (m.m_focoNombre == 0);
    dibujarRectRedondeado(col1X, campoY - 30, colW, 36, 6.0f,
        focoJ1 ? 0.20f : 0.10f, focoJ1 ? 0.20f : 0.10f, focoJ1 ? 0.20f : 0.10f, 0.90f);
    dibujarBordeRedondeado(col1X, campoY - 30, colW, 36, 6.0f,
        focoJ1 ? 1.0f : 0.60f, focoJ1 ? 0.85f : 0.60f, focoJ1 ? 0.10f : 0.60f, focoJ1 ? 2.5f : 1.5f);
    std::string txtJ1 = m.m_nombreJ1Actual;
    if (focoJ1 && (m.m_fotograma / 30) % 2 == 0) txtJ1 += "_";
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 16);
    ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
    ETSIDI::printxy(txtJ1.c_str(), (int)(col1X + 8), (int)(campoY - 12));

    // ── BANDO J1 ─────────────────────────────────────────────
    float bandoY = campoY - 80;
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.90f, 0.85f, 0.60f, 1.0f);
    ETSIDI::printxy("Bando:", (int)col1X, (int)(bandoY + 20));

    if (esJVIA) {
        dibujarRectRedondeado(col1X, bandoY - 30, colW, 36, 6.0f, 0.75f, 0.20f, 0.05f, 0.70f);
        dibujarBordeRedondeado(col1X, bandoY - 30, colW, 36, 6.0f, 1.0f, 0.50f, 0.10f, 2.5f);
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 15);
        ETSIDI::setTextColor(1.0f, 0.90f, 0.65f, 1.0f);
        ETSIDI::printxy("CRISTIANO (fijo)", (int)(col1X + 8), (int)(bandoY - 12));
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 12);
        ETSIDI::setTextColor(0.85f, 0.75f, 0.45f, 1.0f); // VISIBLE
        ETSIDI::printxy("La IA siempre juega como Andalusi", (int)col1X, (int)(bandoY - 52));
    }
    else {
        float bw = (colW - 10) / 2.0f;
        bool crisSel = (m.m_cfg.bando == BandoJugador::CRISTIANO);

        // BOTÓN CRISTIANO J1
        dibujarRectRedondeado(col1X, bandoY - 30, bw, 36, 6.0f,
            crisSel ? 0.75f : 0.12f, crisSel ? 0.15f : 0.12f, crisSel ? 0.05f : 0.12f,
            crisSel ? 0.85f : 0.50f);
        dibujarBordeRedondeado(col1X, bandoY - 30, bw, 36, 6.0f,
            crisSel ? 1.0f : 0.55f, crisSel ? 0.40f : 0.55f, crisSel ? 0.10f : 0.55f,
            crisSel ? 2.5f : 1.5f);
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(crisSel ? 1.0f : 0.75f, crisSel ? 0.90f : 0.75f, crisSel ? 0.65f : 0.75f, 1.0f);
        ETSIDI::printxy("Cristiano", (int)(col1X + 6), (int)(bandoY - 12));

        // BOTÓN ANDALUSÍ J1
        float bx2 = col1X + bw + 10;
        bool andSel = !crisSel;
        dibujarRectRedondeado(bx2, bandoY - 30, bw, 36, 6.0f,
            andSel ? 0.30f : 0.12f, andSel ? 0.05f : 0.12f, andSel ? 0.65f : 0.12f,
            andSel ? 0.85f : 0.50f);
        dibujarBordeRedondeado(bx2, bandoY - 30, bw, 36, 6.0f,
            andSel ? 0.70f : 0.55f, andSel ? 0.30f : 0.55f, andSel ? 1.0f : 0.55f,
            andSel ? 2.5f : 1.5f);
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(andSel ? 0.85f : 0.75f, andSel ? 0.65f : 0.75f, andSel ? 1.0f : 0.75f, 1.0f);
        ETSIDI::printxy("Andalusi", (int)(bx2 + 6), (int)(bandoY - 12));
    }

    // ── COLUMNA DERECHA ───────────────────────────────────────
    if (esJVIA) {
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(0.90f, 0.85f, 0.60f, 1.0f);
        ETSIDI::printxy("Dificultad:", (int)col2X, (int)(campoY + 20));

        const char* niveles[] = {
            "  Facil   - IA principiante",
            "  Medio   - IA equilibrada",
            "  Dificil - IA avanzada"
        };
        NivelDificultad nivelesEnum[] = {
            NivelDificultad::FACIL, NivelDificultad::MEDIO, NivelDificultad::DIFICIL
        };
        for (int i = 0; i < 3; i++) {
            bool sel = (m.m_cfg.dificultad == nivelesEnum[i]);
            float dy = campoY - 30 - i * 46;
            dibujarRectRedondeado(col2X, dy, colW, 36, 6.0f,
                sel ? 0.85f : 0.12f, sel ? 0.70f : 0.12f, sel ? 0.10f : 0.12f,
                sel ? 0.40f : 0.55f);
            dibujarBordeRedondeado(col2X, dy, colW, 36, 6.0f,
                sel ? 1.0f : 0.55f, sel ? 0.85f : 0.55f, sel ? 0.10f : 0.55f,
                sel ? 2.5f : 1.5f);
            ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
            ETSIDI::setTextColor(sel ? 1.0f : 0.80f, sel ? 1.0f : 0.80f, sel ? 0.55f : 0.80f, 1.0f);
            ETSIDI::printxy(niveles[i], (int)(col2X + 8), (int)(dy + 12));
        }
    }
    else {
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(0.90f, 0.85f, 0.60f, 1.0f);
        ETSIDI::printxy("Nombre:", (int)col2X, (int)(campoY + 20));

        bool focoJ2 = (m.m_focoNombre == 1);
        dibujarRectRedondeado(col2X, campoY - 30, colW, 36, 6.0f,
            focoJ2 ? 0.20f : 0.10f, focoJ2 ? 0.20f : 0.10f, focoJ2 ? 0.20f : 0.10f, 0.90f);
        dibujarBordeRedondeado(col2X, campoY - 30, colW, 36, 6.0f,
            focoJ2 ? 1.0f : 0.60f, focoJ2 ? 0.85f : 0.60f, focoJ2 ? 0.10f : 0.60f, focoJ2 ? 2.5f : 1.5f);
        std::string txtJ2 = m.m_nombreJ2Actual;
        if (focoJ2 && (m.m_fotograma / 30) % 2 == 0) txtJ2 += "_";
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 16);
        ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
        ETSIDI::printxy(txtJ2.c_str(), (int)(col2X + 8), (int)(campoY - 12));

        float bandoY2 = campoY - 80;
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(0.90f, 0.85f, 0.60f, 1.0f);
        ETSIDI::printxy("Bando:", (int)col2X, (int)(bandoY2 + 20));

        bool j2Cris = (m.m_cfg.bando != BandoJugador::CRISTIANO);
        float bw = (colW - 10) / 2.0f;

        // BOTÓN CRISTIANO J2
        dibujarRectRedondeado(col2X, bandoY2 - 30, bw, 36, 6.0f,
            j2Cris ? 0.75f : 0.12f, j2Cris ? 0.15f : 0.12f, j2Cris ? 0.05f : 0.12f,
            j2Cris ? 0.85f : 0.50f);
        dibujarBordeRedondeado(col2X, bandoY2 - 30, bw, 36, 6.0f,
            j2Cris ? 1.0f : 0.55f, j2Cris ? 0.40f : 0.55f, j2Cris ? 0.10f : 0.55f,
            j2Cris ? 2.5f : 1.5f);
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(j2Cris ? 1.0f : 0.75f, j2Cris ? 0.90f : 0.75f, j2Cris ? 0.65f : 0.75f, 1.0f);
        ETSIDI::printxy("Cristiano", (int)(col2X + 6), (int)(bandoY2 - 12));

        // BOTÓN ANDALUSÍ J2
        float bx2 = col2X + bw + 10;
        bool j2And = !j2Cris;
        dibujarRectRedondeado(bx2, bandoY2 - 30, bw, 36, 6.0f,
            j2And ? 0.30f : 0.12f, j2And ? 0.05f : 0.12f, j2And ? 0.65f : 0.12f,
            j2And ? 0.85f : 0.50f);
        dibujarBordeRedondeado(bx2, bandoY2 - 30, bw, 36, 6.0f,
            j2And ? 0.70f : 0.55f, j2And ? 0.30f : 0.55f, j2And ? 1.0f : 0.55f,
            j2And ? 2.5f : 1.5f);
        ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 14);
        ETSIDI::setTextColor(j2And ? 0.85f : 0.75f, j2And ? 0.65f : 0.75f, j2And ? 1.0f : 0.75f, 1.0f);
        ETSIDI::printxy("Andalusi", (int)(bx2 + 6), (int)(bandoY2 - 12));
    }

    // BOTÓN CONTINUAR
    float btnW = 200, btnH = 40;
    float btnX = ancho / 2.0f - btnW / 2.0f;
    float btnY = alto * 0.12f;
    menu_opcion("  Continuar ->", btnX, btnY, btnW, btnH, true);

    // INSTRUCCIONES — AHORA VISIBLES
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 13);
    if (m.m_cfg.modo == ModoJuego::JVJ)
        menu_textoCentrado("TAB: cambiar campo de texto   |   Flechas: cambiar bando",
            ancho / 2.0f, alto * 0.07f, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
    else
        menu_textoCentrado("Escribe tu nombre y pulsa ENTER para continuar",
            ancho / 2.0f, alto * 0.07f, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
// PASO 3 — SELECCIÓN DE BATALLA
// ============================================================

void DibujaMenu::menu_paso3(MenuPrincipal& m, int ancho, int alto) {
    menu_textoCentrado("Elige la batalla historica:",
        ancho / 2.0f, alto - 130, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);
    float aw = 360, ah = 48, sep = 12;
    float sx = ancho / 2.0f - aw / 2.0f;
    float sy = alto / 2.0f + 90;
    const char* batallas[] = {
        "  Batalla de Guadalete (711)",
        "  Batalla de Alarcos (1195)",
        "  Las Navas de Tolosa (1212)",
        "  Reconquista de Granada (1492)"
    };
    for (int i = 0; i < 4; i++)
        menu_opcion(batallas[i], sx, sy - i * (ah + sep), aw, ah, m.m_seleccion == i);
    const char* descripciones[] = {
        "Los musulmanes cruzan el estrecho y vencen al rey Rodrigo.",
        "Al-Mansur derrota a Alfonso VIII de Castilla.",
        "Los reinos cristianos unidos rompen el poder almohade.",
        "Los Reyes Catolicos completan la Reconquista peninsular."
    };
    menu_textoCentrado(descripciones[m.m_seleccion],
        ancho / 2.0f, alto / 2.0f - 150, 0.65f, 0.65f, 0.65f, GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
// PASO 4 — CONFIRMAR (resumen + JUGAR / VOLVER)
// ============================================================

void DibujaMenu::menu_paso4(MenuPrincipal& m, int ancho, int alto) {
    std::string modo = (m.m_cfg.modo == ModoJuego::JVJ) ? "Jugador vs Jugador" : "Jugador vs IA";
    std::string bando = (m.m_cfg.bando == BandoJugador::CRISTIANO) ? "Cristiano" : "Andalusi";

    menu_textoCentrado("Modo:    " + modo,
        ancho / 2.0f, alto / 2.0f + 80, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    menu_textoCentrado("J1:      " + m.m_cfg.nombre_j1 + " (" + bando + ")",
        ancho / 2.0f, alto / 2.0f + 50, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    if (m.m_cfg.modo == ModoJuego::JVJ) {
        std::string b2 = (m.m_cfg.bando_j2 == BandoJugador::CRISTIANO) ? "Cristiano" : "Andalusi";
        menu_textoCentrado("J2:      " + m.m_cfg.nombre_j2 + " (" + b2 + ")",
            ancho / 2.0f, alto / 2.0f + 20, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    }
    else {
        std::string dif = (m.m_cfg.dificultad == NivelDificultad::FACIL) ? "Facil" :
            (m.m_cfg.dificultad == NivelDificultad::MEDIO) ? "Medio" : "Dificil";
        menu_textoCentrado("IA:      Dificultad " + dif,
            ancho / 2.0f, alto / 2.0f + 20, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    }
    menu_textoCentrado("Batalla: " + std::string(nombreBatalla(m.m_cfg.batalla)),
        ancho / 2.0f, alto / 2.0f - 10, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float aw = 180, ah = 44, sep = 30;
    float cy = alto / 2.0f - 70;
    menu_opcion("  JUGAR", ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah, m.m_seleccion == 0);
    menu_opcion("  VOLVER", ancho / 2.0f + sep / 2.0f, cy, aw, ah, m.m_seleccion == 1);
}

// ============================================================
// PANTALLA DESTINO — igual que el original
// ============================================================

void DibujaMenu::destino_dibujar(PantallaDestino& p, int ancho, int alto) {
    p.actualizar(ancho, alto);
    util_entrar2D(ancho, alto);
    destino_fondo(p, ancho, alto);
    destino_efectos(p, ancho, alto);
    destino_particulas(p);
    destino_textos(p, ancho, alto);
    destino_continuar(p, ancho, alto);
    util_salir2D();
}

void DibujaMenu::destino_fondo(const PantallaDestino& p, int ancho, int alto) {
    const char* rutas[] = {
        "imagenes\\GUADALETE.png","imagenes\\ALARCOS.png",
        "imagenes\\NAVAS_TOLOSA.png","imagenes\\GRANADA.png"
    };
    auto tex = ETSIDI::getTexture(rutas[(int)p.m_cfg.batalla]);
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, tex.id); glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, (float)alto); glTexCoord2f(1, 0); glVertex2f((float)ancho, (float)alto);
    glTexCoord2f(1, 1); glVertex2f((float)ancho, 0); glTexCoord2f(0, 1); glVertex2f(0, 0);
    glEnd(); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.45f);
    glBegin(GL_QUADS); glVertex2f(0, 0); glVertex2f((float)ancho, 0); glVertex2f((float)ancho, (float)alto); glVertex2f(0, (float)alto); glEnd();
    glDisable(GL_BLEND);
}

void DibujaMenu::destino_efectos(const PantallaDestino& p, int ancho, int alto) {
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    switch (p.m_cfg.batalla) {
    case Batalla::GUADALETE:
        glLineWidth(1.5f);
        for (int j = 0;j < 5;j++) {
            float yBase = (float)alto * 0.10f + j * 16.0f, fase = (float)p.m_fotograma * 0.04f + j * 1.3f;
            glColor4f(0.85f, 0.55f, 0.10f, 0.12f); glBegin(GL_LINE_STRIP);
            for (int x = 0;x <= ancho;x += 10) { float y = yBase + sinf((float)x * 0.018f + fase) * 6.0f; glVertex2f((float)x, y); }
            glEnd();
        } glLineWidth(1.0f); break;
    case Batalla::NAVAS_TOLOSA: {
        float cx = (float)ancho * 0.5f, ty = (float)alto + 10.0f;
        for (int r = 0;r < 10;r++) {
            float t = (r / 9.0f) - 0.5f, angA = t * 1.4f, angB = (t + 1.0f / 9.0f) * 1.4f;
            float pulse = (sinf((float)p.m_fotograma * 0.03f + r * 0.7f) + 1.0f) * 0.5f;
            glColor4f(1.0f, 0.95f, 0.70f, 0.05f + pulse * 0.07f); float len = (float)ancho * 1.5f;
            glBegin(GL_TRIANGLES); glVertex2f(cx, ty);
            glVertex2f(cx + sinf(angA) * len, ty - cosf(angA) * len);
            glVertex2f(cx + sinf(angB) * len, ty - cosf(angB) * len); glEnd();
        } break;
    }
    default: break;
    }
    glDisable(GL_BLEND);
}

void DibujaMenu::destino_particulas(const PantallaDestino& p) {
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glEnable(GL_POINT_SMOOTH);
    for (const auto& part : p.m_particulas) {
        glPointSize(part.tam); glColor4f(part.r, part.g, part.b, part.alfa);
        glBegin(GL_POINTS); glVertex2f(part.x, part.y); glEnd();
    }
    glDisable(GL_POINT_SMOOTH); glPointSize(1.0f); glDisable(GL_BLEND);
}

void DibujaMenu::destino_textos(const PantallaDestino& p, int ancho, int alto) {
    float alfa = p.alfaEntrada(), brillo = (sinf((float)p.m_fotograma * 0.04f) + 1.0f) * 0.10f;
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 24);
    ETSIDI::setTextColor((0.85f + brillo) * alfa, (0.70f + brillo * 0.5f) * alfa, 0.10f * alfa, alfa);
    {
        const char* tituloBat = nombreBatalla(p.m_cfg.batalla);
        int tw = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)tituloBat);
        ETSIDI::printxy(tituloBat, ancho / 2 - (int)(tw * 0.80f), alto - 85);
    }
    glColor4f(0.85f * alfa, 0.68f * alfa, 0.10f * alfa, 0.65f * alfa); glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f((float)ancho / 2.0f - 220.0f, (float)alto - 112.0f);
    glVertex2f((float)ancho / 2.0f + 220.0f, (float)alto - 112.0f);
    glEnd(); glLineWidth(1.0f);
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 18);
    int letrasRestantes = p.m_letraVisible;
    for (int i = 0;i < (int)p.m_lineas.size();i++) {
        if (letrasRestantes <= 0) break;
        int lineLen = (int)p.m_lineas[i].size();
        int mostrar = std::min(letrasRestantes, lineLen);
        letrasRestantes -= lineLen;
        std::string visible = p.m_lineas[i].substr(0, mostrar);
        ETSIDI::setTextColor(0.92f, 0.88f, 0.80f, alfa);
        int lw = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)p.m_lineas[i].c_str());
        ETSIDI::printxy(visible.c_str(), ancho / 2 - lw / 2, alto - 160 - i * 26);
    }
    glDisable(GL_BLEND);
}

void DibujaMenu::destino_continuar(const PantallaDestino& p, int ancho, int alto) {
    if (!p.textoCompleto()) return;
    float parpadeo = (sinf((float)p.m_fotograma * 0.08f) + 1.0f) * 0.5f;
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 12);
    ETSIDI::setTextColor(0.80f, 0.78f, 0.65f, 0.35f + parpadeo * 0.65f);
    const char* contTxt = "Pulsa cualquier tecla para comenzar la batalla";
    int cw = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)contTxt);
    ETSIDI::printxy(contTxt, ancho / 2 - cw / 2, 30);
}

// ============================================================
// RANKING
// ============================================================

void DibujaMenu::ranking_dibujar(int ancho, int alto,
    const std::string& ganador, const std::string& batalla,
    int turnos, int piezasLocal, int piezasRival,
    const std::vector<EntradaRanking>& ranking, bool ganaJ1)
{
    util_entrar2D(ancho, alto);
    menu_fondo(ancho, alto);
    menu_textoCentrado("RESULTADO FINAL", ancho / 2.0f, alto - 80, 0.85f, 0.70f, 0.10f, GLUT_BITMAP_TIMES_ROMAN_24);
    menu_textoCentrado("Batalla: " + batalla, ancho / 2.0f, alto - 130, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    menu_textoCentrado("Turnos: " + std::to_string(turnos), ancho / 2.0f, alto - 155, 0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float margen = ancho * 0.15f, tablaAncho = ancho * 0.70f, tablaY = alto - 200;
    float colAncho = tablaAncho / 3.0f, filaAlto = 40.0f;

    menu_rect(margen, tablaY - filaAlto, tablaAncho, filaAlto, 0.85f, 0.70f, 0.10f, 0.4f);
    menu_texto("Estadistica", margen + 10, tablaY - filaAlto + 12, 1, 1, 1, GLUT_BITMAP_HELVETICA_18);
    menu_texto("Cristiano", margen + colAncho + 10, tablaY - filaAlto + 12, 1, 1, 1, GLUT_BITMAP_HELVETICA_18);
    menu_texto("Andalusi", margen + colAncho * 2 + 10, tablaY - filaAlto + 12, 1, 1, 1, GLUT_BITMAP_HELVETICA_18);

    menu_rect(margen, tablaY - filaAlto * 2, tablaAncho, filaAlto, 0.08f, 0.08f, 0.08f, 0.5f);
    menu_texto("Resultado", margen + 10, tablaY - filaAlto * 2 + 12, 0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(ganaJ1 ? "GANADOR" : "perdedor", margen + colAncho + 10, tablaY - filaAlto * 2 + 12, ganaJ1 ? 1.0f : 0.5f, ganaJ1 ? 1.0f : 0.5f, 0, GLUT_BITMAP_HELVETICA_18);
    menu_texto(!ganaJ1 ? "GANADOR" : "perdedor", margen + colAncho * 2 + 10, tablaY - filaAlto * 2 + 12, !ganaJ1 ? 1.0f : 0.5f, !ganaJ1 ? 1.0f : 0.5f, 0, GLUT_BITMAP_HELVETICA_18);

    menu_rect(margen, tablaY - filaAlto * 3, tablaAncho, filaAlto, 0.12f, 0.12f, 0.12f, 0.5f);
    menu_texto("Piezas eliminadas", margen + 10, tablaY - filaAlto * 3 + 12, 0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(std::to_string(piezasLocal), margen + colAncho + 10, tablaY - filaAlto * 3 + 12, 0.89f, 0.29f, 0.29f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(std::to_string(piezasRival), margen + colAncho * 2 + 10, tablaY - filaAlto * 3 + 12, 0.39f, 0.60f, 0.13f, GLUT_BITMAP_HELVETICA_18);

    menu_borde_recto(margen, tablaY - filaAlto * 3, tablaAncho, filaAlto * 3, 0.85f, 0.70f, 0.10f, 2.0f);
    menu_textoCentrado("VICTORIA " + ganador + "!", ancho / 2.0f, tablaY - filaAlto * 4 - 20, 1.0f, 1.0f, 0.4f, GLUT_BITMAP_TIMES_ROMAN_24);
    menu_textoCentrado("Pulsa ESC para volver al menu", ancho / 2.0f, 30, 0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
    menu_textoCentrado("TOP 10", ancho / 2.0f, tablaY - filaAlto * 5 - 20, 0.85f, 0.70f, 0.10f, GLUT_BITMAP_HELVETICA_18);

    if (ranking.empty()) {
        menu_textoCentrado("No hay partidas registradas todavia",
            ancho / 2.0f, tablaY - filaAlto * 6 - 20, 0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_12);
    }
    else {
        for (int i = 0;i < (int)ranking.size();i++) {
            float fy = tablaY - filaAlto * 6 - 20 - i * 22;
            float r = 0.80f, g = 0.80f, b = 0.80f;
            if (i == 0) { r = 1.0f;g = 0.85f;b = 0.10f; }
            if (i == 1) { r = 0.80f;g = 0.80f;b = 0.80f; }
            if (i == 2) { r = 0.80f;g = 0.50f;b = 0.20f; }
            std::string linea = std::to_string(i + 1) + ". " + ranking[i].ganador + " - " + ranking[i].batalla + " - " + std::to_string(ranking[i].puntuacion) + " pts";
            menu_textoCentrado(linea, ancho / 2.0f, fy, r, g, b, GLUT_BITMAP_HELVETICA_12);
        }
    }
    util_salir2D();
}

// ============================================================
// VICTORIA
// ============================================================

void DibujaMenu::victoria_dibujar(int ancho, int alto,
    const std::string& ganador, const std::string& batalla,
    bool ganaJ1, float tiempoRestante)
{
    util_entrar2D(ancho, alto);
    const char* ruta = "imagenes/fondo_guadalete.png";
    if (batalla == "Guadalete")    ruta = "imagenes/fondo_guadalete.png";
    else if (batalla == "Alarcos") ruta = "imagenes/fondo_alarcos.png";
    else if (batalla == "Navas")   ruta = "imagenes/fondo_navas.png";
    else if (batalla == "Granada") ruta = "imagenes/fondo_granada.png";
    auto tex = ETSIDI::getTexture(ruta);
    if (tex.id != 0) {
        glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, tex.id); glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);glVertex2f(0, 0); glTexCoord2f(1, 0);glVertex2f(ancho, 0);
        glTexCoord2f(1, 1);glVertex2f(ancho, alto); glTexCoord2f(0, 1);glVertex2f(0, alto);
        glEnd(); glDisable(GL_TEXTURE_2D);
    }
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.55f);
    glBegin(GL_QUADS); glVertex2f(0, 0);glVertex2f(ancho, 0);glVertex2f(ancho, alto);glVertex2f(0, alto); glEnd();
    glDisable(GL_BLEND);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 52);
    if (ganaJ1) { ETSIDI::setTextColor(0.86f, 0.08f, 0.24f, 1.0f);ETSIDI::printxy("VICTORIA CRISTIANA", ancho / 2 - 240, alto / 2 + 60); }
    else { ETSIDI::setTextColor(0.55f, 0.10f, 0.75f, 1.0f);ETSIDI::printxy("VICTORIA AL-ANDALUS", ancho / 2 - 255, alto / 2 + 60); }
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 30); ETSIDI::setTextColor(1.0f, 0.95f, 0.80f, 1.0f);
    ETSIDI::printxy(("Ganador: " + ganador).c_str(), ancho / 2 - 120, alto / 2);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 18); ETSIDI::setTextColor(0.70f, 0.70f, 0.70f, 1.0f);
    char buf[32]; sprintf_s(buf, "Ranking en %d segundos...", (int)tiempoRestante + 1);
    ETSIDI::printxy(buf, ancho / 2 - 110, alto / 2 - 60);
    util_salir2D();
}

// ============================================================
// AYUDA
// ============================================================

void DibujaMenu::ayuda_dibujar(int seleccion, int seccion, int ancho, int alto)
{
    util_entrar2D(ancho, alto);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.85f);
    glBegin(GL_QUADS);glVertex2f(0, 0);glVertex2f(ancho, 0);glVertex2f(ancho, alto);glVertex2f(0, alto);glEnd();
    glDisable(GL_BLEND);

    if (seccion == -1) {
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 32);
        ETSIDI::printxy("AYUDA", ancho / 2 - 50, alto - 100);
        const char* ops[2] = { "Controles","Normas" };
        float btnW = 260, btnH = 48, btnX = ancho / 2.0f - btnW / 2.0f;
        float btnY[2] = { alto / 2.0f + 20,alto / 2.0f - 50 };
        for (int i = 0;i < 2;i++) {
            bool sel = (seleccion == i);
            dibujarRectRedondeado(btnX, btnY[i], btnW, btnH, 8.0f,
                sel ? 0.55f : 0.30f, sel ? 0.15f : 0.10f, sel ? 0.80f : 0.50f, 0.95f);
            dibujarBordeRedondeado(btnX, btnY[i], btnW, btnH, 8.0f, 0.85f, 0.70f, 0.25f, sel ? 2.5f : 1.5f);
            ETSIDI::setFont("fuentes/ARIALNBI.ttf", 18);
            if (sel) ETSIDI::setTextColor(1.0f, 1.0f, 0.0f, 1.0f);
            else     ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
            ETSIDI::printxy(ops[i], (int)(btnX + 80), (int)(btnY[i] + 14));
        }
        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver a pausa  |  ENTER: seleccionar", ancho / 2 - 160, 30);
    }
    else if (seccion == 0) {
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 24);
        ETSIDI::printxy("CONTROLES", ancho / 2 - 70, alto - 60);
        ETSIDI::setTextColor(1, 1, 1, 1); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 15);
        int x = ancho / 2 - 300, y = alto - 90, sep = 22;
        ETSIDI::printxy("TABLERO:", x, y);y -= sep;
        ETSIDI::printxy("  W/A/S/D         Mover cursor J1", x, y);y -= sep;
        ETSIDI::printxy("  ESPACIO         Seleccionar/Mover pieza J1", x, y);y -= sep;
        ETSIDI::printxy("  FLECHAS         Mover cursor J2", x, y);y -= sep;
        ETSIDI::printxy("  PUNTO (.)       Seleccionar/Mover pieza J2", x, y);y -= sep;
        ETSIDI::printxy("  H / J           Hechizos J1 / J2", x, y);y -= sep;
        ETSIDI::printxy("  1/2/3/4         Elegir hechizo activo", x, y);y -= sep;
        ETSIDI::printxy("  I               Habilidad Infiltrado", x, y);y -= sep;
        ETSIDI::printxy("  ESC             Menu pausa", x, y);y -= sep + 10;
        ETSIDI::printxy("ARENA:", x, y);y -= sep;
        ETSIDI::printxy("  W/A/S/D / F     Mover / Atacar J1", x, y);y -= sep;
        ETSIDI::printxy("  FLECHAS / L     Mover / Atacar J2 (JvJ)", x, y);y -= sep;
        ETSIDI::printxy("  ENTER           Volver al tablero", x, y);
        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver", ancho / 2 - 40, 30);
    }
    else if (seccion == 1) {
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 24);
        ETSIDI::printxy("NORMAS", ancho / 2 - 50, alto - 60);
        ETSIDI::setTextColor(1, 1, 1, 1); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 15);
        int x = ancho / 2 - 300, y = alto - 110, sep = 26;
        ETSIDI::printxy("OBJETIVO: Ganar por una de estas condiciones:", x, y);y -= sep;
        ETSIDI::printxy("  1. Controlar los 5 Bastiones del tablero.", x, y);y -= sep;
        ETSIDI::printxy("  2. Eliminar todas las piezas enemigas.", x, y);y -= sep;
        ETSIDI::printxy("  3. Dejar al rival con una sola pieza bloqueada.", x, y);y -= sep + 10;
        ETSIDI::printxy("TURNO: Cada jugador mueve una pieza. Si el tiempo acaba, pasa el turno.", x, y);y -= sep + 10;
        ETSIDI::printxy("COMBATE: Si dos piezas coinciden se abre la arena.", x, y);y -= sep;
        ETSIDI::printxy("  El ganador ocupa la casilla con la vida restante.", x, y);y -= sep + 10;
        ETSIDI::printxy("HECHIZOS (uso unico): Avituallamiento / Rutas / Relevo / Asedio", x, y);y -= sep + 10;
        ETSIDI::printxy("INFILTRADO: Selecciona + I para copiar stats de un enemigo.", x, y);y -= sep;
        ETSIDI::printxy("CASILLAS DINAMICAS: cambian de bando cada 4 turnos.", x, y);
        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f); ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver", ancho / 2 - 40, 30);
    }
    util_salir2D();
}