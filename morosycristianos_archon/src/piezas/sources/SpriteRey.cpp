#include "SpriteRey.h"
// SpriteRey.cpp
// Implementación del sistema de animación por spritesheet para el Rey Cristiano.

#include "ETSIDI.h"   // getTexture — misma API que usa pantallabatalla.cpp
#include <iostream>

//inicialización de variables estáticas
GLuint SpriteRey::s_texID = 0;
bool   SpriteRey::s_cargada = false;


// TABLAS DE FRAMES  (u0, v0, u1, v1)
// Imagen 6336x2688.  OpenGL espera v=0 abajo, pero la imagen tiene v=0 arriba,
// por eso v0 < v1 aquí y al dibujar usamos glTexCoord2f(u, 1-v) para invertir.
//se establecen las coordenadas exactas por frame por actividad:

// IDLE (respirar normal)— fila 0 izquierda, 4 frames (celda 792x672)
const FrameUV SpriteRey::FRAMES_IDLE[4] = {
    { 0.0000f, 0.0000f, 0.1250f, 0.2500f },  // frame 0
    { 0.1250f, 0.0000f, 0.2500f, 0.2500f },  // frame 1
    { 0.2500f, 0.0000f, 0.3750f, 0.2500f },  // frame 2
    { 0.3750f, 0.0000f, 0.5000f, 0.2500f },  // frame 3
};

// WALK — filas 1-3 izquierda, 12 frames (4 por fila)
const FrameUV SpriteRey::FRAMES_WALK[12] = {
    { 0.0000f, 0.2500f, 0.1250f, 0.5000f },  // fila1 frame 0
    { 0.1250f, 0.2500f, 0.2500f, 0.5000f },  // fila1 frame 1
    { 0.2500f, 0.2500f, 0.3750f, 0.5000f },  // fila1 frame 2
    { 0.3750f, 0.2500f, 0.5000f, 0.5000f },  // fila1 frame 3
    { 0.0000f, 0.5000f, 0.1250f, 0.7500f },  // fila2 frame 0
    { 0.1250f, 0.5000f, 0.2500f, 0.7500f },  // fila2 frame 1
    { 0.2500f, 0.5000f, 0.3750f, 0.7500f },  // fila2 frame 2
    { 0.3750f, 0.5000f, 0.5000f, 0.7500f },  // fila2 frame 3
    { 0.0000f, 0.7500f, 0.1250f, 1.0000f },  // fila3 frame 0
    { 0.1250f, 0.7500f, 0.2500f, 1.0000f },  // fila3 frame 1
    { 0.2500f, 0.7500f, 0.3750f, 1.0000f },  // fila3 frame 2
    { 0.3750f, 0.7500f, 0.5000f, 1.0000f },  // fila3 frame 3
};

// ATTACK — filas 0-1 derecha, 10 frames (5 por fila)
const FrameUV SpriteRey::FRAMES_ATTACK[10] = {
    { 0.5000f, 0.0000f, 0.5999f, 0.2500f },  // fila0 frame 0
    { 0.5999f, 0.0000f, 0.6998f, 0.2500f },  // fila0 frame 1
    { 0.6998f, 0.0000f, 0.7997f, 0.2500f },  // fila0 frame 2
    { 0.7997f, 0.0000f, 0.8996f, 0.2500f },  // fila0 frame 3
    { 0.8996f, 0.0000f, 0.9995f, 0.2500f },  // fila0 frame 4
    { 0.5000f, 0.2500f, 0.5999f, 0.5000f },  // fila1 frame 0
    { 0.5999f, 0.2500f, 0.6998f, 0.5000f },  // fila1 frame 1
    { 0.6998f, 0.2500f, 0.7997f, 0.5000f },  // fila1 frame 2
    { 0.7997f, 0.2500f, 0.8996f, 0.5000f },  // fila1 frame 3
    { 0.8996f, 0.2500f, 0.9995f, 0.5000f },  // fila1 frame 4
};

// HURT — fila 2 derecha, 4 frames
const FrameUV SpriteRey::FRAMES_HURT[4] = {
    { 0.5000f, 0.5000f, 0.5999f, 0.7500f },  // frame 0
    { 0.5999f, 0.5000f, 0.6998f, 0.7500f },  // frame 1
    { 0.6998f, 0.5000f, 0.7997f, 0.7500f },  // frame 2
    { 0.7997f, 0.5000f, 0.8996f, 0.7500f },  // frame 3
};

// DEATH — fila 3 derecha, 5 frames
const FrameUV SpriteRey::FRAMES_DEATH[5] = {
    { 0.5000f, 0.7500f, 0.5999f, 1.0000f },  // frame 0
    { 0.5999f, 0.7500f, 0.6998f, 1.0000f },  // frame 1
    { 0.6998f, 0.7500f, 0.7997f, 1.0000f },  // frame 2
    { 0.7997f, 0.7500f, 0.8996f, 1.0000f },  // frame 3
    { 0.8996f, 0.7500f, 0.9995f, 1.0000f },  // frame 4
};


// Constructor
SpriteRey::SpriteRey() //inicializa el estado del rey a IDLE (natural normal), el numero del frame, timer y dice que NO esta terminada
    : _estado(EstadoRey::IDLE), _frameActual(0),
    _timerFrame(0.0), _terminada(false)
{}


// Carga / libera textura  (estáticos)
void SpriteRey::cargarTextura(const std::string& ruta)
{
    //carga la textura
    if (s_cargada) return;
    auto tex = ETSIDI::getTexture(ruta.c_str());
    s_texID = tex.id;
    s_cargada = (s_texID != 0);
    //informa si se ha cargado bien por pantalla
    if (!s_cargada)
        std::cerr << "[SpriteRey] ERROR: no se pudo cargar " << ruta << "\n";
    else
        std::cout << "[SpriteRey] Textura cargada, id=" << s_texID << "\n";
}

//libera la textura cuando sea necesario eliminar el personaje
void SpriteRey::liberarTextura()
{
    if (s_cargada) {
        glDeleteTextures(1, &s_texID);
        s_texID = 0;
        s_cargada = false;
    }
}

//setEstado, reinicia los estados y los actualiza a nuevoEstado, actualizando también timers y numero de frames si se reinicia despues de la muerte del personaje
void SpriteRey::setEstado(EstadoRey nuevoEstado)
{
    if (_estado == nuevoEstado) return;

    // Si ya está muerto, no salimos de DEATH
    if (_estado == EstadoRey::DEATH && _terminada) return;

    _estado = nuevoEstado;
    _frameActual = 0;
    _timerFrame = 0.0;
    _terminada = false;
}


// Helpers internos
const FrameUV* SpriteRey::framesActuales() const
{
    switch (_estado) {
    case EstadoRey::IDLE:   return FRAMES_IDLE;
    case EstadoRey::WALK:   return FRAMES_WALK;
    case EstadoRey::ATTACK: return FRAMES_ATTACK;
    case EstadoRey::HURT:   return FRAMES_HURT;
    case EstadoRey::DEATH:  return FRAMES_DEATH;
    }
    return FRAMES_IDLE;
}

//los frames totales según el estado del rey
int SpriteRey::totalFrames() const
{
    switch (_estado) {
    case EstadoRey::IDLE:   return 4;
    case EstadoRey::WALK:   return 12;
    case EstadoRey::ATTACK: return 10;
    case EstadoRey::HURT:   return 4;
    case EstadoRey::DEATH:  return 5;
    }
    return 4;
}

//retorna los segundos por frames
double SpriteRey::segsPerFrame() const
{
    switch (_estado) {
    case EstadoRey::IDLE:   return FPS_IDLE;
    case EstadoRey::WALK:   return FPS_WALK;
    case EstadoRey::ATTACK: return FPS_ATTACK;
    case EstadoRey::HURT:   return FPS_HURT;
    case EstadoRey::DEATH:  return FPS_DEATH;
    }
    return FPS_IDLE;
}


// update  — avanza la animación
// METER DENTRO DE MUEVE(DOUBLE DT) en tablerogl
void SpriteRey::update(double dt)
{
    if (_terminada) return;  // animación congelada (DEATH último frame)

    _timerFrame += dt;
    if (_timerFrame < segsPerFrame()) return;

    _timerFrame -= segsPerFrame();
    _frameActual++;

    int total = totalFrames();

    switch (_estado) {
        // Animaciones en bucle
    case EstadoRey::IDLE:
    case EstadoRey::WALK:
        if (_frameActual >= total)
            _frameActual = 0;
        break;

        // Animaciones de una sola pasada: se congelan en el último frame (attack y death porque no se recupera)
    case EstadoRey::ATTACK:
    case EstadoRey::HURT:
        if (_frameActual >= total) {
            _frameActual = total - 1;
            _terminada = true;
        }
        break;

        // DEATH: igual que las anteriores pero además señalizamos al exterior
    case EstadoRey::DEATH:
        if (_frameActual >= total) {
            _frameActual = total - 1;  // congela en el último frame (Cruz)
            _terminada = true;
        }
        break;
    }
}


// dibujar  — renderiza el frame actual centrado en (cx, cy)
// Llama esto dentro de un bloque entrar2D / salir2D del tablero.

void SpriteRey::dibujar(float cx, float cy, float size) const
{
    if (!s_cargada) return;

    const FrameUV& f = framesActuales()[_frameActual];

    // La imagen tiene v=0 arriba; OpenGL espera v=0 abajo → invertimos V
    float v0 = 1.0f - f.v1;
    float v1 = 1.0f - f.v0;

    float hw = size * 0.5f;  // mitad ancho
    float hh = size * 0.5f;  // mitad alto (el sprite es cuadrado por celda)

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, s_texID);

    // Transparencia: el fondo gris del spritesheet se descarta con blending
    // (funciona bien si la imagen tiene fondo sólido uniforme ~192,192,192)
    glEnable(GL_BLEND);
    glDisable(GL_BLEND);
   // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // sin tinte de color

    glBegin(GL_QUADS);
    glTexCoord2f(f.u0, v1);  glVertex2f(cx - hw, cy - hh);  // inf-izq
    glTexCoord2f(f.u1, v1);  glVertex2f(cx + hw, cy - hh);  // inf-der
    glTexCoord2f(f.u1, v0);  glVertex2f(cx + hw, cy + hh);  // sup-der
    glTexCoord2f(f.u0, v0);  glVertex2f(cx - hw, cy + hh);  // sup-izq
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}