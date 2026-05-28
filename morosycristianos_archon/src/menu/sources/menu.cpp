// Autor: Ines Alcérreca Sánchez
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
    m_nombreJ1Actual = "";
    m_nombreJ2Actual = "";
    m_focoNombre = 0; // FOCO INICIAL EN J1
}

// ============================================================
// CONFIRMAR — avanza al siguiente paso según el paso actual
// ============================================================
void MenuPrincipal::confirmar() {

    // ── PASO 0: MENÚ PRINCIPAL ──────────────────────────────
    if (m_paso == 0) {
        switch (m_seleccion) {
        case 0: // CARGAR PARTIDA
            if (GestorPartida::hayPartidaGuardada()) {
                m_siguiente = EstadoJuego::CARGANDO;
                m_terminado = true;
            }
            break;
        case 1: // NUEVA PARTIDA → elige modo
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
    }

    // ── PASO 1: ELEGIR MODO (JVJ / JVIA) ────────────────────
    else if (m_paso == 1) {
        switch (m_seleccion) {
        case 0: m_cfg.modo = ModoJuego::JVJ;  break;
        case 1: m_cfg.modo = ModoJuego::JVIA; break;
        }

        // EN JVIA: FIJAMOS BANDOS AUTOMÁTICAMENTE (JUGADOR = CRISTIANO, IA = ANDALUSÍ)
        if (m_cfg.modo == ModoJuego::JVIA) {
            m_cfg.bando = BandoJugador::CRISTIANO;
            m_cfg.bando_j2 = BandoJugador::MUSULMAN;
        }

        // PASAMOS A LA PANTALLA DE CONFIGURACIÓN
        m_paso = 2; m_seleccion = 0;
        m_nombreJ1Actual = "";
        m_nombreJ2Actual = "";
        m_focoNombre = 0;
    }

    // ── PASO 2: CONFIGURACIÓN (nombres + bandos/dificultad) ─
    else if (m_paso == 2) {
        // GUARDAMOS LOS NOMBRES SI NO ESTÁN VACÍOS
        if (!m_nombreJ1Actual.empty()) m_cfg.nombre_j1 = m_nombreJ1Actual;
        if (!m_nombreJ2Actual.empty() && m_cfg.modo == ModoJuego::JVJ)
            m_cfg.nombre_j2 = m_nombreJ2Actual;

        // EN JVJ: ASIGNAMOS BANDO J2 OPUESTO AL J1
        if (m_cfg.modo == ModoJuego::JVJ) {
            m_cfg.bando_j2 = (m_cfg.bando == BandoJugador::CRISTIANO)
                ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
        }

        // PASAMOS A SELECCIÓN DE BATALLA
        m_paso = 3; m_seleccion = 0;
    }

    // ── PASO 3: SELECCIÓN DE BATALLA ────────────────────────
    else if (m_paso == 3) {
        m_cfg.batalla = (Batalla)m_seleccion;
        m_cfg.turno1 = iniciativa(m_cfg.batalla);
<<<<<<< Updated upstream
        m_paso = 5; m_seleccion = 0;
=======
        m_paso = 4; m_seleccion = 0;
>>>>>>> Stashed changes
    }

    // ── PASO 4: CONFIRMAR (JUGAR / VOLVER) ──────────────────
    else if (m_paso == 4) {
        if (m_seleccion == 0) {
            // JUGAR — LANZAMOS LA PARTIDA
            m_siguiente = EstadoJuego::DESTINO;
            m_terminado = true;
        }
        else {
            // VOLVER — RETROCEDEMOS A SELECCIÓN DE BATALLA
            m_paso = 3; m_seleccion = 0;
        }
    }
}

// ============================================================
// TECLAS DE TEXTO — captura caracteres para los nombres
// ============================================================

void MenuPrincipal::teclaTextoJ1(unsigned char key) {
    if (key == 13) { // ENTER: MUEVE EL FOCO A J2 O CONFIRMA
        if (m_cfg.modo == ModoJuego::JVJ)
            m_focoNombre = 1; // PASA A ESCRIBIR J2
        else
            confirmar(); // EN JVIA SOLO HAY J1, CONFIRMA DIRECTO
    }
    else if ((key == 8 || key == 127) && !m_nombreJ1Actual.empty()) {
        m_nombreJ1Actual.pop_back(); // BACKSPACE
    }
    else if (key >= 32 && key < 127 && m_nombreJ1Actual.size() < 20) {
        m_nombreJ1Actual += (char)key;
    }
}

void MenuPrincipal::teclaTextoJ2(unsigned char key) {
    if (key == 13) { // ENTER: CONFIRMA LA CONFIGURACIÓN
        confirmar();
    }
    else if ((key == 8 || key == 127) && !m_nombreJ2Actual.empty()) {
        m_nombreJ2Actual.pop_back(); // BACKSPACE
    }
    else if (key >= 32 && key < 127 && m_nombreJ2Actual.size() < 20) {
        m_nombreJ2Actual += (char)key;
    }
}