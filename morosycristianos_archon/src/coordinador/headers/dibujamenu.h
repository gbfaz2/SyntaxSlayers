// AUTOR: INES ALCÉRRECA SÁNCHEZ
// DIBUJAMENU — TODO EL DIBUJO DE PANTALLAINTRO Y MENUPRINCIPAL

#pragma once
#include "dibuja.h"
#include "menu.h"
#include "GestorPartida.h"
#include <string>
#include "GestorRanking.h"

// DECLARACIÓN ANTICIPADA
class PantallaDestino;

class DibujaMenu : public Dibuja {
    // SECCIONES DEL MENÚ
    static void menu_fondo(int ancho, int alto);
    static void menu_titulo(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso0(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso1(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso2(MenuPrincipal& m, int ancho, int alto);
    static void menu_paso3(MenuPrincipal& m, int ancho, int alto);
    static void menu_pie(int ancho, int alto);
    static void menu_paso_nombre(MenuPrincipal& m, int ancho, int alto);

    // PRIMITIVAS COMPARTIDAS DEL MENÚ 
    static void menu_texto(const std::string& texto,
        float x, float y,
        float r, float g, float b,
        void* fuente = nullptr);

    static void menu_textoCentrado(const std::string& texto,
        float cx, float y,
        float r, float g, float b,
        void* fuente = nullptr);

    static void menu_rectangulo(float x, float y,
        float ancho, float alto,
        float r, float g, float b,
        float alfa = 1.0f);

    static void menu_borde(float x, float y,
        float ancho, float alto,
        float r, float g, float b,
        float grosor = 2.0f);

    static void menu_opcion(const std::string& texto,
        float x, float y,
        float ancho, float alto,
        bool seleccionada);

    // FUNCIONES DE DIBUJO DE DESTINO
    static void destino_fondo(const PantallaDestino& p, int ancho, int alto);
    static void destino_efectos(const PantallaDestino& p, int ancho, int alto);
    static void destino_particulas(const PantallaDestino& p);
    static void destino_textos(const PantallaDestino& p, int ancho, int alto);
    static void destino_continuar(const PantallaDestino& p, int ancho, int alto);



    static void victoria_dibujar(int ancho, int alto, const std::string& ganador,
        const std::string& batalla, bool ganaJ1, float tiempoRestante);

public:
    // ── PANTALLA INTRO
    static void intro_dibujar(PantallaIntro& p, int ancho, int alto);

    // ── MENÚ PRINCIPAL
    static void menu_dibujar(MenuPrincipal& m, int ancho, int alto);

    // ── PANTALLA DESTINO
    static void destino_dibujar(PantallaDestino& p, int ancho, int alto);

    // Para el ranking
    static void ranking_dibujar(int ancho, int alto, const std::string& ganador, const std::string& batalla, int turnos, int piezasLocal, int piezasRival, const std::vector<EntradaRanking>& ranking, bool ganaJ1);

    static void victoria_dibujar(int ancho, int alto, const std::string& ganador,
        const std::string& batalla, bool ganaJ1, float tiempoRestante);
};