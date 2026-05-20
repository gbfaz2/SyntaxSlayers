#include "GestorMovimiento.h"
#include <cmath>
#include <iostream>

//

// MÉTODO PRINCIPAL
// Es el único que llama María desde tablerogl
// 1. Primero deja que la pieza valide geometría (radio, diagonal, límites)
// 2. Luego según el tipo de movimiento, escanea el tablero de forma distinta

ResultadoMovimiento GestorMovimiento::resolverMovimiento(
    Pieza* pieza,//usamos puntero * por el polimorfismo
    Tablero& tablero,//copia directa con &
    int toFila, int toCol) //destino
{
    //valida la gemoetria: diagonal , limites tablero, radio...
    // funcion puede moverse() valida todo eso para cada tipo de pieza
    // puedeMoverse() ya está implementado en PiezaTerrestre, PiezaVoladora y PiezaTeleporte
    if (!pieza->puedeMoverse(toFila, toCol))
        return ResultadoMovimiento::MOVIMIENTO_INVALIDO;

    //función getTipoMovimiento() definida en Pieza.h como virtual pura (=0)
    // Escanea el tipo de pieza, para cada tipo de pieza aplicará un método u otro
    switch (pieza->getTipoMovimiento()) {

        //terrestre de la clase de tipoMovimiento(resultado de gettipomovimiento())
        //si es pieza terrestre:
    case TipoMovimiento::TERRESTRE:
        // escanea casilla a casilla buscando aliados y enemigos en el camino
        return escanearCaminoTerrestre(pieza, tablero, toFila, toCol);

        //para volador / teleport, solo comprueban destino 
    case TipoMovimiento::VOLADOR:
    case TipoMovimiento::TELEPORTE:
        // saltan todo el camino, solo miran el destino
       
    _ultimoResultado = comprobarDestino(pieza, tablero, toFila, toCol); 
    return _ultimoResultado;

     }

    _ultimoResultado = ResultadoMovimiento::MOVIMIENTO_INVALIDO;
    return _ultimoResultado; // por seguridad
   
}


// ESCANEAR CAMINO TERRESTRE
// Recorre casilla a casilla desde la posición actual de la pieza hasta el destino
// Si encuentra aliado en cualquier punto (camino o destino) → BLOQUEADO
// Si encuentra enemigo en el camino → COMBATE, se para ahí
// Si encuentra enemigo en el destino → COMBATE
// Si todo libre → MOVIMIENTO_OK

ResultadoMovimiento GestorMovimiento::escanearCaminoTerrestre(
    Pieza* pieza,  
    Tablero& tablero,  
    int toFila, int toCol)
{
    // Calculamos el paso a dar en cada dirección (1, -1 o 0)
    //Calcula la posicion actual de la pieza
    int fromFila = pieza->getFila();
    int fromCol = pieza->getColumna();

    //Calcula en que direccion se mueve la fila
    int dirFila = 0, dirCol = 0;  //inicializa variables de dirección
    if (toFila > fromFila) dirFila = 1;  // baja
    if (toFila < fromFila) dirFila = -1;  // sube
    if (toCol > fromCol)  dirCol = 1;  // derecha
    if (toCol < fromCol)  dirCol = -1;  // izquierda


    // Bando de la pieza que se mueve, para comparar con lo que encontremos
    BandoPieza bandoOrigen = tablero.getCasilla(fromFila, fromCol).bando;

    //avanzamos a la siguiente casilla 
    // Empezamos a escanear desde la siguiente casilla al origen
    int fila = fromFila + dirFila;
    int col = fromCol + dirCol;

    // Recorremos casilla a casilla hasta llegar al destino
    //siempre el movimiento va a ser en 1 sola direccion por lo que podemos plantear
    //el bucle con una condicion AND:
    //sigue hasta que las dos lleguen a su destino
    while (fila != toFila || col != toCol) {

        // ¿Hay algo en esta casilla intermedia?
        if (tablero.getCasilla(fila, col).pieza != pieza_nada) {

            //si hay algo (distinto de nada) entonces:
            //bando de la pieza en la casilla de escaneo
            BandoPieza bandoCasilla = tablero.getCasilla(fila, col).bando;

            //si es aliado:
            if (bandoCasilla == bandoOrigen) {
                // Aliado en el camino → bloqueado, no puede pasar
                //informa por pantalla
                std::cout << "[GestorMovimiento] Camino bloqueado por aliado en ("
                    << fila << "," << col << ")" << std::endl;
                
                //NO SE MUEVE
                //retorna bloqueado_aliado como resultado del movimiento
                return ResultadoMovimiento::BLOQUEADO_ALIADO;
            }

            //si es enemigo: se abre arena 
            else {
                // Enemigo en el camino → la terrestre se para aquí y combate
                //informa por pantalla
                std::cout << "[GestorMovimiento] Enemigo en el camino en ("
                    << fila << "," << col << ") — COMBATE!" << std::endl;
              
                // NO MOVEMOS TODAVIA, igual que en comprobarDestino
                _ultimoAtacante = pieza;
                _ultimaDefensora = tablero.getCasilla(fila, col).obj;
                _filaAtacante = fromFila;
                _colAtacante = fromCol;
                _filaDefensora = fila;
                _colDefensora = col;


                //retorna combate, como resultado del movimiento
                _ultimoAtacante = pieza;
                _ultimaDefensora = tablero.getCasilla(fila, col).obj; // ya está en destino
               
                //retorna combate, como resultado del movimiento
                return ResultadoMovimiento::COMBATE;
            }
        }
        
        // Casilla libre, seguimos avanzando y repitiendo el bucle hasta destino
        fila += dirFila;
        col += dirCol;
    }
    //salimos del bucle->hemos llegado al destino
    //comprobamos qué hay ahí con el otro método

    return comprobarDestino(pieza, tablero, toFila, toCol);
}

// COMPROBAR DESTINO
// Solo mira la casilla destino, sin recorrer el camino
// La usan voladoras y teleporte directamente

ResultadoMovimiento GestorMovimiento::comprobarDestino(
    Pieza* pieza,
    Tablero& tablero,
    int toFila, int toCol)
{
    //escanea la casilla actual de la pieza
    int fromFila = pieza->getFila();
    int fromCol = pieza->getColumna();

    //escanea el bando de la pieza
    BandoPieza bandoOrigen = tablero.getCasilla(fromFila, fromCol).bando; //escanea bando de la pieza origen
   
    //escanea el tipo de pieza en el destino y su bando
    TipoPieza piezaDst = tablero.getCasilla(toFila, toCol).pieza;
    BandoPieza     bandoDst = tablero.getCasilla(toFila, toCol).bando; //escanea bando de la pieza de destino

    
   //no hay pieza en el destino, puede moverse
    if (piezaDst == pieza_nada) {
   
        tablero.muevePieza(fromFila, fromCol, toFila, toCol);
        //actualiza la posicion de la pieza
        pieza->setPosicion(toFila, toCol);
        return ResultadoMovimiento::MOVIMIENTO_OK;
    }

    //pieza aliado, movimiento bloqueado
    if (bandoDst == bandoOrigen) {
        // Aliado en el destino → bloqueado
        std::cout << "[GestorMovimiento] Destino bloqueado por aliado en ("
            << toFila << "," << toCol << ")" << std::endl;
        return ResultadoMovimiento::BLOQUEADO_ALIADO;
    }

    // Enemigo en el destino, combate
    // Enemigo en el destino, combate
    std::cout << "[GestorMovimiento] Enemigo en destino ("
        << toFila << "," << toCol << ") — COMBATE!" << std::endl;

    // NO MOVEMOS TODAVIA, SOLO GUARDAMOS LAS PIEZAS
    _ultimoAtacante = pieza;
    _ultimaDefensora = tablero.getCasilla(toFila, toCol).obj;
    _filaAtacante = fromFila;  // GUARDAMOS POSICIONES PARA LUEGO
    _colAtacante = fromCol;
    _filaDefensora = toFila;
    _colDefensora = toCol;

    return ResultadoMovimiento::COMBATE;
}