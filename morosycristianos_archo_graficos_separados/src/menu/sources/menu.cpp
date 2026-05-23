
// Autor: Ines Alcérreca Sánchez
// Implementación de las pantallas de introducción y menú principal

#include "menu.h"

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
    m_paso = 0; // 0=modo, 1=bando, 2=confirmar
    m_seleccion = 0; // opcion resaltada
    m_fotograma = 0; // para animaciones
    m_terminado = false; // cuando el jugador confirma, se marca como terminado para que el main cambie de estado
    m_siguiente = EstadoJuego::MENU; // el estado al que se pasará cuando termine (puede ser MENU, TABLERO, ARENA, RANKING...)
    m_cfg = ConfigPartida(); // configuración de partida (modo, bando, batalla, turno1)
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
    }
    else if (m_paso == 1) {
        m_cfg.bando = (m_seleccion == 0) ? BandoJugador::CRISTIANO : BandoJugador::MUSULMAN;
        m_paso = 2; m_seleccion = 0;
    }
    else if (m_paso == 2) {
        m_cfg.batalla = (Batalla)m_seleccion;
        m_cfg.turno1 = iniciativa(m_cfg.batalla); //para mostrar quien empieza
        m_paso = 3; m_seleccion = 0;
    }
    else if (m_paso == 3) {
        if (m_seleccion == 0) {
            m_siguiente = EstadoJuego::DESTINO;
            m_terminado = true;
        }
        else {
            m_paso = 2; m_seleccion = 0;
        }
    }
}