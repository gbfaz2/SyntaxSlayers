#include "dibujamenu.h"
#include "freeglut.h"
#include <cmath>
#include <string>
#include "pantallabatalla.h"
#include "ETSIDI.h" // NECESARIO PARA TEXTURAS Y TEXTOS
#include <algorithm>

static ETSIDI::SpriteSequence* s_gifInicio = nullptr;

// PANTALLA INTRO

void DibujaMenu::intro_dibujar(PantallaIntro& p, int ancho, int alto) {
    p.m_fotograma++;
    if (p.m_fotograma >= PantallaIntro::DURACION) { p.m_terminado = true; return; }

    float t = p.m_fotograma / (float)PantallaIntro::DURACION;
    float alfa = 1.0f;
    if (t < 0.25f) alfa = t / 0.25f;
    else if (t > 0.75f) alfa = (1.0f - t) / 0.25f;

    util_entrar2D(ancho, alto);

    // GIF DE FONDO
    if (!s_gifInicio) {
        s_gifInicio = new ETSIDI::SpriteSequence(
            "imagenes\\GIFT_GOT.png",
            5, 9, 80, true, 0, 0, 480, 269
        );
    }
    
    //PARA CENTRAR EL GIFT DEL INICIO
    s_gifInicio->setPos(200.0f, 50.0f); //He cambiado los valores 20 veces y asi se ve mejor, asi q asi se queda
    s_gifInicio->setSize((float)ancho * 1.15f, (float)alto * 1.15f);

    s_gifInicio->loop();
    s_gifInicio->draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TÍTULO PRINCIPAL
    ETSIDI::setFont("fuentes\\ALGER.TTF", 40);
    ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, alfa);
    ETSIDI::printxy("MOROS Y CRISTIANOS", ancho / 2 - 270, alto / 2 + 30);

    // SUBTÍTULO
    ETSIDI::setFont("fuentes\\BRUSHSCI.TTF", 28);
    ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, alfa);
    ETSIDI::printxy("La Reconquista", ancho / 2 - 110, alto / 2 - 20);

    // TEXTO SKIP (PARPADEA EN LA SEGUNDA MITAD)
    if (p.m_fotograma > PantallaIntro::DURACION / 2) {
        float parpadeo = (sinf(p.m_fotograma * 0.1f) + 1.0f) * 0.5f * alfa;
        std::string skip = "Pulsa cualquier tecla para continuar";
        int pk = glutBitmapLength((unsigned char*)GLUT_BITMAP_HELVETICA_12,
            (const unsigned char*)skip.c_str());
        glColor4f(0.7f, 0.7f, 0.7f, parpadeo);
        glRasterPos2f(ancho / 2.0f - pk / 2.0f, alto / 4.0f);
        for (char c : skip) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glDisable(GL_BLEND);
    util_salir2D();
}


// ══════════════════════════════════════════════════════════════════════════
// MENÚ PRINCIPAL — ORQUESTADOR
// ══════════════════════════════════════════════════════════════════════════

void DibujaMenu::menu_dibujar(MenuPrincipal& m, int ancho, int alto) {
    m.m_fotograma++;
    util_entrar2D(ancho, alto);
    menu_fondo(ancho, alto);
    menu_titulo(m, ancho, alto);
    switch (m.m_paso) {
    case 0: menu_paso0(m, ancho, alto); break;
    case 1: menu_paso_nombre(m, ancho, alto); break;
    case 2: menu_paso1(m, ancho, alto); break;
    case 3: menu_paso_nombre(m, ancho, alto); break;
    case 4: menu_paso2(m, ancho, alto); break;
    case 5: menu_paso3(m, ancho, alto); break;
    }
    menu_pie(ancho, alto);
    util_salir2D();
}


// ══════════════════════════════════════════════════════════════════════════
// MENÚ PRINCIPAL — SECCIONES
// ══════════════════════════════════════════════════════════════════════════

void DibujaMenu::menu_fondo(int ancho, int alto) {
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(0, alto);
    glColor3f(0.05f, 0.05f, 0.22f); glVertex2f(ancho, alto);
    glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(ancho, 0);
    glColor3f(0.18f, 0.03f, 0.03f); glVertex2f(0, 0);
    glEnd();

    // LÍNEA DORADA SEPARADORA
    glLineWidth(2.0f);
    glColor3f(0.85f, 0.70f, 0.10f);
    glBegin(GL_LINES);
    glVertex2f(ancho * 0.05f, alto * 0.75f);
    glVertex2f(ancho * 0.95f, alto * 0.75f);
    glEnd();
    glLineWidth(1.0f);
}

void DibujaMenu::menu_titulo(MenuPrincipal& m, int ancho, int alto) {
    float brillo = (sinf(m.m_fotograma * 0.04f) + 1.0f) * 0.12f;
    menu_textoCentrado("MOROS Y CRISTIANOS", ancho / 2.0f, alto - 70,
        0.85f + brillo, 0.70f + brillo * 0.5f, 0.10f,
        GLUT_BITMAP_TIMES_ROMAN_24);
    menu_textoCentrado("- La Reconquista -", ancho / 2.0f, alto - 100,
        0.75f, 0.75f, 0.75f,
        GLUT_BITMAP_HELVETICA_12);
}

void DibujaMenu::menu_paso0(MenuPrincipal& m, int ancho, int alto) {
    menu_textoCentrado("Selecciona el modo de juego:", ancho / 2.0f, alto - 135,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);

    float aw = 280, ah = 44, sep = 14;
    float sx = ancho / 2.0f - aw / 2.0f;
    float sy = alto / 2.0f + 60;

    const char* opciones[] = {
        "  Jugador vs Jugador",
        "  Jugador vs IA",
        "  Cargar Partida",
        "  Ranking",
        "  Salir"
    };
    for (int i = 0; i < 4; i++)
        menu_opcion(opciones[i], sx, sy - i * (ah + sep), aw, ah, m.m_seleccion == i);

    // Si no hay partida guardada, se muestra un aviso
    if (m.m_seleccion == 2 && !GestorPartida::hayPartidaGuardada()) {
        menu_textoCentrado("No hay partida guardada",
            ancho / 2.0f, alto / 2.0f - 100,
            0.9f, 0.3f, 0.3f, GLUT_BITMAP_HELVETICA_12);
    }
}

void DibujaMenu::menu_paso1(MenuPrincipal& m, int ancho, int alto) {
    std::string modotxt = (m.m_cfg.modo == ModoJuego::JVJ) ?
        "Modo: Jugador vs Jugador" : "Modo: Jugador vs IA";
    menu_textoCentrado(modotxt, ancho / 2.0f, alto - 135,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);

    float aw = 240, ah = 55, sep = 30;
    float sy = alto / 2.0f + 10;

    menu_opcion("  CRISTIANO",
        ancho / 2.0f - aw - sep / 2.0f, sy, aw, ah, m.m_seleccion == 0);
    menu_opcion("  MUSULMAN",
        ancho / 2.0f + sep / 2.0f, sy, aw, ah, m.m_seleccion == 1);
}

void DibujaMenu::menu_paso2(MenuPrincipal& m, int ancho, int alto) {
    menu_textoCentrado("Elige la batalla:", ancho / 2.0f, alto - 135,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_12);

    float aw = 340, ah = 48, sep = 12;
    float sx = ancho / 2.0f - aw / 2.0f;
    float sy = alto / 2.0f + 80;

    const char* batallas[] = {
        "  Batalla de Guadalete (711)",
        "  Batalla de Alarcos (1195)",
        "  Las Navas de Tolosa (1212)",
        "  Reconquista de Granada (1492)"
    };
    for (int i = 0; i < 4; i++)
        menu_opcion(batallas[i], sx, sy - i * (ah + sep), aw, ah, m.m_seleccion == i);

    // DESCRIPCIÓN DE LA BATALLA RESALTADA
    const char* descripciones[] = {
        "Los musulmanes cruzan el estrecho y vencen al rey Rodrigo.",
        "Al-Mansur derrota a Alfonso VIII de Castilla.",
        "Los reinos cristianos unidos rompen el poder almohade.",
        "Los Reyes Catolicos completan la Reconquista peninsular."
    };
    menu_textoCentrado(descripciones[m.m_seleccion],
        ancho / 2.0f, alto / 2.0f - 140,
        0.65f, 0.65f, 0.65f, GLUT_BITMAP_HELVETICA_12);
}

void DibujaMenu::menu_paso3(MenuPrincipal& m, int ancho, int alto) {
    std::string modo = (m.m_cfg.modo == ModoJuego::JVJ) ?
        "Jugador vs Jugador" : "Jugador vs IA";
    std::string bando = (m.m_cfg.bando == BandoJugador::CRISTIANO) ?
        "Cristiano" : "Musulman";

    menu_textoCentrado("Modo:  " + modo, ancho / 2.0f, alto / 2.0f + 70,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    menu_textoCentrado("Bando: " + bando, ancho / 2.0f, alto / 2.0f + 40,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    float aw = 180, ah = 44, sep = 30;
    float cy = alto / 2.0f - 20;
    menu_opcion("  JUGAR",
        ancho / 2.0f - aw - sep / 2.0f, cy, aw, ah, m.m_seleccion == 0);
    menu_opcion("  VOLVER",
        ancho / 2.0f + sep / 2.0f, cy, aw, ah, m.m_seleccion == 1);
}

void DibujaMenu::menu_pie(int ancho, int alto) {
    menu_textoCentrado(
        "[Flechas/Raton] Navegar       [ENTER/Click] Seleccionar       [ESC] Volver",
        ancho / 2.0f, 12, 0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);
}


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
    glEnd();
    glDisable(GL_BLEND);
}

void DibujaMenu::menu_borde(float x, float y,
    float ancho, float alto,
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

void DibujaMenu::menu_opcion(const std::string& texto,
    float x, float y,
    float ancho, float alto,
    bool seleccionada) {
    if (seleccionada)
        menu_rectangulo(x, y, ancho, alto, 0.85f, 0.70f, 0.10f, 0.25f);
    else
        menu_rectangulo(x, y, ancho, alto, 0.08f, 0.08f, 0.08f, 0.50f);

    if (seleccionada)
        menu_borde(x, y, ancho, alto, 0.95f, 0.80f, 0.10f, 2.0f);
    else
        menu_borde(x, y, ancho, alto, 0.35f, 0.35f, 0.35f, 1.0f);

    if (seleccionada)
        menu_texto(texto, x + 12, y + alto / 2.0f - 6,
            1.0f, 1.0f, 0.55f, GLUT_BITMAP_HELVETICA_18);
    else
        menu_texto(texto, x + 12, y + alto / 2.0f - 6,
            0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
}

// PANTALLA DESTINO (BATALLA) — ORQUESTADOR Y DIBUJO

void DibujaMenu::destino_dibujar(PantallaDestino& p, int ancho, int alto) {
    // 1. ¡SUPER IMPORTANTE! ACTUALIZAR LA LÓGICA ANTES DE DIBUJAR
    p.actualizar(ancho, alto);

    // 2. DIBUJAMENU SE ENCARGA DE ENTRAR EN EL MODO 2D
    util_entrar2D(ancho, alto);

    // 3. EJECUTA TODAS LAS CAPAS VISUALES LEYENDO LOS DATOS DE 'p'
    destino_fondo(p, ancho, alto);
    destino_efectos(p, ancho, alto);
    destino_particulas(p);
    destino_textos(p, ancho, alto);
    destino_continuar(p, ancho, alto);

    // 4. RESTAURA LA CÁMARA
    util_salir2D();
}

void DibujaMenu::destino_fondo(const PantallaDestino& p, int ancho, int alto) {
    // DEFINIMOS LAS RUTAS SEGÚN LA BATALLA DIRECTAMENTE EN EL MOTOR GRÁFICO
    const char* rutas[] = {
        "imagenes\\GUADALETE.png",
        "imagenes\\ALARCOS.png",
        "imagenes\\NAVAS_TOLOSA.png",
        "imagenes\\GRANADA.png"
    };

    // CARGAMOS LA TEXTURA AQUÍ, SIN ENSUCIAR LA LÓGICA
    auto textura = ETSIDI::getTexture(rutas[(int)p.m_cfg.batalla]);

    // DIBUJAMOS LA IMAGEN DE FONDO (USANDO CAST A FLOAT PARA EVITAR WARNINGS)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura.id);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0.0f, (float)alto);
    glTexCoord2f(1, 0); glVertex2f((float)ancho, (float)alto);
    glTexCoord2f(1, 1); glVertex2f((float)ancho, 0.0f);
    glTexCoord2f(0, 1); glVertex2f(0.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // CAPA OSCURA SEMITRANSPARENTE PARA QUE EL TEXTO SEA LEGIBLE
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f((float)ancho, 0.0f);
    glVertex2f((float)ancho, (float)alto);
    glVertex2f(0.0f, (float)alto);
    glEnd();
    glDisable(GL_BLEND);
}

void DibujaMenu::destino_efectos(const PantallaDestino& p, int ancho, int alto) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (p.m_cfg.batalla) {
    case Batalla::GUADALETE:
        glLineWidth(1.5f);
        for (int j = 0; j < 5; j++) {
            float yBase = (float)alto * 0.10f + j * 16.0f;
            float fase = (float)p.m_fotograma * 0.04f + j * 1.3f;
            glColor4f(0.85f, 0.55f, 0.10f, 0.12f);
            glBegin(GL_LINE_STRIP);
            for (int x = 0; x <= ancho; x += 10) {
                float y = yBase + sinf((float)x * 0.018f + fase) * 6.0f;
                glVertex2f((float)x, y);
            }
            glEnd();
        }
        glLineWidth(1.0f);
        break;

    case Batalla::NAVAS_TOLOSA: {
        float cx = (float)ancho * 0.5f;
        float ty = (float)alto + 10.0f;
        int nRayos = 10;
        for (int r = 0; r < nRayos; r++) {
            float t = (r / (float)(nRayos - 1)) - 0.5f;
            float angA = t * 1.4f;
            float angB = (t + 1.0f / (nRayos - 1)) * 1.4f;
            float pulse = (sinf((float)p.m_fotograma * 0.03f + r * 0.7f) + 1.0f) * 0.5f;
            glColor4f(1.0f, 0.95f, 0.70f, 0.05f + pulse * 0.07f);
            float len = (float)ancho * 1.5f;
            glBegin(GL_TRIANGLES);
            glVertex2f(cx, ty);
            glVertex2f(cx + sinf(angA) * len, ty - cosf(angA) * len);
            glVertex2f(cx + sinf(angB) * len, ty - cosf(angB) * len);
            glEnd();
        }
        break;
    }
    default: break;
    }

    glDisable(GL_BLEND);
}

void DibujaMenu::destino_particulas(const PantallaDestino& p) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);

    // ITERA SOBRE EL ARRAY DE PARTICULAS Y LAS DIBUJA
    for (const auto& part : p.m_particulas) {
        glPointSize(part.tam);
        glColor4f(part.r, part.g, part.b, part.alfa);
        glBegin(GL_POINTS);
        glVertex2f(part.x, part.y);
        glEnd();
    }

    glDisable(GL_POINT_SMOOTH);
    glPointSize(1.0f);
    glDisable(GL_BLEND);
}

void DibujaMenu::destino_textos(const PantallaDestino& p, int ancho, int alto) {
    float alfa = p.alfaEntrada();
    float brillo = (sinf((float)p.m_fotograma * 0.04f) + 1.0f) * 0.10f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TÍTULO DE LA BATALLA
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 24);
    ETSIDI::setTextColor((0.85f + brillo) * alfa, (0.70f + brillo * 0.5f) * alfa, 0.10f * alfa, alfa);
    {
        const char* tituloBat = nombreBatalla(p.m_cfg.batalla);
        int tw = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)tituloBat);
        ETSIDI::printxy(tituloBat, ancho / 2 - (int)(tw * 0.80f), alto - 85);
    }

    // LÍNEA DECORATIVA
    glColor4f(0.85f * alfa, 0.68f * alfa, 0.10f * alfa, 0.65f * alfa);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f((float)ancho / 2.0f - 220.0f, (float)alto - 112.0f);
    glVertex2f((float)ancho / 2.0f + 220.0f, (float)alto - 112.0f);
    glEnd();
    glLineWidth(1.0f);

    // CONTEXTO HISTÓRICO (MÁQUINA DE ESCRIBIR)
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 18);
    int letrasRestantes = p.m_letraVisible;
    for (int i = 0; i < (int)p.m_lineas.size(); i++) {
        if (letrasRestantes <= 0) break;
        int lineLen = (int)p.m_lineas[i].size();
        int mostrar = std::min(letrasRestantes, lineLen);
        letrasRestantes -= lineLen;
        std::string visible = p.m_lineas[i].substr(0, mostrar);
        ETSIDI::setTextColor(0.92f, 0.88f, 0.80f, alfa);
        // Centrar usando la línea completa para que no se desplace durante el efecto máquina de escribir
        int lw = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)p.m_lineas[i].c_str());
        ETSIDI::printxy(visible.c_str(), ancho / 2 - lw / 2, alto - 160 - i * 26);
    }

    glDisable(GL_BLEND);
}

void DibujaMenu::destino_continuar(const PantallaDestino& p, int ancho, int alto) {
    if (!p.textoCompleto()) return;

    // EFECTO DE PARPADEO DEL TEXTO CONTINUAR
    float parpadeo = (sinf((float)p.m_fotograma * 0.08f) + 1.0f) * 0.5f;
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 12);
    ETSIDI::setTextColor(0.80f, 0.78f, 0.65f, 0.35f + parpadeo * 0.65f);
    {
        const char* contTxt = "Pulsa cualquier tecla para comenzar la batalla";
        int cw = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)contTxt);
        ETSIDI::printxy(contTxt, ancho / 2 - cw / 2, 30);
    }
}

// RANKING
void DibujaMenu::ranking_dibujar(int ancho, int alto, const std::string& ganador, const std::string& batalla, int turnos, int piezasLocal, int piezasRival, const std::vector<EntradaRanking>& ranking, bool ganaJ1)
{
    util_entrar2D(ancho, alto);
    menu_fondo(ancho, alto);

    // Título
    menu_textoCentrado("RESULTADO FINAL", ancho / 2.0f, alto - 80,
        0.85f, 0.70f, 0.10f, GLUT_BITMAP_TIMES_ROMAN_24);

    // Batalla y turnos
    menu_textoCentrado("Batalla: " + batalla, ancho / 2.0f, alto - 130,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);
    menu_textoCentrado("Turnos jugados: " + std::to_string(turnos), ancho / 2.0f, alto - 155,
        0.90f, 0.85f, 0.60f, GLUT_BITMAP_HELVETICA_18);

    // Tabla
    float margen = ancho * 0.15f;
    float tablaAncho = ancho * 0.70f;
    float tablaY = alto - 200;
    float colAncho = tablaAncho / 3.0f;
    float filaAlto = 40.0f;

    // Cabecera de la tabla
    menu_rectangulo(margen, tablaY - filaAlto, tablaAncho, filaAlto,
        0.85f, 0.70f, 0.10f, 0.4f);
    menu_texto("Estadistica", margen + 10, tablaY - filaAlto + 12,
        1.0f, 1.0f, 1.0f, GLUT_BITMAP_HELVETICA_18);
    menu_texto("Cristiano", margen + colAncho + 10, tablaY - filaAlto + 12,
        1.0f, 1.0f, 1.0f, GLUT_BITMAP_HELVETICA_18);
    menu_texto("Andalusi", margen + colAncho * 2 + 10, tablaY - filaAlto + 12,
        1.0f, 1.0f, 1.0f, GLUT_BITMAP_HELVETICA_18);

    // Fila 1: Ganador
    menu_rectangulo(margen, tablaY - filaAlto * 2, tablaAncho, filaAlto,
        0.08f, 0.08f, 0.08f, 0.5f);
    menu_texto("Resultado", margen + 10, tablaY - filaAlto * 2 + 12,
        0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(ganaJ1 ? "GANADOR" : "perdedor",
        margen + colAncho + 10, tablaY - filaAlto * 2 + 12,
        ganador == "Cristiano" ? 1.0f : 0.5f,
        ganador == "Cristiano" ? 1.0f : 0.5f,
        0.0f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(!ganaJ1 ? "GANADOR" : "perdedor",
        margen + colAncho * 2 + 10, tablaY - filaAlto * 2 + 12,
        ganador == "Andalusi" ? 1.0f : 0.5f,
        ganador == "Andalusi" ? 1.0f : 0.5f,
        0.0f, GLUT_BITMAP_HELVETICA_18);

    // Fila 2: Piezas eliminadas
    menu_rectangulo(margen, tablaY - filaAlto * 3, tablaAncho, filaAlto,
        0.12f, 0.12f, 0.12f, 0.5f);
    menu_texto("Piezas eliminadas", margen + 10, tablaY - filaAlto * 3 + 12,
        0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(std::to_string(piezasLocal),
        margen + colAncho + 10, tablaY - filaAlto * 3 + 12,
        0.89f, 0.29f, 0.29f, GLUT_BITMAP_HELVETICA_18);
    menu_texto(std::to_string(piezasRival),
        margen + colAncho * 2 + 10, tablaY - filaAlto * 3 + 12,
        0.39f, 0.60f, 0.13f, GLUT_BITMAP_HELVETICA_18);

    // Borde de la tabla
    menu_borde(margen, tablaY - filaAlto * 3, tablaAncho, filaAlto * 3,
        0.85f, 0.70f, 0.10f, 2.0f);

    // Ganador destacado
    menu_textoCentrado("¡VICTORIA " + ganador + "!",
        ancho / 2.0f, tablaY - filaAlto * 4 - 20,
        1.0f, 1.0f, 0.4f, GLUT_BITMAP_TIMES_ROMAN_24);

    menu_textoCentrado("Pulsa ESC para volver al menu",
        ancho / 2.0f, 30,
        0.50f, 0.50f, 0.50f, GLUT_BITMAP_HELVETICA_12);

    // TOP 10
    //auto ranking = GestorRanking::cargar();
    menu_textoCentrado("TOP 10", ancho / 2.0f, tablaY - filaAlto * 5 - 20,
        0.85f, 0.70f, 0.10f, GLUT_BITMAP_HELVETICA_18);

    if (ranking.empty()) {
        menu_textoCentrado("No hay partidas registradas todavia",
            ancho / 2.0f, tablaY - filaAlto * 6 - 20,
            0.80f, 0.80f, 0.80f, GLUT_BITMAP_HELVETICA_12);
    }
    else {
        for (int i = 0; i < (int)ranking.size(); i++) {
            float fy = tablaY - filaAlto * 6 - 20 - i * 22;
            float r = 0.80f, g = 0.80f, b = 0.80f;
            if (i == 0) { r = 1.0f;  g = 0.85f; b = 0.10f; } // ORO
            if (i == 1) { r = 0.80f; g = 0.80f; b = 0.80f; } // PLATA
            if (i == 2) { r = 0.80f; g = 0.50f; b = 0.20f; } // BRONCE

            std::string linea = std::to_string(i + 1) + ". " +
                ranking[i].ganador + " - " +
                ranking[i].batalla + " - " +
                std::to_string(ranking[i].puntuacion) + " pts";
            menu_textoCentrado(linea, ancho / 2.0f, fy, r, g, b, GLUT_BITMAP_HELVETICA_12);
        }
    }
    util_salir2D();
}


// ══════════════════════════════════════════════════════════════════════════
// VICTORIA
// ══════════════════════════════════════════════════════════════════════════

void DibujaMenu::victoria_dibujar(int ancho, int alto, const std::string& ganador,
    const std::string& batalla, bool ganaJ1, float tiempoRestante)
{
    static int s_fotoV = 0;
    s_fotoV++;
    // RESET: si tiempoRestante es alto (partida nueva) reiniciamos
    static float s_prevTime = -1.0f;
    if (tiempoRestante > s_prevTime + 1.0f) s_fotoV = 0;
    s_prevTime = tiempoRestante; 

    float alfa = (s_fotoV < 60) ? s_fotoV / 60.0f : 1.0f; // FADE-IN EN 1 SEGUNDO

    util_entrar2D(ancho, alto);

    // FONDO DE BATALLA
        // FONDO DE BATALLA (mismas rutas que la pantalla destino)
    const char* ruta = "imagenes\\GUADALETE.png";
    if      (batalla.find("Guadalete") != std::string::npos) ruta = "imagenes\\GUADALETE.png";
    else if (batalla.find("Alarcos")   != std::string::npos) ruta = "imagenes\\ALARCOS.png";
    else if (batalla.find("Navas")     != std::string::npos) ruta = "imagenes\\NAVAS_TOLOSA.png";
    else if (batalla.find("Granada")   != std::string::npos) ruta = "imagenes\\GRANADA.png";

    auto tex = ETSIDI::getTexture(ruta);
    if (tex.id != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0.0f, (float)alto);
        glTexCoord2f(1, 0); glVertex2f((float)ancho, (float)alto);
        glTexCoord2f(1, 1); glVertex2f((float)ancho, 0.0f);
        glTexCoord2f(0, 1); glVertex2f(0.0f, 0.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    // OVERLAY OSCURO
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(ancho, 0);
    glVertex2f(ancho, alto); glVertex2f(0, alto);
    glEnd();
    glDisable(GL_BLEND);

    // MENSAJE DE VICTORIA — CON FADE Y CENTRADO DINÁMICO
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const char* tituloVic = ganaJ1 ? "VICTORIA CRISTIANA" : "VICTORIA AL-ANDALUS";
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 52);
    if (ganaJ1)
        ETSIDI::setTextColor(0.95f, 0.75f, 0.10f, alfa); // ORO CRISTIANO
    else
        ETSIDI::setTextColor(0.55f, 0.10f, 0.85f, alfa); // PÚRPURA ANDALUSÍ

    // ARIALNBI bold italic: cada caracter ocupa aprox 0.62 * tamano de fuente px
    int twV = (int)(strlen(tituloVic) * 52 * 0.62f);
    ETSIDI::printxy(tituloVic, ancho / 2 - twV / 2, alto / 2 + 80);

    // NOMBRE DEL GANADOR — CENTRADO Y CON FADE
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 30);
    ETSIDI::setTextColor(1.0f, 0.95f, 0.80f, alfa);
    {
        std::string txt = "Ganador: " + ganador;
        int tw2 = (int)(txt.size() * 30 * 0.62f);
        ETSIDI::printxy(txt.c_str(), ancho / 2 - tw2 / 2, alto / 2 + 20);
    }

    // LÍNEA SEPARADORA DORADA
    if (alfa > 0.3f) {
        glColor4f(0.85f * alfa, 0.68f * alfa, 0.10f * alfa, 0.8f * alfa);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f((float)ancho / 2.0f - 200.0f, (float)alto / 2.0f - 10.0f);
        glVertex2f((float)ancho / 2.0f + 200.0f, (float)alto / 2.0f - 10.0f);
        glEnd();
        glLineWidth(1.0f);
    }

    // CONTADOR PARPADEANTE
    float parpadeo = (sinf((float)s_fotoV * 0.08f) + 1.0f) * 0.5f * alfa;
    ETSIDI::setFont("fuentes\\ARIALNBI.ttf", 18);
    ETSIDI::setTextColor(0.70f, 0.70f, 0.70f, parpadeo);
    {
        char buf[64];
        sprintf_s(buf, "Ranking en %d segundos...", (int)tiempoRestante);
        int tw3 = (int)(strlen(buf) * 18 * 0.62f);
        ETSIDI::printxy(buf, ancho / 2 - tw3 / 2, alto / 2 - 50);
    }

    glDisable(GL_BLEND);
    // PARTÍCULAS DE CELEBRACIÓN
    static std::vector<Particula> s_partV;
    static int s_partFotoV = 0;
    static float s_prevPartTime = -1.0f;
    if (tiempoRestante > s_prevPartTime + 1.0f) { s_partV.clear(); s_partFotoV = 0; }
    s_prevPartTime = tiempoRestante;
    s_partFotoV++;

    // EMITIR NUEVAS PARTÍCULAS
    if ((int)s_partV.size() < 200) {
        for (int i = 0; i < 3; i++) {
            Particula p;
            float v = (float)rand() / RAND_MAX;
            if (ganaJ1) { // CRISTIANO: dorado
                p.r = 0.90f + v * 0.10f; p.g = 0.70f + v * 0.20f; p.b = 0.05f;
            }
            else {      // ANDALUSÍ: púrpura
                p.r = 0.40f + v * 0.30f; p.g = 0.05f; p.b = 0.70f + v * 0.30f;
            }
            p.vidaMax = 80.0f + v * 100.0f;
            p.vida = p.vidaMax;
            p.alfa = 0.9f;
            p.x = (float)(rand() % ancho);
            p.y = -5.0f;
            p.vx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            p.vy = 1.5f + (float)rand() / RAND_MAX * 2.5f;
            p.tam = 2.0f + (float)rand() / RAND_MAX * 3.0f;
            s_partV.push_back(p);
        }
    }

    // ACTUALIZAR Y DIBUJAR
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    for (auto& p : s_partV) {
        p.x += p.vx; p.y += p.vy; p.vida -= 1.0f;
        p.alfa = (p.vida / p.vidaMax) * 0.85f;
        glPointSize(p.tam);
        glColor4f(p.r, p.g, p.b, p.alfa * alfa);
        glBegin(GL_POINTS); glVertex2f(p.x, p.y); glEnd();
    }
    s_partV.erase(
        std::remove_if(s_partV.begin(), s_partV.end(),
            [&](const Particula& p) { return p.vida <= 0 || p.y > alto + 10; }),
        s_partV.end());
    glDisable(GL_POINT_SMOOTH);
    glPointSize(1.0f);
    glDisable(GL_BLEND);

    util_salir2D();
}

void DibujaMenu::ayuda_dibujar(int seleccion, int seccion, int ancho, int alto)
{
    util_entrar2D(ancho, alto);

    // FONDO OSCURO
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(ancho, 0);
    glVertex2f(ancho, alto); glVertex2f(0, alto);
    glEnd();
    glDisable(GL_BLEND);

    if (seccion == -1) {
        // MENU AYUDA: DOS BOTONES
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 32);
        ETSIDI::printxy("AYUDA", ancho / 2 - 50, alto - 100);

        const char* opciones[2] = { "Controles", "Normas" };
        float btnW = 260, btnH = 48;
        float btnX = ancho / 2.0f - btnW / 2.0f;
        float btnY[2] = { alto / 2.0f + 20, alto / 2.0f - 50 };

        for (int i = 0; i < 2; i++) {
            bool sel = (seleccion == i);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(sel ? 0.55f : 0.30f, sel ? 0.15f : 0.10f, sel ? 0.80f : 0.50f, 0.95f);
            glBegin(GL_QUADS);
            glVertex2f(btnX, btnY[i]); glVertex2f(btnX + btnW, btnY[i]);
            glVertex2f(btnX + btnW, btnY[i] + btnH); glVertex2f(btnX, btnY[i] + btnH);
            glEnd();
            glColor4f(0.85f, 0.70f, 0.25f, 1.0f);
            glLineWidth(sel ? 2.5f : 1.5f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(btnX, btnY[i]); glVertex2f(btnX + btnW, btnY[i]);
            glVertex2f(btnX + btnW, btnY[i] + btnH); glVertex2f(btnX, btnY[i] + btnH);
            glEnd();
            glLineWidth(1.0f);
            glDisable(GL_BLEND);
            if (sel) ETSIDI::setTextColor(1.0f, 1.0f, 0.0f, 1.0f);
            else     ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
            ETSIDI::setFont("fuentes/ARIALNBI.ttf", 18);
            ETSIDI::printxy(opciones[i], (int)(btnX + 80), (int)(btnY[i] + 14));
        }
        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver a pausa  |  ENTER: seleccionar", ancho / 2 - 160, 30);
    }
    else if (seccion == 0) {
        // CONTROLES
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 24);
        ETSIDI::printxy("CONTROLES", ancho / 2 - 70, alto - 60);

        ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 15);
        int x = ancho / 2 - 300, y = alto - 90, sep = 22;
        ETSIDI::printxy("TABLERO:", x, y); y -= sep;
        ETSIDI::printxy("  W/A/S/D         Mover cursor Jugador 1", x, y); y -= sep;
        ETSIDI::printxy("  ESPACIO         Seleccionar/Mover pieza J1", x, y); y -= sep;
        ETSIDI::printxy("  FLECHAS         Mover cursor Jugador 2", x, y); y -= sep;
        ETSIDI::printxy("  PUNTO (.)       Seleccionar/Mover pieza J2", x, y); y -= sep;
        ETSIDI::printxy("  H               Activar hechizo J1 (selecciona Rey primero)", x, y); y -= sep;
        ETSIDI::printxy("  J               Activar hechizo J2 (selecciona Emir primero)", x, y); y -= sep;
        ETSIDI::printxy("  1/2/3/4         Elegir hechizo activo", x, y); y -= sep;
        ETSIDI::printxy("  I               Habilidad del Infiltrado", x, y); y -= sep;
        ETSIDI::printxy("  ESC             Menu de pausa", x, y); y -= sep + 10;
        ETSIDI::printxy("ARENA:", x, y); y -= sep;
        ETSIDI::printxy("  W/A/S/D         Mover J1", x, y); y -= sep;
        ETSIDI::printxy("  F               Atacar J1", x, y); y -= sep;
        ETSIDI::printxy("  FLECHAS         Mover J2 (JvJ)", x, y); y -= sep;
        ETSIDI::printxy("  L               Atacar J2 (JvJ)", x, y); y -= sep;
        ETSIDI::printxy("  ENTER           Volver al tablero (tras combate)", x, y);

        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver", ancho / 2 - 40, 30);
    }
    else if (seccion == 1) {
        // NORMAS
        ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 24);
        ETSIDI::printxy("NORMAS", ancho / 2 - 50, alto - 60);

        ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 15);
        int x = ancho / 2 - 300, y = alto - 110, sep = 26;
        ETSIDI::printxy("OBJETIVO: Ganar controlando el tablero mediante una de estas condiciones:", x, y); y -= sep;
        ETSIDI::printxy("  1. Controlar los 5 Bastiones Historicos del tablero.", x, y); y -= sep;
        ETSIDI::printxy("  2. Eliminar todas las piezas enemigas.", x, y); y -= sep;
        ETSIDI::printxy("  3. Dejar al rival con una sola pieza bloqueada.", x, y); y -= sep + 10;
        ETSIDI::printxy("TURNO: Cada jugador mueve una pieza por turno. Si el tiempo se acaba, pasa el turno.", x, y); y -= sep + 10;
        ETSIDI::printxy("COMBATE: Si dos piezas coinciden en una casilla se abre la arena.", x, y); y -= sep;
        ETSIDI::printxy("  En la arena luchan en tiempo real. El ganador ocupa la casilla.", x, y); y -= sep;
        ETSIDI::printxy("  La pieza ganadora mantiene la vida con la que termino el combate.", x, y); y -= sep + 10;
        ETSIDI::printxy("HECHIZOS: El Rey/Emir puede lanzar 4 conjuros (uno de cada, uso unico):", x, y); y -= sep;
        ETSIDI::printxy("  1. Avituallamiento: cura una pieza aliada a vida maxima.", x, y); y -= sep;
        ETSIDI::printxy("  2. Rutas Secretas: teleporta una pieza aliada a cualquier casilla libre.", x, y); y -= sep;
        ETSIDI::printxy("  3. Relevo de Guardia: intercambia dos piezas aliadas de posicion.", x, y); y -= sep;
        ETSIDI::printxy("  4. Asedio: bloquea una pieza enemiga en su casilla.", x, y); y -= sep + 10;
        ETSIDI::printxy("HABILIDAD ESPECIAL: infiltrado, asesino de elite:", x, y); y -= sep;
        ETSIDI::printxy("  Selecciona al Infiltrado y pulsa I para activar la habilidad.", x, y); y -= sep;
        ETSIDI::printxy("  Luego haz clic en un enemigo para copiar sus estadisticas.", x, y); y -= sep;
        ETSIDI::printxy("  Pulsa ENTER para confirmar el robo. Gasta el turno.", x, y); y -= sep + 10;
        ETSIDI::printxy("CASILLAS: Las casillas dinamicas cambian de bando cada 4 turnos.", x, y); y -= sep;
        ETSIDI::printxy("  Luchar en casillas propias da ventaja en combate.", x, y);

        ETSIDI::setTextColor(0.6f, 0.6f, 0.6f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("ESC: volver", ancho / 2 - 40, 30);
    }

    util_salir2D();
}