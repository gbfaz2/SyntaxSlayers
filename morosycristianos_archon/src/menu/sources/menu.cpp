
// Implementación de las pantallas de introducción y menú principal

#include "menu.h"
#include "GestorPartida.h"

// ============================================================
// PANTALLA INTRO
// ============================================================

PantallaIntro::PantallaIntro() { reiniciar(); }

void PantallaIntro::reiniciar() {
    m_fotograma = 0;
    m_terminado = false;
}

void PantallaIntro::saltar() {
    m_terminado = true;
}

// ============================================================
// MENÚ PRINCIPAL
// ============================================================

MenuPrincipal::MenuPrincipal() { reiniciar(); }

void MenuPrincipal::reiniciar() {
    m_paso = 0;
    m_seleccion = 0;
    m_fotograma = 0;
    m_terminado = false;
    m_siguiente = EstadoJuego::MENU;
    m_cfg = ConfigPartida();
    m_nombreActual = "";
    m_focoNombre = 0;         // foco en J1 por defecto
    m_nombreJ1Actual = "";
    m_nombreJ2Actual = "";
}

// ============================================================
// CONFIRMAR — sincronizado con dibujamenu.cpp
//
// Paso 0: menú principal   (0=Cargar, 1=Nueva, 2=Ayuda, 3=Ranking, 4=Salir)
// Paso 1: modo de juego    (0=JvJ,   1=JvIA)
// Paso 2: configuración    (nombre J1, bando, nombre J2 si JvJ, dificultad si JvIA)
// Paso 3: batalla          (0-3)
// Paso 4: confirmar        (0=JUGAR, 1=VOLVER)
// ============================================================

void MenuPrincipal::confirmar() {

    // ── PASO 0: menú principal ─────────────────────────────
    if (m_paso == 0) {
        switch (m_seleccion) {
        case 0: // CARGAR PARTIDA
            if (GestorPartida::hayPartidaGuardada()) {
                m_siguiente = EstadoJuego::CARGANDO;
                m_terminado = true;
            }
            break;
        case 1: // NUEVA PARTIDA → elegir modo
            m_paso = 1; m_seleccion = 0;
            break;
        case 2: // AYUDA
            m_siguiente = EstadoJuego::AYUDA;
            m_terminado = true;
            break;
        case 3: // RANKING
            m_siguiente = EstadoJuego::RANKING;
            m_terminado = true;
            break;
        case 4: // SALIR
            m_siguiente = EstadoJuego::FINAL;
            m_terminado = true;
            break;
        }
        return;
    }

    // ── PASO 1: elegir modo ────────────────────────────────
    if (m_paso == 1) {
        m_cfg.modo = (m_seleccion == 0) ? ModoJuego::JVJ : ModoJuego::JVIA;
        // bando J1 por defecto = CRISTIANO
        m_cfg.bando = BandoJugador::CRISTIANO;
        m_cfg.bando_j2 = BandoJugador::MUSULMAN;
        m_cfg.dificultad = NivelDificultad::MEDIO;
        m_focoNombre = 0;
        m_nombreJ1Actual = "";
        m_nombreJ2Actual = "";
        m_paso = 2; m_seleccion = 0;
        return;
    }

    // ── PASO 2: configuración ──────────────────────────────
    // En el dibujo hay un botón "Continuar ->" siempre visible.
    // ENTER aquí confirma la configuración y avanza a la batalla.
    if (m_paso == 2) {
        // Guarda nombres capturados
        if (!m_nombreJ1Actual.empty()) m_cfg.nombre_j1 = m_nombreJ1Actual;
        if (m_cfg.modo == ModoJuego::JVJ && !m_nombreJ2Actual.empty())
            m_cfg.nombre_j2 = m_nombreJ2Actual;
        else if (m_cfg.modo == ModoJuego::JVIA)
            m_cfg.nombre_j2 = "IA";

        m_paso = 3; m_seleccion = 0;
        return;
    }

    // ── PASO 3: elegir batalla ─────────────────────────────
    if (m_paso == 3) {
        m_cfg.batalla = (Batalla)m_seleccion;
        m_cfg.turno1 = iniciativa(m_cfg.batalla);
        m_paso = 4; m_seleccion = 0;
        return;
    }

    // ── PASO 4: confirmar ──────────────────────────────────
    if (m_paso == 4) {
        if (m_seleccion == 0) {        // JUGAR
            m_siguiente = EstadoJuego::DESTINO;
            m_terminado = true;
        }
        else {                         // VOLVER → batalla
            m_paso = 3; m_seleccion = 0;
        }
        return;
    }
}

// ============================================================
// CAPTURA DE TEXTO (pasos con campos de nombre)
// ============================================================

void MenuPrincipal::teclaTexto(unsigned char key) {
    if (key == 13) { // ENTER confirma y avanza
        confirmar();
        return;
    }

    // TAB cambia el foco entre J1 y J2 (solo en JvJ, paso 2)
    if (key == 9 && m_paso == 2 && m_cfg.modo == ModoJuego::JVJ) {
        m_focoNombre = (m_focoNombre == 0) ? 1 : 0;
        return;
    }

    // BACKSPACE
    if (key == 8 || key == 127) {
        if (m_focoNombre == 0 && !m_nombreJ1Actual.empty())
            m_nombreJ1Actual.pop_back();
        else if (m_focoNombre == 1 && !m_nombreJ2Actual.empty())
            m_nombreJ2Actual.pop_back();
        return;
    }

    // Caracteres imprimibles
    if (key >= 32 && key < 127) {
        if (m_focoNombre == 0 && m_nombreJ1Actual.size() < 20)
            m_nombreJ1Actual += (char)key;
        else if (m_focoNombre == 1 && m_nombreJ2Actual.size() < 20)
            m_nombreJ2Actual += (char)key;
    }
}