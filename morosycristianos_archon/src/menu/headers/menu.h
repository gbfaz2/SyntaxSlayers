// Autor: Ines Alcérreca Sánchez
// Pantallas E_INTRO y E_MENU

#pragma once
#include "estadojuego.h"
#include <string>
#include "dibuja.h"
#include "GestorPartida.h"

class DibujaMenu; // DECLARACION ANTICIPADA PARA LA AMISTAD

// ============================================================
// PANTALLA INTRO (E_INTRO)
// Muestra el GIF de introducción con fade y texto de skip
// ============================================================
class PantallaIntro {
    int  m_fotograma;
    bool m_terminado;

    static const int DURACION = 420; // ~7 segundos a 60fps

    friend class DibujaMenu;

public:
    PantallaIntro();
    void saltar();
    bool terminado() const { return m_terminado; }
    void reiniciar();
};


// ============================================================
// MENÚ PRINCIPAL (E_MENU)
//
// FLUJO DE PASOS:
//   0 → Menú principal (Cargar / Nueva / Ayuda / Ranking / Salir)
//   1 → Elegir modo    (JVJ / JVIA)
//   2 → Configuración  (nombres + bandos/dificultad en dos columnas)
//   3 → Selección de batalla
//   4 → Confirmar (Jugar / Volver)
// ============================================================
class MenuPrincipal {

    // PASO ACTUAL DEL FLUJO
    int m_paso;

    // OPCIÓN RESALTADA EN EL PASO ACTUAL
    int m_seleccion;

    // CONTADOR DE FOTOGRAMAS PARA ANIMACIONES
    int m_fotograma;

    // TRUE CUANDO EL JUGADOR HA CONFIRMADO Y SE PUEDE CAMBIAR DE ESTADO
    bool m_terminado;

    // ESTADO AL QUE SE PASARÁ CUANDO m_terminado SEA TRUE
    EstadoJuego m_siguiente;

    // CONFIGURACIÓN QUE SE VA RELLENANDO A LO LARGO DEL MENÚ
    ConfigPartida m_cfg;

    // TEXTO QUE ESTÁ ESCRIBIENDO EL JUGADOR EN EL PASO DE NOMBRES
    std::string m_nombreJ1Actual; // NOMBRE QUE ESCRIBE J1
    std::string m_nombreJ2Actual; // NOMBRE QUE ESCRIBE J2

    // INDICA QUÉ CAMPO TIENE EL FOCO DE ESCRITURA EN EL PASO 2
    // 0 = nombre J1, 1 = nombre J2
    int m_focoNombre;

    // CONFIRMA LA SELECCIÓN ACTUAL Y AVANZA AL SIGUIENTE PASO
    void confirmar();

    // CAPTURA TECLAS DE TEXTO PARA ESCRIBIR NOMBRES
    void teclaTextoJ1(unsigned char key);
    void teclaTextoJ2(unsigned char key);

    // DEVUELVE CUÁNTAS OPCIONES HAY EN EL PASO ACTUAL
    // (USADO POR GestorInput PARA NAVEGAR CON FLECHAS)
    int maxOpciones() const {
        switch (m_paso) {
<<<<<<< Updated upstream
        case 0: return 5;  // modo
        case 2: return 2;  // bando
        case 4: return 4;  // batalla
        case 5: return 2;  // confirmar
        default: return 0; // pasos de texto (1 y 3)
=======
        case 0: return 5; // CARGAR / NUEVA / AYUDA / RANKING / SALIR
        case 1: return 2; // JVJ / JVIA
        case 2: return 0; // PANTALLA UNIFICADA — INPUTS PROPIOS, NO USA FLECHAS GLOBALES
        case 3: return 4; // 4 BATALLAS
        case 4: return 2; // JUGAR / VOLVER
        default: return 0;
>>>>>>> Stashed changes
        }
    }

    static void menu_paso0(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso1(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso2(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso3(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso4(MenuPrincipal& m, int ancho, int alto);

public:
    MenuPrincipal();

    void dibujar(int ancho, int alto);

    bool          terminado()       const { return m_terminado; }
    EstadoJuego   siguienteEstado() const { return m_siguiente; }
    ConfigPartida getConfiguracion()const { return m_cfg; }

    void reiniciar();

    friend class GestorInput; // ACCESO TOTAL PARA GESTIONAR INPUTS
    friend class DibujaMenu;  // ACCESO TOTAL PARA DIBUJAR
};