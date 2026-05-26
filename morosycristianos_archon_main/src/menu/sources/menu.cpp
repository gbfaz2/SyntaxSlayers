
// Autor: Ines Alcérreca Sánchez
// Implementación de las pantallas de introducción y menú principal

#include "menu.h"
#include "GestorPartida.h"

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

//  PANTALLA MENU PRINCIPAL

MenuPrincipal::MenuPrincipal() { reiniciar(); }

void MenuPrincipal::reiniciar() {
    m_paso = 0; 
    m_seleccion = 0; // opcion resaltada
    m_fotograma = 0; // para animaciones
    m_terminado = false; // cuando el jugador confirma, se marca como terminado para que el main cambie de estado
    m_siguiente = EstadoJuego::MENU; // el estado al que se pasará cuando termine (puede ser MENU, TABLERO, ARENA, RANKING...)
    m_cfg = ConfigPartida(); // configuración de partida (modo, bando, batalla, turno1)
	m_nombreActual = ""; // para escribir los nombres de los jugadores
}

// Confirmar seleccion y avanzar al siguiente paso o estado
void MenuPrincipal::confirmar() {
    if (m_paso == 0) {
        switch (m_seleccion) {
        case 0: m_cfg.modo = ModoJuego::JVJ;  m_paso = 1; m_nombreActual = ""; break;
        case 1: m_cfg.modo = ModoJuego::JVIA; m_paso = 1; m_nombreActual = ""; break;
        case 2:
            if (GestorPartida::hayPartidaGuardada()) {
                m_siguiente = EstadoJuego::CARGANDO;
                m_terminado = true;
            }
            break;
        case 3: m_siguiente = EstadoJuego::RANKING; m_terminado = true; break;
        case 4: m_siguiente = EstadoJuego::FINAL;   m_terminado = true; break;
        }
    }
    else if (m_paso == 1) {
        if (!m_nombreActual.empty()) m_cfg.nombre_j1 = m_nombreActual;
        m_nombreActual = "";
        m_paso = 2; m_seleccion = 0;
    }
    else if (m_paso == 2) {
        m_cfg.bando = (m_seleccion == 0) ? BandoJugador::CRISTIANO : BandoJugador::MUSULMAN;
        if (m_cfg.modo == ModoJuego::JVJ) {
            m_paso = 3; m_nombreActual = "";
        }
        else {
            m_paso = 4; m_seleccion = 0;
        }
    }
    else if (m_paso == 3) {
        if (!m_nombreActual.empty()) m_cfg.nombre_j2 = m_nombreActual;
        m_cfg.bando_j2 = (m_cfg.bando == BandoJugador::CRISTIANO)
            ? BandoJugador::MUSULMAN : BandoJugador::CRISTIANO;
        m_nombreActual = "";
        m_paso = 4; m_seleccion = 0;
    }
    else if (m_paso == 4) {
        m_cfg.batalla = (Batalla)m_seleccion;
        m_cfg.turno1 = iniciativa(m_cfg.batalla);
        m_paso = 5; m_seleccion = 0;
    }
    else if (m_paso == 5) {
        if (m_seleccion == 0) {
            m_siguiente = EstadoJuego::DESTINO;
            m_terminado = true;
        }
        else {
            m_paso = 4; m_seleccion = 0;
        }
    }
}

void MenuPrincipal::teclaTexto(unsigned char key) {
    if (key == 13) {  // ENTER confirma el nombre
        confirmar();
    }
    else if ((key == 8 || key == 127) && !m_nombreActual.empty()) { // BACKSPACE
        m_nombreActual.pop_back();
    }
    else if (key >= 32 && key < 127 && m_nombreActual.size() < 20) {
        m_nombreActual += (char)key;
    }
}