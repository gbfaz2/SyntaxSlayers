#include "menu.h"
#include "GestorPartida.h"

PantallaIntro::PantallaIntro() { reiniciar(); }

void PantallaIntro::reiniciar() {
    m_fotograma = 0;
    m_terminado = false;
}

void PantallaIntro::saltar() { m_terminado = true; }

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
    m_focoNombre = 0;
}

void MenuPrincipal::confirmar() {
    if (m_paso == 0) {
        switch (m_seleccion) {
        case 0:
            if (GestorPartida::hayPartidaGuardada()) {
                m_siguiente = EstadoJuego::CARGANDO;
                m_terminado = true;
            }
            break;
        case 1: m_paso = 1; m_seleccion = 0; break;
        case 2: m_siguiente = EstadoJuego::AYUDA; m_terminado = true; break;
        case 3: m_siguiente = EstadoJuego::RANKING; m_terminado = true; break;
        case 4: m_siguiente = EstadoJuego::FINAL; m_terminado = true; break;
        }
    }
    else if (m_paso == 1) {
        switch (m_seleccion) {
        case 0: m_cfg.modo = ModoJuego::JVJ;  break;
        case 1: m_cfg.modo = ModoJuego::JVIA; break;
        }
        if (m_cfg.modo == ModoJuego::JVIA) {
            m_cfg.bando = BandoJugador::CRISTIANO;
            m_cfg.bando_j2 = BandoJugador::MUSULMAN;
        }
        m_paso = 2; m_seleccion = 0;
        m_nombreJ1Actual = "";
        m_nombreJ2Actual = "";
        m_focoNombre = 0;
    }
    else if (m_paso == 2) {
        if (!m_nombreJ1Actual.empty()) m_cfg.nombre_j1 = m_nombreJ1Actual;
        if (!m_nombreJ2Actual.empty() && m_cfg.modo == ModoJuego::JVJ)
            m_cfg.nombre_j2 = m_nombreJ2Actual;
        if (m_cfg.modo == ModoJuego::JVJ) {
            m_cfg.bando_j2 = (m_cfg.bando == BandoJugador::CRISTIANO)
                ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
        }
        m_paso = 3; m_seleccion = 0;
    }
    else if (m_paso == 3) {
        m_cfg.batalla = (Batalla)m_seleccion;
        m_cfg.turno1 = iniciativa(m_cfg.batalla);
        m_paso = 4; m_seleccion = 0;
    }
    else if (m_paso == 4) {
        if (m_seleccion == 0) {
            m_siguiente = EstadoJuego::DESTINO;
            m_terminado = true;
        }
        else {
            m_paso = 3; m_seleccion = 0;
        }
    }
}

void MenuPrincipal::teclaTextoJ1(unsigned char key) {
    if (key == 13) {
        if (m_cfg.modo == ModoJuego::JVJ) m_focoNombre = 1;
        else confirmar();
    }
    else if ((key == 8 || key == 127) && !m_nombreJ1Actual.empty()) {
        m_nombreJ1Actual.pop_back();
    }
    else if (key >= 32 && key < 127 && m_nombreJ1Actual.size() < 20) {
        m_nombreJ1Actual += (char)key;
    }
}

void MenuPrincipal::teclaTextoJ2(unsigned char key) {
    if (key == 13) confirmar();
    else if ((key == 8 || key == 127) && !m_nombreJ2Actual.empty()) {
        m_nombreJ2Actual.pop_back();
    }
    else if (key >= 32 && key < 127 && m_nombreJ2Actual.size() < 20) {
        m_nombreJ2Actual += (char)key;
    }
}