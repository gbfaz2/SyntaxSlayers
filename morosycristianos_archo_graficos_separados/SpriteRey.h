#pragma once
// SpriteRey.h
// Sistema de animacion por spritesheet para el Rey Cristiano en el tablero.
//
// USO:
//   1. Llama SpriteRey::cargarTextura("imagenes\\IMAGEN_REY_CRISTIANO.png")
//      una sola vez al inicializar en tablerogl las figuras.
//   2. En cada casilla donde haya un Rey, llama:
//        spriteRey.setEstado(EstadoRey::IDLE / WALK / ATTACK / HURT / DEATH);
//        spriteRey.update(dt);
//        spriteRey.dibujar(cx, cy, size);   // cx,cy = centro OpenGL de la casilla
//   3. Llama SpriteRey::liberarTextura() al destruir el tablero.

#include "freeglut.h"
#include <string>

//Estados posibles del Rey (hay que añadir con hechizos???? o no)
enum class EstadoRey {
    IDLE,    // parado/respirando    (4 frames)
    WALK,    // moviéndose           (12 frames)
    ATTACK,  // atacando en combate  (10 frames)
    HURT,    // recibiendo daño      (4 frames)
    DEATH    // muriendo             (5 frames, se congela en el último)
};

//Coordenadas exactas para cada frame de la imagen para saber que dibujar en que momento
struct FrameUV {
    float u0, v0, u1, v1;
};


class SpriteRey {
public:
    //constructor principal
    SpriteRey();

    // Carga/libera la textura compartida (llamar una sola vez por partida)
    static void cargarTextura(const std::string& ruta);
    static void liberarTextura();

    // Cambia el estado de animación.
    // Si el estado nuevo es distinto al actual, reinicia el frame dpeendiendo del estado.
    void setEstado(EstadoRey nuevoEstado);

    //lee el estado actual del rey (getter)
    EstadoRey getEstado() const { return _estado; }

    // Avanza la animación (llamar cada frame con dt en segundos)
    //en el mueve(double dt) para actualizar
    void update(double dt);

    // Dibuja el sprite centrado en (cx, cy) con tamaño 'size' en coords OpenGL.
    // Llama esto dentro de un contexto 2D ortográfico (entrar2D/salir2D).
    void dibujar(float cx, float cy, float size) const;

    // ¿Ha terminado una animación de una sola pasada? (HURT, ATTACK, DEATH)
    //comrpueba
    bool animacionTerminada() const { return _terminada; }

private:
    //Textura compartida por todas las instancias
    static GLuint   s_texID;
    static bool     s_cargada;

    //Estado de animación: estado del rey, frame, timer y terminada o no
    EstadoRey _estado{ EstadoRey::IDLE };
    int       _frameActual{ 0 };
    double    _timerFrame{ 0.0 };   // tiempo acumulado en el frame actual
    bool      _terminada{ false }; // true cuando HURT/ATTACK/DEATH completan un ciclo

    //Configuración de velocidad por animación (segundos por frame) según queramos que luzca el personaje
    static constexpr double FPS_IDLE = 0.20;  //  5 fps  -> aspecto tranquilo
    static constexpr double FPS_WALK = 0.10;  // 10 fps  -> caminar fluido
    static constexpr double FPS_ATTACK = 0.07;  // ~14 fps -> ataque rápido
    static constexpr double FPS_HURT = 0.12;  //  8 fps  -> golpe visible
    static constexpr double FPS_DEATH = 0.18;  //  5.5 fps -> muerte dramática

    // ── Tablas de frames ──────────────────────────────────────────────────────
    // Imagen: 6336 x 2688 px
    // Izquierda (x < 0.5): 4 cols x 4 filas, celda 792x672
    // Derecha   (x >= 0.5): 5 cols x 4 filas, celda 633x672

    //inicializa las coordenadas según los frames teniendo en cuenta un array de una enum
    static const FrameUV FRAMES_IDLE[4];
    static const FrameUV FRAMES_WALK[12];
    static const FrameUV FRAMES_ATTACK[10];
    static const FrameUV FRAMES_HURT[4];
    static const FrameUV FRAMES_DEATH[5];

    //metodos para saber la frame actual, el total de frames y los segs por frame
    const FrameUV* framesActuales() const;
    int            totalFrames()    const;
    double         segsPerFrame()   const;
};