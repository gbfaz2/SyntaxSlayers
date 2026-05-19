#pragma once
#include <string>

//FALTAN LAS ARMAS

//clase "madre" de la que van a nacer las clases "utiles"
// Tipos de movimiento posible
enum class TipoMovimiento {
    TERRESTRE,
    VOLADOR,
    TELEPORTE
};

// Bandos del juego
enum class Bando {
    CRISTIANO,
    ANDALUSI
};

class Pieza {
protected: //solo pueden acceder clase Y (sus hijas)
    // Identidad de cada pieza (nombre y equipo)
    std::string    nombre;
    Bando          bando;

    // Stats de combate (valores 0-100 según la tabla del documento)
    int            vida;
    int            vidaMax;
    int            fuerza;
    int            velAtaque;
    int            recarga;
    int            radioMov;

    // Posición en el tablero
    int            fila;
    int            columna;

    // Estado
    bool           viva;  
    bool           enPuntoDePoder;  //para casillas especiales con curaciones etc especiales

public:
    // Constructor con las carcaterítsicas necesarias para las piezas para inicializar
    Pieza(std::string nombre, Bando bando,
        int vidaMax, int fuerza,
        int velAtaque, int recarga, int radioMov);

    // Destructor virtual: OBLIGATORIO en clases base con herencia
    virtual ~Pieza() {}

    //MÉTODOS:
    // Métodos virtuales puros(=0): cambian para cada pieza (los implementa a su manera)
    virtual TipoMovimiento getTipoMovimiento() const = 0; //devuelve terrestre, voladora, teleport...
    virtual bool puedeMoverse(int filaDestino, int colDestino) const = 0;  //comprueba si una ficha puede moverse o no a donde desea, y devuelve 1 o 0

    // Métodos comunes (todos se comportan igual)
    void  recibirDanio(int danio);  //resta vida cuando la pieza es atacada en combate
    void  curar(int cantidad);  //suma vida con casillas especiales o cuando con conjuros de curación por el líder
    bool  estaViva()   const { return viva; }  //devueve si la pieza sigue jugando o ha sido eliminada
    Bando getBando()   const { return bando; }  //devuelve de que bando es la pieza, para saber si al enfrentarse dos piezas, de que bando es cada una

    std::string getNombre() const { return nombre; }  //devuelve el nombre de la pieza para mostrarlo por pantalla
    int  getFila()     const { return fila; }  //devuelve la fila para calcular posibles movimientos
    int  getColumna()  const { return columna; }  //devuelve la columna para calcular posibles movimientos
    void setPosicion(int f, int c) { fila = f; columna = c; }  //devuelve la posicion actual en el tablero
    int getFuerza()    const { return fuerza; } //devuelve la fuerza
    int getVelAtaque() const { return velAtaque; }  //devuelve la velocidad
    int getRecarga()   const { return recarga; }  //devuelve la recarga
    int getRadioMov() const { return radioMov; }//devuelve el radio de movimiento de cada pieza

    int getVida()    const { return vida; }
    int getVidaMax() const { return vidaMax; }

    // Para la demo: muestra las stats por consola
    virtual void mostrarStats() const;  //esto es solo para la demo aunque quedaría bien, mostrar las stats por pantalla

    void setVida(int v) { vida = (v < vidaMax) ? v : vidaMax; }
};