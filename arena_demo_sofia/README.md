# Arena de combate - Archon Warfare

Esta carpeta es mi parte del trabajo de Informática Industrial 2025-2026 (grupo SyntaxSlayers). El juego sobre el que estamos trabajando es una versión libre del Archon ambientada en la Reconquista: Cristianos vs Andalusíes.

## Mi parte:

Dentro del reparto del equipo, mi parte es la **lógica de juego, los turnos y las habilidades**. En concreto:

- Motor de turnos
- Sistema de combate en la arena (detección de impacto, vida, fin de combate)
- Detección de las 3 condiciones de victoria
- Habilidades / conjuros del Líder
- Temporizadores y puntuación
- Highlight de casillas válidas
- Audio (SFX)

Esta demo se centra en la **arena de combate** primer avance para la tutoría del 13 de mayo. Por ahora son dos cubos peleando, pero ya está toda la mecánica funcionando: movimiento en 3D, ataque cuerpo a cuerpo, barras de vida y condición de victoria.

## Cómo está hecho

- C++ con Visual Studio 2026
- OpenGL + freeglut para los gráficos 3D
- Cámara aérea-oblicua, estilo Archon Ultra
- Estructura orientada a objetos con la lógica desacoplada del motor gráfico (como pide el enunciado)

### Organización de archivos

- `Combatiente.h/.cpp` - una unidad de combate (vida, posición, ataque)
- `InputState.h` - capa de abstracción para teclado/mando
- `Arena.h/.cpp` - escenario y lógica del combate (sin OpenGL)
- `ArenaRenderer.h/.cpp` - todo el dibujo 3D va aquí
- `main.cpp` - bucle principal de GLUT

## Controles

| Acción      | Jugador 1 (Cristiano) | Jugador 2 (Andalusí) |
|-------------|-----------------------|----------------------|
| Mover       | W A S D               | flechas              |
| Atacar      | F                     | L                    |
| Reiniciar   | R                     | R                    |
| Salir       | ESC                   | ESC                  |

## Pendiente

- [ ] Soporte de mando de PlayStation (la idea es que cada jugador tenga un mando ya que es más cómodo que chocar las manos jugando con teclas)
- [ ] Animaciones del ataque (no solo el flash blanco)
- [ ] Hechizos / órdenes reales del Líder
- [ ] Integración con la fase de tablero (cuando Alba tenga el esqueleto listo)
- [ ] Sonidos
