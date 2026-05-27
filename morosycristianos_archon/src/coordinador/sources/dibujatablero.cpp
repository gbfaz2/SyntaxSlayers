#include "dibujatablero.h"
#include "freeglut.h"
#include "ETSIDI.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>
#include "Infiltrado.h"

static const int SEGS = 24; // NUMERO DE SEGMENTOS PARA DIBUJAR LOS CIRCULOS

dibujapersonajes DibujaTablero::_dibujador; //definir como variable estática


// INICIALIZACIÓN DE LUCES Y PERSPECTIVA

void DibujaTablero::tablero_init() {
    glEnable(GL_LIGHT0);         // ACTIVA LA FUENTE DE LUZ
    glEnable(GL_LIGHTING);       // ACTIVA EL SISTEMA DE ILUMINACIÓN DE OPENGL
    glEnable(GL_DEPTH_TEST);     // TEST DE PROFUNDIDAD PARA EL 3D
    glEnable(GL_COLOR_MATERIAL); // PERMITE USAR GLCOLOR3F CON ILUMINACIÓN ACTIVA

    glMatrixMode(GL_PROJECTION); // MATRIZ DE PROYECCIÓN
    gluPerspective(40.0, 800.0 / 600.0, 0.1, 150.0);

    _dibujador.init(); //inicializa los personajes dibujados
}

// ORQUESTADOR PRINCIPAL: DIBUJA TODO EL FRAME DEL TABLERO

void DibujaTablero::tablero_dibujar(Tablerogl& t) {
    _dibujador.update(); //para que se vaya actualizando los personajes

    glEnable(GL_DEPTH_TEST);    // QUE LAS PIEZAS NO SE SOLAPEN MAL
    glEnable(GL_LIGHTING);      // ENCENDER LAS LUCES
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_PROJECTION); // SELECCIONAR LA LENTE DE LA CÁMARA
    glLoadIdentity();
    gluPerspective(40.0, 1024.0 / 768.0, 0.1, 150.0);

    // RECALCULAMOS EL CENTRO LEYENDO DE 't'
    t.centro_x = t.N * t.ancho / 2.0f;
    t.centro_y = -t.N * t.ancho / 2.0f;

    // COLOR DE FONDO DE LA VENTANA
    glClearColor(0.10f, 0.06f, 0.03f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // COLOCAMOS LA CÁMARA
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        t.centro_x, t.centro_y, t.dist, // POSICIÓN DEL OJO
        t.centro_x, t.centro_y, t.centro_z, // PUNTO AL QUE MIRA
        0.0, 1.0, 0.0 // VECTOR "ARRIBA"
    );

    glDisable(GL_LIGHTING); // APAGAMOS LUCES PARA PINTAR FONDOS PLANOS

    // ORDEN DE CAPAS DE DIBUJO
    tablero_fondo(t);
    glClear(GL_DEPTH_BUFFER_BIT); // LIMPIAMOS PROFUNDIDAD PARA QUE EL TABLERO QUEDE ENCIMA DEL FONDO
    tablero_casillas(t);
    tablero_marco(t);
    tablero_simbolos(t);

    // PELICULA SEMITRANSPARENTE PARA ATENUAR EL TABLERO OSCURECIÉNDOLO UN POCO
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.35f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.003f);
    glVertex3f(t.N * t.ancho, 0.0f, 0.003f);
    glVertex3f(t.N * t.ancho, -t.N * t.ancho, 0.003f);
    glVertex3f(0.0f, -t.N * t.ancho, 0.003f);
    glEnd();
    glDisable(GL_BLEND);

    // DIBUJO DE INTERFAZ DEL JUGADOR SOBRE EL TABLERO
    tablero_cursores(t);
    tablero_seleccion(t);
    tablero_movimientos_validos(t);

    // ENCENDEMOS LUCES Y DIBUJAMOS LOS MODELOS 3D DE LAS PIEZAS
    glEnable(GL_LIGHTING);
    tablero_piezas(t);
    glDisable(GL_LIGHTING);

    // RECTÁNGULO TRANSPARENTE PARA GARANTIZAR CAPTURA DE RAYCAST DEL RATÓN
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.0f); // COMPLETAMENTE INVISIBLE
    glPushMatrix();
    glTranslatef((float)t.centro_x, (float)t.centro_y, 0.0f);
    float mitad = t.N * t.ancho / 2.0f;
    glRectf(-mitad, mitad, mitad, -mitad);
    glPopMatrix();
    glDisable(GL_BLEND);
    
    //CONTADORES, MENSAJE MOVIMIENTO INVÁLIDO Y MENÚ DE PIEZA
    util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
    tablero_contadores(t);
    tablero_panel_pieza(t);
    tablero_panel_hechizos(t);
    tablero_panel_infiltrado(t);
    tablero_mensaje_invalido(t);
    util_salir2D();
}

// CAPAS BASE DEL TABLERO E IMÁGENES

void DibujaTablero::tablero_fondo(const Tablerogl& t) {
    // SELECCIONA FONDO SEGUN BATALLA
    const char* ruta = "imagenes/fondo.png";
    switch (t._batallaActual) {
    case 0: ruta = "imagenes/fondo_guadalete.png"; break;
    case 1: ruta = "imagenes/fondo_alarcos.png";   break;
    case 2: ruta = "imagenes/fondo_navas.png";     break;
    case 3: ruta = "imagenes/fondo_granada.png";   break;
    }

    auto tex = ETSIDI::getTexture(ruta);
    if (tex.id == 0) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    // QUAD GRANDE CENTRADO EN EL TABLERO, MUY POR DETRÁS EN Z
    float cx = (float)t.centro_x;
    float cy = (float)t.centro_y;
    float medio = 2.5f;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(cx - medio, cy - medio, -0.5f);
    glTexCoord2f(1, 1); glVertex3f(cx + medio, cy - medio, -0.5f);
    glTexCoord2f(1, 0); glVertex3f(cx + medio, cy + medio, -0.5f);
    glTexCoord2f(0, 0); glVertex3f(cx - medio, cy + medio, -0.5f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void DibujaTablero::tablero_casillas(const Tablerogl& t) {
    for (int fila = 0; fila < t.N; fila++) {
        for (int col = 0; col < t.N; col++) {
            tablero_color_casilla(t, fila, col);
            tablero_casilla_individual(t, fila, col);
        }
    }
}

void DibujaTablero::tablero_color_casilla(const Tablerogl& t, int fila, int col) {
    switch (t.m_tablero->getCasilla(fila, col).tipo) {
    case Casilla_local:
        glColor3f(0.75f, 0.15f, 0.15f); // ROJO MAS CLARO 
        return;
    case Casilla_rival:
        glColor3f(0.70f, 0.30f, 0.85f); // MORADO MAS CLARO
        return;
    case Casilla_neutra:
        glColor3f(0.50f, 0.50f, 0.50f); // GRIS MAS CLARO
        return;
    case Casilla_poder:
        if (col < 4)       glColor3f(0.55f, 0.10f, 0.65f); // zona morada
        else if (col == 4) glColor3f(0.35f, 0.35f, 0.35f); // zona gris
        else               glColor3f(0.50f, 0.04f, 0.04f); // zona roja
        return;
    default:
        glColor3f(0.5f, 0.5f, 0.5f);
    }
}

void DibujaTablero::tablero_casilla_individual(const Tablerogl& t, int fila, int col) {
    float x0 = col * t.ancho;
    float x1 = x0 + t.ancho;
    float y0 = -fila * t.ancho;
    float y1 = y0 - t.ancho;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // APUNTA HACIA LA CÁMARA
    glVertex3f(x0, y0, -0.001f);
    glVertex3f(x1, y0, -0.001f);
    glVertex3f(x1, y1, -0.001f);
    glVertex3f(x0, y1, -0.001f);
    glEnd();
}

void DibujaTablero::tablero_marco(const Tablerogl& t) {
    glDisable(GL_LIGHTING);

    float margen = 0.06f;
    float x0 = -margen;
    float x1 = t.N * t.ancho + margen;
    float y0 = margen;
    float y1 = -(t.N * t.ancho) - margen;
    float z = 0.007f;

    // CARGA TEXTURA DE MADERA DESDE ETSIDI
    auto texMarco = ETSIDI::getTexture("imagenes/marco_tablero.png");
    bool tieneTextura = (texMarco.id != 0);

    if (tieneTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texMarco.id);
        glColor3f(1, 1, 1);
    }
    else {
        glColor3f(0.45f, 0.22f, 0.08f); // CAOBA DE FALLBACK
    }

    // DIBUJA LOS 4 LADOS DEL MARCO CON LA TEXTURA
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(x0, y0, z);
    glTexCoord2f(1, 0); glVertex3f(x1, y0, z);
    glTexCoord2f(1, 1); glVertex3f(x1, 0.0f, z);
    glTexCoord2f(0, 1); glVertex3f(x0, 0.0f, z);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(x0, -(t.N * t.ancho), z);
    glTexCoord2f(1, 0); glVertex3f(x1, -(t.N * t.ancho), z);
    glTexCoord2f(1, 1); glVertex3f(x1, y1, z);
    glTexCoord2f(0, 1); glVertex3f(x0, y1, z);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(x0, y0, z);
    glTexCoord2f(1, 0); glVertex3f(0.0f, y0, z);
    glTexCoord2f(1, 1); glVertex3f(0.0f, y1, z);
    glTexCoord2f(0, 1); glVertex3f(x0, y1, z);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(t.N * t.ancho, y0, z);
    glTexCoord2f(1, 0); glVertex3f(x1, y0, z);
    glTexCoord2f(1, 1); glVertex3f(x1, y1, z);
    glTexCoord2f(0, 1); glVertex3f(t.N * t.ancho, y1, z);
    glEnd();

    if (tieneTextura) glDisable(GL_TEXTURE_2D);

    // BORDES OSCUROS (INTERIOR Y EXTERIOR)
    glColor3f(0.15f, 0.07f, 0.02f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x0, y0, z + 0.001f); glVertex3f(x1, y0, z + 0.001f);
    glVertex3f(x1, y1, z + 0.001f); glVertex3f(x0, y1, z + 0.001f);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(0.0f, 0.0f, z + 0.001f); glVertex3f(t.N * t.ancho, 0.0f, z + 0.001f);
    glVertex3f(t.N * t.ancho, -(t.N * t.ancho), z + 0.001f); glVertex3f(0.0f, -(t.N * t.ancho), z + 0.001f);
    glEnd();
    glLineWidth(1.0f);

    // ── MAGIA DE COORDENADAS PARA DIBUJAR LAS LETRAS EN 2D SOBRE EL MUNDO 3D ──
    GLdouble modelview[16], projection[16];
    GLint viewport[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.90f, 0.85f, 0.70f, 1.0f);

    // LETRAS COLUMNAS (A-I) EN MARCO SUPERIOR
    for (int col = 0; col < t.N; col++) {
        float cx, cy;
        // NECESITAMOS LLAMAR A LA FUNCION MATEMATICA DE TABLEROGL PARA CONVERTIR COORDENADAS
        const_cast<Tablerogl*>(&t)->cell2center(0, col, cx, cy);
        GLdouble sx, sy, sz;
        gluProject(cx, y0 * 0.3f, z, modelview, projection, viewport, &sx, &sy, &sz);
        char letra[2] = { (char)('A' + col), '\0' };
        ETSIDI::printxy(letra, (int)sx, (int)sy);
    }

    // NÚMEROS FILAS (1-9) EN MARCO IZQUIERDO
    for (int fila = 0; fila < t.N; fila++) {
        float cx, cy;
        const_cast<Tablerogl*>(&t)->cell2center(fila, 0, cx, cy);
        GLdouble sx, sy, sz;
        gluProject(x0 * 0.6f, cy, z, modelview, projection, viewport, &sx, &sy, &sz);
        char numero[3];
        sprintf_s(numero, "%d", fila + 1);
        ETSIDI::printxy(numero, (int)sx, (int)sy);
    }

    util_salir2D();
    glEnable(GL_LIGHTING);
}

// DECORACIÓN DEL TABLERO Y EFECTOS

void DibujaTablero::tablero_simbolos(const Tablerogl& t) {
    for (int fila = 0; fila < t.N; fila++) {
        for (int col = 0; col < t.N; col++) {
            float cx, cy;
            const_cast<Tablerogl*>(&t)->cell2center(fila, col, cx, cy);
            float size = t.ancho * 0.34f;
            TipoCasilla tipo = t.m_tablero->getCasilla(fila, col).tipo;

            if (tipo == Casilla_local) tablero_cruz(cx, cy, size);
            else if (tipo == Casilla_rival) tablero_luna(cx, cy, size);
            else if (tipo == Casilla_poder) tablero_punto_poder(cx, cy, size);
        }
    }
}

void DibujaTablero::tablero_cruz(float cx, float cy, float size) {
    glColor3f(1.0f, 1.0f, 1.0f);
    float brazo = size;
    float mitad_brazo = size * 0.30f;

    glBegin(GL_QUADS);
    glVertex3f(cx - mitad_brazo, cy + brazo, 0.002f);
    glVertex3f(cx + mitad_brazo, cy + brazo, 0.002f);
    glVertex3f(cx + mitad_brazo, cy - brazo, 0.002f);
    glVertex3f(cx - mitad_brazo, cy - brazo, 0.002f);

    glVertex3f(cx - brazo, cy + mitad_brazo, 0.002f);
    glVertex3f(cx + brazo, cy + mitad_brazo, 0.002f);
    glVertex3f(cx + brazo, cy - mitad_brazo, 0.002f);
    glVertex3f(cx - brazo, cy - mitad_brazo, 0.002f);
    glEnd();
}

void DibujaTablero::tablero_luna(float cx, float cy, float size) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, 0.002f);
    for (int i = 0; i <= SEGS; i++) {
        float a = -3.14159265f / 2.0f - 3.14159265f * i / SEGS;
        glVertex3f(cx + size * cosf(a), cy - size * sinf(a), 0.002f);
    }
    glEnd();
}

void DibujaTablero::tablero_punto_poder(float cx, float cy, float size) {
    glColor3f(1.0f, 0.90f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < SEGS; i++) {
        float angulo = 2.0f * (float)3.14159265f * i / SEGS;
        glVertex3f(cx + size * cos(angulo), cy + size * sin(angulo), 0.002f);
    }
    glEnd();
    glLineWidth(1.0f);

    float r = size * 0.30f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(cx, cy, 0.002f);
    for (int i = 0; i <= SEGS; i++) {
        float angulo = 2.0f * (float)3.14159265f * i / SEGS;
        glVertex3f(cx + r * cos(angulo), cy + r * sin(angulo), 0.002f);
    }
    glEnd();
}

// HUD DEL JUGADOR SOBRE EL TABLERO (SELECCIÓN Y MOVIMIENTOS)

void DibujaTablero::tablero_casilla_resaltada(const Tablerogl& t, int fila, int col, float r, float g, float b, float lw, float z) {
    float x0 = col * t.ancho + 0.004f, x1 = (col + 1) * t.ancho - 0.004f;
    float y0 = -fila * t.ancho - 0.004f, y1 = -(fila + 1) * t.ancho + 0.004f;
    glColor3f(r, g, b);
    glLineWidth(lw);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x0, y0, z); glVertex3f(x1, y0, z);
    glVertex3f(x1, y1, z); glVertex3f(x0, y1, z);
    glEnd();
    glLineWidth(1.0f);
}

void DibujaTablero::tablero_cursores(const Tablerogl& t) {
    tablero_casilla_resaltada(t, t.Filacursor[0], t.Colcursor[0], 1.0f, 1.0f, 0.0f, 3.0f, 0.005f); // BANDO LOCAL
    tablero_casilla_resaltada(t, t.Filacursor[1], t.Colcursor[1], 0.0f, 1.0f, 1.0f, 3.0f, 0.005f); // BANDO RIVAL
}

void DibujaTablero::tablero_seleccion(const Tablerogl& t) {
    if (!t.piezaSeleccionada || t.fromFila < 0) return;
    tablero_casilla_resaltada(t, t.fromFila, t.fromCol, 1.0f, 1.0f, 1.0f, 4.0f, 0.006f);
}

void DibujaTablero::tablero_movimientos_validos(const Tablerogl& t) {
    if (!t.piezaSeleccionada || t.fromFila < 0) return;  // NO HAY PIEZA SELECCIONADA
    if (t._modoHechizo) return;                           // MODO HECHIZO ACTIVO

    BandoPieza bandoActual = t.gestorTurnos.getBandoActual(); // BANDO CON TURNO
    const Casilla& cas = t.m_tablero->getCasilla(t.fromFila, t.fromCol);
    if (cas.bando != bandoActual) return;                 // PIEZA NO ES DEL TURNO ACTUAL

    auto validas = t.m_tablero->casillasValidas(t.fromFila, t.fromCol); // CASILLAS VÁLIDAS

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto& pos : validas) {
        float x0 = pos.col * t.ancho;
        float x1 = x0 + t.ancho;
        float y0 = -pos.fila * t.ancho;
        float y1 = y0 - t.ancho;

        glColor4f(1.0f, 1.0f, 0.0f, 0.25f);
        glBegin(GL_QUADS);
        glVertex3f(x0, y0, 0.004f); glVertex3f(x1, y0, 0.004f);
        glVertex3f(x1, y1, 0.004f); glVertex3f(x0, y1, 0.004f);
        glEnd();

        glColor4f(1.0f, 1.0f, 0.0f, 0.85f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(x0 + 0.003f, y0 - 0.003f, 0.005f);
        glVertex3f(x1 - 0.003f, y0 - 0.003f, 0.005f);
        glVertex3f(x1 - 0.003f, y1 + 0.003f, 0.005f);
        glVertex3f(x0 + 0.003f, y1 + 0.003f, 0.005f);
        glEnd();
        glLineWidth(1.0f);
    }
    glDisable(GL_BLEND);
}

// RENDERIZADO 3D DE LAS PIEZAS EN EL TABLERO

void DibujaTablero::tablero_piezas(Tablerogl& t) {
    for (int fila = 0; fila < t.N; fila++) {
        for (int col = 0; col < t.N; col++) {
            if (t.m_tablero->getCasilla(fila, col).pieza != pieza_nada)
                tablero_pieza_individual(t, fila, col);
        }
    }
}

void DibujaTablero::tablero_pieza_individual(Tablerogl& t, int fil, int col) {
    const Casilla& casilla = t.m_tablero->getCasilla(fil, col);
    float cx, cy;
    t.cell2center(fil, col, cx, cy);

    if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
        cx = t._animMov.origenX + (t._animMov.destinoX - t._animMov.origenX) * t._animMov.t;
        cy = t._animMov.origenY + (t._animMov.destinoY - t._animMov.origenY) * t._animMov.t;
    }

    GLdouble model[16], proj[16];
    GLint view[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);


    // COLOR BASE DEL BANDO
    if (casilla.bando == bando_local) glColor3f(1.00f, 0.95f, 0.85f);
    else                              glColor3f(0.90f, 0.55f, 0.15f);

    float escala = t.ancho * 0.30f;

    glPushMatrix();
    glTranslatef(cx, cy, escala + 0.03f); //elevamos sobre el tablero

    switch (casilla.pieza) {

    case pieza_esfera: {
        //REY / EMIR

        // Capturamos matrices ANTES de salir del contexto 3D
        GLdouble winX, winY, winZ;

        // Usamos la posición z donde están las piezas
        gluProject(cx, cy, 0.01, model, proj, view, &winX, &winY, &winZ);

        glPopMatrix();

        // Calculamos posición y tamaño en el plano 2D de la ventana
        
        
        util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
        glDisable(GL_LIGHTING);


        // Le pedimos a la pieza su ID único en lugar de usar un contador global
        TipoPersonaje tipo = dibujapersonajes::tipoDesdePieza(casilla.pieza, casilla.bando);
        int idAnim = casilla.obj ? casilla.obj->getIdAnimacion() : 0;
       
        bool voltear = (casilla.bando == bando_rival);


        EstadoPersonaje estadoEmir = EstadoPersonaje::IDLE;
        bool moviendo = false;


        // Para el emir: 
        if (casilla.bando != bando_local) {
            float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.85f;
            float px = (float)winX + size * 0.18f; //- size * 0.07f;
            float py = (float)winY + size * 0.2f;

            if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
                moviendo = true;
                estadoEmir = EstadoPersonaje::IDLE;

            }

            _dibujador.dibujar(tipo, px, py, size,
                estadoEmir, idAnim, moviendo, voltear);
            util_salir2D();
            return;
        }
        else {
            float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.93f;
            float px = (float)winX - size * 0.17f; //- size * 0.07f;
            float py = (float)winY + size * 0.1f;

            if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
                moviendo = true;
                estadoEmir = EstadoPersonaje::IDLE;

            }

            _dibujador.dibujar(tipo, px, py, size,
                estadoEmir, idAnim, moviendo, voltear);
            util_salir2D();
            return;

        }
 
    }
    break;

    case pieza_dodecaedro:
    //INFILTRADO
    {
        // Capturamos matrices ANTES de salir del contexto 3D
        GLdouble winX, winY, winZ;

        // Usamos la posición z donde están las piezas
        gluProject(cx, cy, 0.01, model, proj, view, &winX, &winY, &winZ);

        glPopMatrix();

        // Calculamos posición y tamaño en el plano 2D de la ventana
        float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.8f;
        float px = (float)winX - size * 0.3f;
        float py = (float)winY + size * 0.1f;

        util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
        glDisable(GL_LIGHTING);



        EstadoPersonaje estadoInfiltrado = EstadoPersonaje::IDLE;
        bool moviendo = false;

        // Si la pieza se está moviendo, cambiamos su estado a ATTACK
        if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
            moviendo = true;
            estadoInfiltrado = EstadoPersonaje::IDLE;
        }

        // Le pedimos a la pieza su ID único en lugar de usar un contador global
        TipoPersonaje tipo = dibujapersonajes::tipoDesdePieza(casilla.pieza, casilla.bando);
        int idAnim = casilla.obj ? casilla.obj->getIdAnimacion() : 0;

        
        // Solo dibujamos para el bando cristiano
        if (casilla.bando != bando_local) {
            bool voltear = true;
            _dibujador.dibujar(tipo, px, py, size,
                EstadoPersonaje::IDLE, idAnim, false, voltear);
            util_salir2D();
            return;
        }

        // Dibujamos el miliciano usando su ID
        _dibujador.dibujar(tipo, px, py, size,
            EstadoPersonaje::IDLE, idAnim, false);
        util_salir2D();

        

        return;
    }
    break;
    case pieza_icosaedro:
        glScalef(escala * 0.65f, escala * 0.65f, escala * 0.65f);
        glutSolidIcosahedron();
        break;

    case pieza_tetraedro:
    {
        // Filtramos el local, si cae en cristiano dibuja figura ( de momento):
        if (casilla.bando != bando_rival) {
            glScalef(escala * 0.70f, escala * 0.70f, escala * 0.70f);
            glutSolidTetrahedron();
            glPopMatrix();
            return;
        }

        //codigo dibujo jinete bereber

        // Capturamos matrices ANTES de salir del contexto 3D
        GLdouble winX, winY, winZ;

        // Usamos la posición z donde están las piezas
        gluProject(cx, cy, 0.01, model, proj, view, &winX, &winY, &winZ);

        glPopMatrix();

        // Calculamos posición y tamaño en el plano 2D de la ventana
        float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.95f;
        float px = (float)winX + size * 0.2f;
        float py = (float)winY + size * 0.10f;

        util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
        glDisable(GL_LIGHTING);



        EstadoPersonaje estadoJinete = EstadoPersonaje::IDLE;
        bool moviendo = false;

        // Si la pieza se está moviendo, cambiamos su estado a ATTACK
        if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
            moviendo = true;
            estadoJinete = EstadoPersonaje::IDLE;
        }

        // Le pedimos a la pieza su ID único en lugar de usar un contador global
        TipoPersonaje tipo = dibujapersonajes::tipoDesdePieza(casilla.pieza, casilla.bando);
        int idAnim = casilla.obj ? casilla.obj->getIdAnimacion() : 0;

        //voltear =true si casilla es del bando musulman
        bool voltear = true;

        // Dibujamos el miliciano usando su ID
        _dibujador.dibujar(tipo, px, py, size,
            EstadoPersonaje::IDLE, idAnim, false, voltear);
        util_salir2D();
        return;
    }
    break;

    case pieza_cubog:
    {
        // Filtramos el local, si cae en cristiano dibuja figura ( de momento):
        if (casilla.bando != bando_rival) {
            glScalef(escala * 0.70f, escala * 0.70f, escala * 0.70f);
            glutSolidTetrahedron();
            glPopMatrix();
            return;
        }

        //codigo dibujo guardia negra

        // Capturamos matrices ANTES de salir del contexto 3D
        GLdouble winX, winY, winZ;

        // Usamos la posición z donde están las piezas
        gluProject(cx, cy, 0.01, model, proj, view, &winX, &winY, &winZ);

        glPopMatrix();

        // Calculamos posición y tamaño en el plano 2D de la ventana
        float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.9f;
        float px = (float)winX - size * 0.3f;
        float py = (float)winY + size * 0.1f;

        util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
        glDisable(GL_LIGHTING);



        EstadoPersonaje estadoGuardia = EstadoPersonaje::IDLE;
        bool moviendo = false;

        // Si la pieza se está moviendo, cambiamos su estado a ATTACK
        if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
            moviendo = true;
            estadoGuardia = EstadoPersonaje::IDLE;
        }

        // Le pedimos a la pieza su ID único en lugar de usar un contador global
        TipoPersonaje tipo = dibujapersonajes::tipoDesdePieza(casilla.pieza, casilla.bando);
        int idAnim = casilla.obj ? casilla.obj->getIdAnimacion() : 0;

        // Dibujamos el miliciano usando su ID
        _dibujador.dibujar(tipo, px, py, size,
            EstadoPersonaje::IDLE, idAnim, false);
        util_salir2D();
        return;
    }
    break;

    case pieza_cono:
    {
        GLUquadric* q = gluNewQuadric();
        glRotatef(90, 1, 0, 0);
        glTranslatef(0, 0, -escala * 0.7f);
        gluCylinder(q, escala * 0.5f, 0.0f, escala * 1.2f, 12, 1);
        gluDisk(q, 0, escala * 0.5f, 12, 1);
        gluDeleteQuadric(q);
    }
    break;

    case pieza_cilindro:
    {
        GLUquadric* q = gluNewQuadric();
        glTranslatef(0, 0, -escala * 0.5f);
        gluCylinder(q, escala * 0.35f, escala * 0.35f, escala * 1.0f, 12, 1);
        gluDisk(q, 0, escala * 0.35f, 12, 1);
        glTranslatef(0, 0, escala * 1.0f);
        gluDisk(q, 0, escala * 0.35f, 12, 1);
        gluDeleteQuadric(q);
    }
    break;


    case pieza_cubo_p:
    {

        //dibujo MILICIANO
        // Solo dibujamos para el bando cristiano
        if (casilla.bando != bando_local) {
            glutSolidCube(escala * 0.75f);
            glPopMatrix();
            return;
        }


        // Capturamos matrices ANTES de salir del contexto 3D
        GLdouble winX, winY, winZ;
        
        // Usamos la posición z donde están las piezas
        gluProject(cx, cy, 0.01, model, proj, view, &winX, &winY, &winZ);

        glPopMatrix();

        // Calculamos posición y tamaño en el plano 2D de la ventana
        float size = (Tablerogl::_anchoVentana * 0.595f / t.N) * 0.95f;
        float px = (float)winX - size * 0.3f;
        float py = (float)winY + size * 0.1f;

        util_entrar2D(Tablerogl::_anchoVentana, Tablerogl::_altoVentana);
        glDisable(GL_LIGHTING);

        

        EstadoPersonaje estadoMiliciano = EstadoPersonaje::IDLE;
        bool moviendo = false;

        // Si la pieza se está moviendo, cambiamos su estado a ATTACK
        if (t._animMov.activa && t._animMov.pieza == casilla.obj) {
            moviendo = true;
            estadoMiliciano = EstadoPersonaje::IDLE;
        }

        // Le pedimos a la pieza su ID único en lugar de usar un contador global
        TipoPersonaje tipo = dibujapersonajes::tipoDesdePieza(casilla.pieza, casilla.bando);
        int idAnim = casilla.obj ? casilla.obj->getIdAnimacion() : 0;

        // Dibujamos el miliciano usando su ID
        _dibujador.dibujar(tipo, px, py, size,
            EstadoPersonaje::IDLE, idAnim, false);
        util_salir2D();
        return;
    }
    break;
    

}
glPopMatrix();

}
// PANTALLA FINAL DE VICTORIA

void DibujaTablero::tablero_victoria(const Tablerogl& t) {
    float cx = (float)t.centro_x;
    float cy = (float)t.centro_y;
    float hw = t.N * t.ancho * 0.45f;
    float hh = t.N * t.ancho * 0.15f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glBegin(GL_QUADS);
    glVertex3f(cx - hw, cy + hh, 0.02f);
    glVertex3f(cx + hw, cy + hh, 0.02f);
    glVertex3f(cx + hw, cy - hh, 0.02f);
    glVertex3f(cx - hw, cy - hh, 0.02f);
    glEnd();
    glDisable(GL_BLEND);

    if (t.victoria_ == bando_local) {
        ETSIDI::setTextColor(1.0f, 0.85f, 0.10f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 36);
        ETSIDI::printxy("VICTORIA CRISTIANA", 300, 400);
    }
    else {
        ETSIDI::setTextColor(0.75f, 0.20f, 0.90f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 36);
        ETSIDI::printxy("VICTORIA AL-ANDALUS", 280, 400);
    }

    ETSIDI::setTextColor(0.80f, 0.80f, 0.80f, 1.0f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 18);
    ETSIDI::printxy("Pulsa ESC para volver al menu", 310, 360);

    glEnable(GL_DEPTH_TEST);
}

//CONTADORES Y MENSAJE INVÁLIDO

void DibujaTablero::tablero_contadores(const Tablerogl& t)
{
    int av = Tablerogl::_altoVentana;
    int aw = Tablerogl::_anchoVentana;

    float tiempo = t.gestorTurnos.getTiempoRestante();
    int   seg = (int)tiempo;
    bool  esLocal = (t.gestorTurnos.getBandoActual() == bando_local);

    // TÍTULO CENTRADO ARRIBA
    ETSIDI::setTextColor(0.85f, 0.70f, 0.30f, 1.0f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 22);
    ETSIDI::printxy("MOROS Y CRISTIANOS", aw / 2 - 130, av - 35);

    // JUGADOR 1 — bando y nombre (izquierda)
    if (esLocal) ETSIDI::setTextColor(0.86f, 0.08f, 0.24f, 1.0f);  // CARMESÍ VIVO SI ES SU TURNO
    else         ETSIDI::setTextColor(0.40f, 0.04f, 0.12f, 1.0f);  // CARMESÍ APAGADO SI ESPERA
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 21);
    ETSIDI::printxy("Bando Cristiano:", 15, av - 28);
    ETSIDI::printxy(t.nombre_j1.c_str(), 15, av - 52);

    // TIEMPO (centro) — debajo del título, bien separado
    char bufTiempo[16];
    if (esLocal) ETSIDI::setTextColor(0.86f, 0.08f, 0.24f, 1.0f);  // CARMESÍ SI ES TURNO LOCAL
    else         ETSIDI::setTextColor(0.45f, 0.0f, 0.60f, 1.0f);   // PÚRPURA SI ES TURNO RIVAL
    sprintf_s(bufTiempo, "%02d s", seg);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 21);
    ETSIDI::printxy(bufTiempo, aw / 2 - 20, av - 62);

    // JUGADOR 2 — bando y nombre (derecha)
    if (!esLocal) ETSIDI::setTextColor(0.45f, 0.0f, 0.60f, 1.0f);  // PÚRPURA VIVO SI ES SU TURNO
    else          ETSIDI::setTextColor(0.20f, 0.0f, 0.28f, 1.0f);  // PÚRPURA APAGADO SI ESPERA
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 21);
    ETSIDI::printxy("Bando Andalus\xED:", aw - 190, av - 28);
    ETSIDI::printxy(t.nombre_j2.c_str(), aw - 190, av - 52);

    // TURNO ACTIVO — centrado debajo del tablero
    std::string turnoTexto = "Turno de " + (esLocal ? t.nombre_j1 : t.nombre_j2);
    if (esLocal) ETSIDI::setTextColor(0.86f, 0.08f, 0.24f, 1.0f);  // CARMESÍ
    else         ETSIDI::setTextColor(0.45f, 0.0f, 0.60f, 1.0f);   // PÚRPURA
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 21);
    ETSIDI::printxy(turnoTexto.c_str(), aw / 2 - 100, 30);
}

void DibujaTablero::tablero_mensaje_invalido(const Tablerogl& t)
{
    if (t._tiempoMensajeInvalido <= 0.0f) return;
    if (t._mensajeInvalido.empty()) return;

    int av = Tablerogl::_altoVentana;
    int aw = Tablerogl::_anchoVentana;

    // Fondo rojo semitransparente centrado en pantalla
    //util_entrar2D(aw, av);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.55f, 0.0f, 0.0f, 0.80f);
    float mx = aw * 0.25f, my = av * 0.48f;
    float mw = aw * 0.50f, mh = av * 0.06f;
    glBegin(GL_QUADS);
    glVertex2f(mx, my);    glVertex2f(mx + mw, my);
    glVertex2f(mx + mw, my + mh); glVertex2f(mx, my + mh);
    glEnd();
    glDisable(GL_BLEND);

    // Texto del cartel
    ETSIDI::setTextColor(1.0f, 0.35f, 0.35f, 1.0f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 22);
    ETSIDI::printxy(t._mensajeInvalido.c_str(),
        (int)(mx + 20), (int)(my + mh * 0.35f));
    util_salir2D();
}

void DibujaTablero::tablero_panel_pieza(const Tablerogl& t) {
    // Solo mostramos el panel si hay pieza seleccionada
    if (!t.piezaSeleccionada || t.fromFila < 0 || t.fromCol < 0) return;

    const Casilla& cas = t.m_tablero->getCasilla(t.fromFila, t.fromCol);
    if (cas.pieza == pieza_nada || cas.obj == nullptr) return;

    Pieza* p = cas.obj;

    int av = Tablerogl::_altoVentana;
    int aw = Tablerogl::_anchoVentana;

    // Tamaño del panel
    const int PW = 195; // ancho del panel en píxeles
    const int PH = 210; // alto del panel
    const int PY = 52;  // separación desde el borde inferior
    const int RADIO_ESQ = 6; // no se usa directamente pero orienta los márgenes

    // Posición X según bando: izquierda para local, derecha para rival
    int px, py;
    if (cas.bando == bando_local) {
        px = 10;               // pegado al borde izquierdo
    }
    else {
        px = aw - PW - 10;     // pegado al borde derecho
    }
    py = PY; // borde inferior

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sombra exterior
    glColor4f(0.0f, 0.0f, 0.0f, 0.60f);
    glBegin(GL_QUADS);
    glVertex2i(px + 6, py - 6);
    glVertex2i(px + PW + 6, py - 6);
    glVertex2i(px + PW + 6, py + PH + 6);
    glVertex2i(px + 6, py + PH + 6);
    glEnd();

    // Fondo de cuero (degradado vertical marron)
    glBegin(GL_QUADS);
    glColor4f(0.42f, 0.22f, 0.09f, 0.98f); // arriba: cuero claro
    glVertex2i(px, py + PH);
    glVertex2i(px + PW, py + PH);
    glColor4f(0.20f, 0.09f, 0.03f, 0.98f); // abajo: cuero oscuro
    glVertex2i(px + PW, py);
    glVertex2i(px, py);
    glEnd();

    glDisable(GL_BLEND);

    // Borde del panel, dorado grueso
    glColor3f(0.80f, 0.60f, 0.16f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(px, py);
    glVertex2i(px + PW, py);
    glVertex2i(px + PW, py + PH);
    glVertex2i(px, py + PH);
    glEnd();

    // Borde interior dorado fino (para que de sensacion de relieve)
    glColor3f(0.95f, 0.80f, 0.35f);
    glLineWidth(1.2f);
    int m = 6;
    glBegin(GL_LINE_LOOP);
    glVertex2f(px + m, py + m);
    glVertex2f(px + PW - m, py + m);
    glVertex2f(px + PW - m, py + PH - m);
    glVertex2f(px + m, py + PH - m);
    glEnd();
    glLineWidth(1.0f);

    // Diseños en esquinas (pequeños cuadrados dorados)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.80f, 0.60f, 0.16f, 1.0f);
    int cs = 7; // corner size
    int off = 3;
    // Esquina inferior-izquierda
    glBegin(GL_QUADS);
    glVertex2i(px + off, py + off);
    glVertex2i(px + off + cs, py + off);
    glVertex2i(px + off + cs, py + off + cs);
    glVertex2i(px + off, py + off + cs);
    glEnd();
    // Esquina inferior-derecha
    glBegin(GL_QUADS);
    glVertex2i(px + PW - off - cs, py + off);
    glVertex2i(px + PW - off, py + off);
    glVertex2i(px + PW - off, py + off + cs);
    glVertex2i(px + PW - off - cs, py + off + cs);
    glEnd();
    // Esquina superior-izquierda
    glBegin(GL_QUADS);
    glVertex2i(px + off, py + PH - off - cs);
    glVertex2i(px + off + cs, py + PH - off - cs);
    glVertex2i(px + off + cs, py + PH - off);
    glVertex2i(px + off, py + PH - off);
    glEnd();
    // Esquina superior-derecha
    glBegin(GL_QUADS);
    glVertex2i(px + PW - off - cs, py + PH - off - cs);
    glVertex2i(px + PW - off, py + PH - off - cs);
    glVertex2i(px + PW - off, py + PH - off);
    glVertex2i(px + PW - off - cs, py + PH - off);
    glEnd();

    glDisable(GL_BLEND);

    // Nombre de la pieza (centrado, fuente grande)
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 18);
    ETSIDI::setTextColor(0.97f, 0.92f, 0.76f, 1.0f);
    // Lo centramos aproximadamente
    int nombreLen = (int)p->getNombre().size();
    int nombreX = px + (PW / 2) - (nombreLen * 5);
    ETSIDI::printxy(p->getNombre().c_str(), nombreX, py + PH - 28);

    // Separador, dorado (debajo del nombre)
    glColor3f(0.75f, 0.56f, 0.14f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2i(px + 12, py + PH - 37);
    glVertex2i(px + PW - 12, py + PH - 37);
    glEnd();
    glLineWidth(1.0f);

    // ── Barras de estadísticas ───────────────────────────────

    // Función lambda local para dibujar un círculo-número de icono
    auto dibujaIcono = [&](int cx, int cy, int num, float r, float g, float b) {
        int radio = 9;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Relleno
        glColor4f(r * 0.35f, g * 0.35f, b * 0.35f, 0.95f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2i(cx, cy);
        for (int i = 0; i <= 20; i++) {
            float a = 2.0f * 3.14159f * i / 20.0f;
            glVertex2f(cx + radio * cosf(a), cy + radio * sinf(a));
        }
        glEnd();
        // Borde
        glColor4f(r, g, b, 1.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 20; i++) {
            float a = 2.0f * 3.14159f * i / 20.0f;
            glVertex2f(cx + radio * cosf(a), cy + radio * sinf(a));
        }
        glEnd();
        glLineWidth(1.0f);
        glDisable(GL_BLEND);
        };

    char buf[64];
    int barX = px + 14;
    int barW = PW - 28;
    int barH = 9;
    int lineH = 43;

    // Vida (verde)
    int yVida = py + PH - 62;
    dibujaIcono(px + 20, yVida + 4, 1, 0.20f, 0.85f, 0.20f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
    ETSIDI::setTextColor(0.70f, 1.0f, 0.70f, 1.0f);
    ETSIDI::printxy("Vida:", barX + 18, yVida + 8);
    sprintf_s(buf, "%d", p->getVida());
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.97f, 0.97f, 0.97f, 1.0f);
    ETSIDI::printxy(buf, px + PW - 28, yVida + 7);
    tablero_barra(barX, yVida - 7, barW, barH, (float)p->getVida(), 100.0f, 0.15f, 0.85f, 0.15f);


    // Fuerza (rojo anaranjado)
    int yFuerza = yVida - lineH;
    dibujaIcono(px + 20, yFuerza + 4, 2, 0.90f, 0.40f, 0.10f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
    ETSIDI::setTextColor(1.0f, 0.62f, 0.30f, 1.0f);
    ETSIDI::printxy("Fuerza:", barX + 18, yFuerza + 8);
    sprintf_s(buf, "%d", p->getFuerza());
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.97f, 0.97f, 0.97f, 1.0f);
    ETSIDI::printxy(buf, px + PW - 28, yFuerza + 7);
    tablero_barra(barX, yFuerza - 7, barW, barH, (float)p->getFuerza(), 100.0f, 0.90f, 0.40f, 0.10f);

    // Velocidad de ataque (azul)
    int yVel = yFuerza - lineH;
    dibujaIcono(px + 20, yVel + 4, 3, 0.25f, 0.55f, 0.95f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
    ETSIDI::setTextColor(0.50f, 0.80f, 1.0f, 1.0f);
    ETSIDI::printxy("Vel.Atk:", barX + 18, yVel + 8);
    sprintf_s(buf, "%d", p->getVelAtaque());
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
    ETSIDI::setTextColor(0.97f, 0.97f, 0.97f, 1.0f);
    ETSIDI::printxy(buf, px + PW - 28, yVel + 7);
    tablero_barra(barX, yVel - 7, barW, barH, (float)p->getVelAtaque(), 100.0f, 0.20f, 0.50f, 0.95f);

    // Radio de movimiento (texto simple, no barra)
    int yRad = yVel - lineH + 10;
    dibujaIcono(px + 20, yRad + 4, 4, 0.82f, 0.63f, 0.15f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
    ETSIDI::setTextColor(0.95f, 0.88f, 0.55f, 1.0f);
    ETSIDI::printxy("Radio mov:", barX + 18, yRad + 8);
    sprintf_s(buf, "%d", p->getRadioMov());
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 15);
    ETSIDI::setTextColor(0.97f, 0.97f, 0.97f, 1.0f);
    ETSIDI::printxy(buf, px + PW - 28, yRad + 6);

    // Pie de carta
    glColor3f(0.75f, 0.56f, 0.14f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2i(px + 12, py + 22);
    glVertex2i(px + PW - 12, py + 22);
    glEnd();

    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 9);
    ETSIDI::setTextColor(0.70f, 0.52f, 0.18f, 1.0f);
    ETSIDI::printxy("LA RECONQUISTA", px + 14, py + 10);
}
void DibujaTablero::tablero_barra(int x, int y, int ancho, int alto,
    float valor, float maximo, float r, float g, float b) {
    float fraccion = (maximo > 0.0f) ? (valor / maximo) : 0.0f;
    if (fraccion < 0.0f) fraccion = 0.0f;
    if (fraccion > 1.0f) fraccion = 1.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Fondo oscuro de la barra (ranura)
    glColor4f(0.05f, 0.03f, 0.01f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + ancho, y);
    glVertex2i(x + ancho, y + alto);
    glVertex2i(x, y + alto);
    glEnd();

    // Relleno de color según el valor
    int relleno = (int)(ancho * fraccion);
    glColor4f(r, g, b, 0.88f);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + relleno, y);
    glVertex2i(x + relleno, y + alto);
    glVertex2i(x, y + alto);
    glEnd();

    // Brillo superior de la barra
    glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2i(x, y + alto / 2);
    glVertex2i(x + relleno, y + alto / 2);
    glVertex2i(x + relleno, y + alto);
    glVertex2i(x, y + alto);
    glEnd();

    // Borde dorado de la barra
    glColor4f(0.70f, 0.52f, 0.14f, 0.80f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + ancho, y);
    glVertex2i(x + ancho, y + alto);
    glVertex2i(x, y + alto);
    glEnd();

    glDisable(GL_BLEND);
}

void DibujaTablero::tablero_panel_hechizos(const Tablerogl& t)
{
    // Solo procedemos si el jugador tiene una pieza seleccionada en el tablero
    if (!t.piezaSeleccionada || t.fromFila < 0 || t.fromCol < 0) return;

    //Comprobamos si la pieza seleccionada es el REY/EMIR 
    const Casilla& cas = t.m_tablero->getCasilla(t.fromFila, t.fromCol);
    if (cas.pieza != pieza_esfera || cas.obj == nullptr) return;

    int av = Tablerogl::_altoVentana;
    int aw = Tablerogl::_anchoVentana;

    // DIMENSIONES DEL PANEL
    const int PW = 195;
    const int PH = 140; // Altura compacta ideal para el título y los 4 hechizos
    const int PY = 275; // Justo encima del panel de stats

    // Posición: Izquierda para locales, Derecha para rivales
    int px = (cas.bando == bando_local) ? 10 : (aw - PW - 10);
    int py = PY;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sombra
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2i(px + 5, py - 5);
    glVertex2i(px + PW + 5, py - 5);
    glVertex2i(px + PW + 5, py + PH + 5);
    glVertex2i(px + 5, py + PH + 5);
    glEnd();

    // Fondo cuero (más oscuro para diferenciarlo del de stats)
    glBegin(GL_QUADS);
    glColor4f(0.36f, 0.18f, 0.07f, 0.98f);
    glVertex2i(px, py + PH);
    glVertex2i(px + PW, py + PH);
    glColor4f(0.18f, 0.08f, 0.02f, 0.98f);
    glVertex2i(px + PW, py);
    glVertex2i(px, py);
    glEnd();

    glDisable(GL_BLEND);

    // Marco dorado exterior
    glColor3f(0.80f, 0.60f, 0.16f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(px, py);
    glVertex2i(px + PW, py);
    glVertex2i(px + PW, py + PH);
    glVertex2i(px, py + PH);
    glEnd();

    // Marco interior fino
    glColor3f(0.95f, 0.80f, 0.35f);
    glLineWidth(1.2f);
    int m = 6;
    glBegin(GL_LINE_LOOP);
    glVertex2i(px + m, py + m);
    glVertex2i(px + PW - m, py + m);
    glVertex2i(px + PW - m, py + PH - m);
    glVertex2i(px + m, py + PH - m);
    glEnd();
    glLineWidth(1.0f);

    // Ornamentos esquinas
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.80f, 0.60f, 0.16f, 1.0f);
    int cs = 7, off = 3;
    glBegin(GL_QUADS); glVertex2i(px + off, py + off); glVertex2i(px + off + cs, py + off); glVertex2i(px + off + cs, py + off + cs); glVertex2i(px + off, py + off + cs); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + PW - off - cs, py + off); glVertex2i(px + PW - off, py + off); glVertex2i(px + PW - off, py + off + cs); glVertex2i(px + PW - off - cs, py + off + cs); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + off, py + PH - off - cs); glVertex2i(px + off + cs, py + PH - off - cs); glVertex2i(px + off + cs, py + PH - off); glVertex2i(px + off, py + PH - off); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + PW - off - cs, py + PH - off - cs); glVertex2i(px + PW - off, py + PH - off - cs); glVertex2i(px + PW - off, py + PH - off); glVertex2i(px + PW - off - cs, py + PH - off); glEnd();
    glDisable(GL_BLEND);

    // TEXTO DEL TÍTULO E INDICACIÓN DE TECLA
    if (t._modoHechizo) {
        // Si el modo hechizo ya se ha pulsado, alertamos visualmente en verde
        ETSIDI::setTextColor(0.30f, 1.0f, 0.30f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("CONJURO ACTIVO", px + 12, PY + PH - 26);
    }
    else {
        // Estado de espera normal
        ETSIDI::setTextColor(0.97f, 0.92f, 0.76f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        if (cas.bando == bando_local) ETSIDI::printxy("[H] Lanza Hechizo:", px + 12, PY + PH - 26);
        else if (cas.bando == bando_rival) ETSIDI::printxy("[J] Lanza Hechizo:", px + 12, PY + PH - 26);
    }

    // LÍNEA SEPARADORA DE CORTE DE PANEL
    glColor3f(0.75f, 0.56f, 0.14f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2i(px + 12, PY + PH - 35);
    glVertex2i(px + PW - 12, PY + PH - 35);
    glEnd();
    glLineWidth(1.0f);

    //LISTADO DE LOS 4 HECHIZOS DISPONIBLES
    int filaY = PY + PH - 54;
    int saltoLinea = 22;

    // Lambda interna para imprimir cada línea iluminándola en amarillo si está seleccionada
    auto printLineaConjuro = [&](const char* texto, bool seleccionado) {
        if (seleccionado) {
            ETSIDI::setTextColor(1.0f, 1.0f, 0.20f, 1.0f); // Amarillo resaltado
            ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
        }
        else {
            ETSIDI::setTextColor(0.88f, 0.82f, 0.68f, 1.0f); // Blanco/Gris suave estático
            ETSIDI::setFont("fuentes/ARIALNBI.ttf", 12);
        }
        ETSIDI::printxy(texto, px + 14, filaY);
        filaY -= saltoLinea;
        };

    // Imprimimos controlando con vuestros flags de 'tablerogl' si se están ejecutando numéricamente
    printLineaConjuro("[1] Avituallamiento", t._modoHechizo && t._conjuroActivo == Conjuro::AVITUALLAMIENTO);
    printLineaConjuro("[2] Rutas Secretas", t._modoHechizo && t._conjuroActivo == Conjuro::RUTAS_SECRETAS);
    printLineaConjuro("[3] Relevo Guardia", t._modoHechizo && t._conjuroActivo == Conjuro::RELEVO_GUARDIA);
    printLineaConjuro("[4] Asedio", t._modoHechizo && t._conjuroActivo == Conjuro::ASEDIO);
}

void DibujaTablero::tablero_panel_infiltrado(const Tablerogl& t)
{
    bool mostrar = false;
    BandoPieza bandoPanel = bando_nada;

    // Se muestra si estamos en el modo, o si acabamos de seleccionar a un Infiltrado
    if (t._modoInfiltrado) {
        mostrar = true;
        bandoPanel = t.gestorTurnos.getBandoActual();
    }
    else if (t.piezaSeleccionada && t.fromFila >= 0 && t.fromCol >= 0) {
        const Casilla& cas = t.m_tablero->getCasilla(t.fromFila, t.fromCol);
        // Magia del polimorfismo: Si el casteo funciona, la pieza es un Infiltrado
        if (cas.obj != nullptr && dynamic_cast<Infiltrado*>(cas.obj) != nullptr) {
            mostrar = true;
            bandoPanel = cas.bando;
        }
    }

    if (!mostrar) return;

    int av = Tablerogl::_altoVentana;
    int aw = Tablerogl::_anchoVentana;

    const int PW = 195;
    const int PH = 120; // Panel más compacto
    const int PY = 430; // Posicionado encima de las stats

    int px = (bandoPanel == bando_local) ? 10 : (aw - PW - 10);
    int py = PY;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sombra
    glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2i(px + 5, py - 5);
    glVertex2i(px + PW + 5, py - 5);
    glVertex2i(px + PW + 5, py + PH + 5);
    glVertex2i(px + 5, py + PH + 5);
    glEnd();

    // Fondo cuero
    glBegin(GL_QUADS);
    glColor4f(0.36f, 0.18f, 0.07f, 0.98f);
    glVertex2i(px, py + PH);
    glVertex2i(px + PW, py + PH);
    glColor4f(0.18f, 0.08f, 0.02f, 0.98f);
    glVertex2i(px + PW, py);
    glVertex2i(px, py);
    glEnd();

    glDisable(GL_BLEND);

    // Marco dorado exterior
    glColor3f(0.80f, 0.60f, 0.16f);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(px, py);
    glVertex2i(px + PW, py);
    glVertex2i(px + PW, py + PH);
    glVertex2i(px, py + PH);
    glEnd();

    // Marco interior fino
    glColor3f(0.95f, 0.80f, 0.35f);
    glLineWidth(1.2f);
    int m = 6;
    glBegin(GL_LINE_LOOP);
    glVertex2i(px + m, py + m);
    glVertex2i(px + PW - m, py + m);
    glVertex2i(px + PW - m, py + PH - m);
    glVertex2i(px + m, py + PH - m);
    glEnd();
    glLineWidth(1.0f);

    // Ornamentos esquinas
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.80f, 0.60f, 0.16f, 1.0f);
    int cs = 7, off = 3;
    glBegin(GL_QUADS); glVertex2i(px + off, py + off); glVertex2i(px + off + cs, py + off); glVertex2i(px + off + cs, py + off + cs); glVertex2i(px + off, py + off + cs); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + PW - off - cs, py + off); glVertex2i(px + PW - off, py + off); glVertex2i(px + PW - off, py + off + cs); glVertex2i(px + PW - off - cs, py + off + cs); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + off, py + PH - off - cs); glVertex2i(px + off + cs, py + PH - off - cs); glVertex2i(px + off + cs, py + PH - off); glVertex2i(px + off, py + PH - off); glEnd();
    glBegin(GL_QUADS); glVertex2i(px + PW - off - cs, py + PH - off - cs); glVertex2i(px + PW - off, py + PH - off - cs); glVertex2i(px + PW - off, py + PH - off); glVertex2i(px + PW - off - cs, py + PH - off); glEnd();
    glDisable(GL_BLEND);

    // TEXTOS Y ESTADOS
    if (t._modoInfiltrado) {
        ETSIDI::setTextColor(0.2f, 1.0f, 0.2f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        ETSIDI::printxy("MODO COPIA ACTIVO", px + 12, PY + PH - 26);
    }
    else {
        ETSIDI::setTextColor(1.0f, 0.95f, 0.80f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
        ETSIDI::printxy("[I] Activar Habilidad:", px + 2, PY + PH - 26);
    }

    // Separador dorado
    glColor3f(0.75f, 0.56f, 0.14f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2i(px + 12, py + PH - 35);
    glVertex2i(px + PW - 12, py + PH - 35);
    glEnd();
    glLineWidth(1.0f);

    // Contenido
    int filaY = PY + PH - 52;
    if (t._modoInfiltrado) {
        ETSIDI::setTextColor(1.0f, 1.0f, 0.20f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 13);
        ETSIDI::printxy("Click en un enemigo", px + 12, filaY);
        ETSIDI::printxy("para robarle sus", px + 12, filaY - 20);
        ETSIDI::printxy("estadisticas.", px + 12, filaY - 40);
    }
    else {
        ETSIDI::setTextColor(0.88f, 0.82f, 0.68f, 1.0f);
        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 12);
        ETSIDI::printxy("Permite robar los", px + 12, filaY);
        ETSIDI::printxy("stats de una pieza", px + 12, filaY - 20);
        ETSIDI::printxy("del bando rival.", px + 12, filaY - 40);
    }
}

void DibujaTablero::tablero_guardando(Tablerogl& t, int ancho, int alto, float tiempoRestante)
{
    tablero_dibujar(t);
    util_entrar2D(ancho, alto);

    // FONDO OSCURO
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.72f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(ancho, 0);
    glVertex2f(ancho, alto); glVertex2f(0, alto);
    glEnd();
    glDisable(GL_BLEND);

    // PANEL CENTRAL
    float pw = 380, ph = 320;
    float px = (ancho - pw) / 2.0f;
    float py = (alto - ph) / 2.0f;

    // FONDO PANEL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.12f, 0.04f, 0.20f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2f(px, py); glVertex2f(px + pw, py);
    glVertex2f(px + pw, py + ph); glVertex2f(px, py + ph);
    glEnd();

    // BORDE DORADO
    glColor4f(0.85f, 0.70f, 0.25f, 1.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(px, py); glVertex2f(px + pw, py);
    glVertex2f(px + pw, py + ph); glVertex2f(px, py + ph);
    glEnd();
    glLineWidth(1.0f);
    glDisable(GL_BLEND);

    // TÍTULO
    ETSIDI::setTextColor(0.85f, 0.70f, 0.25f, 1.0f);
    ETSIDI::setFont("fuentes/ARIALNBI.ttf", 32);
    ETSIDI::printxy("PAUSA", (int)(px + pw / 2 - 60), (int)(py + ph - 45));

    // SEPARADOR
    glColor3f(0.85f, 0.70f, 0.25f);
    glBegin(GL_LINES);
    glVertex2f(px + 20, py + ph - 58);
    glVertex2f(px + pw - 20, py + ph - 58);
    glEnd();

    // OPCIONES
    const char* opciones[4] = {
        "Continuar",
        "Guardar partida",
        "Ayuda",
        "Salir al menu"
    };

    float btnW = 300, btnH = 44;
    float btnX = px + (pw - btnW) / 2.0f;
    float btnYBase = py + ph - 110;
    float btnSep = 56;

    for (int i = 0; i < 4; i++) {
        float by = btnYBase - i * btnSep;
        bool seleccionado = (t._pausaSeleccion == i); // RESALTADO

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (seleccionado)
            glColor4f(0.55f, 0.15f, 0.80f, 0.95f); // MORADO VIVO SI SELECCIONADO
        else
            glColor4f(0.30f, 0.10f, 0.50f, 0.90f); // MORADO OSCURO

        glBegin(GL_QUADS);
        glVertex2f(btnX, by); glVertex2f(btnX + btnW, by);
        glVertex2f(btnX + btnW, by + btnH); glVertex2f(btnX, by + btnH);
        glEnd();

        // BORDE
        if (seleccionado)
            glColor4f(0.85f, 0.70f, 0.25f, 1.0f); // DORADO SI SELECCIONADO
        else
            glColor4f(0.65f, 0.20f, 0.85f, 1.0f); // MORADO

        glLineWidth(seleccionado ? 2.5f : 1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(btnX, by); glVertex2f(btnX + btnW, by);
        glVertex2f(btnX + btnW, by + btnH); glVertex2f(btnX, by + btnH);
        glEnd();
        glLineWidth(1.0f);
        glDisable(GL_BLEND);

        // TEXTO CENTRADO
        if (seleccionado)
            ETSIDI::setTextColor(1.0f, 1.0f, 0.0f, 1.0f); // AMARILLO SI SELECCIONADO
        else
            ETSIDI::setTextColor(1.0f, 1.0f, 1.0f, 1.0f); // BLANCO

        ETSIDI::setFont("fuentes/ARIALNBI.ttf", 14);
        int texW = strlen(opciones[i]) * 7;
        // LONGITUDES APROXIMADAS DE CADA OPCIÓN EN PÍXELES
        int offsets[4] = { 40, 63, 30, 50 }; // CONTINUAR, GUARDAR PARTIDA, AYUDA, SALIR AL MENU
        ETSIDI::printxy(opciones[i], (int)(btnX + btnW / 2 - offsets[i]), (int)(by + btnH * 0.35f));
    }

    util_salir2D();
}