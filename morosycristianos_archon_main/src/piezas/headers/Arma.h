#pragma once
#include<string>

//crear tipo de dato con los 3 tipos de ataques
enum class TipoAtaque {
        CUERPO_A_CUERPO,
        DISTANCIA,
        CARGA
    };


class Arma
{
protected:
    std::string nombreArma;  //defino cadena de caracteres para el nombreArma
    TipoAtaque tipoAtaque;  //defino tipoataque segun el tipo de datos creado arriba

public:
    // Constructor para inicializar variables comunes a todas las armas
    Arma(std::string nombreArma, TipoAtaque tipoAtaque);

    // Destructor virtual: OBLIGATORIO al usar polimorfismo
    virtual ~Arma() {}

    // Métodos comunes (getters)
    std::string getNombreArma() const { return nombreArma; }
    TipoAtaque getTipoAtaque() const { return tipoAtaque; }

    // MÉTODO VIRTUAL PURO (=0): Cada arma ataca distinto en la arena
    virtual void realizarAtaque() const = 0;
};