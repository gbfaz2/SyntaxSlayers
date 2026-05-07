# ⚔️ ARCHON WARFARE: Moros y Cristianos

> Una variación del clásico juego de estrategia y acción **ARCHON (1983)**, ambientada en las guerras entre los Reinos Cristianos y Al-Ándalus durante la Edad Media peninsular.

**Asignatura:** Informática Industrial — Curso 2025-2026  
**Equipo:** SyntaxSlayers  
**Tecnología:** C++ · OpenGL / GLUT · ETSIDIlib · STL  

---

## 👥 Equipo

| Nombre | GitHub | Rol Principal |
|---|---|---|
| María Heredero Diezma | — | Gráficos, escenarios e interfaz visual |
| Alba De la Cruz Conde | — | Estructura POO y gestión de datos |
| Sofía Jara Quiñones | — | Lógica de juego, turnos y habilidades |
| Gabriela Fernández Álvarez | — | Inteligencia artificial y combate en arena |
| Inés Alcérreca Sánchez | — | Animaciones, input e interactividad |

---

## 🎮 Descripción del Juego

**Moros y Cristianos** es un juego de **estrategia y acción en tiempo real** desarrollado íntegramente en C++. Combina una fase táctica sobre un tablero 9×9 (el *Mapa Fronterizo Peninsular*) con una fase de combate 1vs1 en una arena a pantalla completa (la *Escaramuza*), fiel a la mecánica del juego original ARCHON.

Dos bandos se enfrentan por el control de la Península Ibérica:

- ✝️ **Bando Cristiano** — Los Reinos del Norte
- 🌙 **Bando Andalusí** — Al-Ándalus

---

## 🗺️ Escenarios Históricos

Al iniciar la partida, el sistema elige aleatoriamente uno de cuatro escenarios históricos reales. Cada uno cambia el aspecto del tablero y determina qué bando tiene la iniciativa del primer turno:

| Escenario | Año | Victoria Histórica | Primer Turno |
|---|---|---|---|
| 🌙 Batalla de Guadalete | 711 | Andalusí | Bando Musulmán |
| 🌙 Batalla de Alarcos | 1195 | Andalusí | Bando Musulmán |
| ✝️ Las Navas de Tolosa | 1212 | Cristiana | Bando Cristiano |
| ✝️ Reconquista de Granada | 1492 | Cristiana | Bando Cristiano |

---

## ⚙️ Mecánica de Juego

### Condiciones de Victoria
Un jugador gana si cumple **una** de estas condiciones:
1. **Dominio total:** Controlar los 5 *Bastiones Históricos* del tablero.
2. **Victoria militar:** Aniquilar todas las tropas enemigas.
3. **Asedio final:** Dejar al rival con una única unidad acorralada en las mazmorras.

### Secuencia de Partida
1. **Menú Principal** — Selección de modo de juego.
2. **Destino Histórico (RNG)** — El sistema elige escenario y asigna el turno inicial.
3. **Fase de Estrategia (Tablero 9×9)** — Los jugadores se alternan moviendo unidades o gastando *Órdenes Reales*.
4. **Fase de Acción (La Arena)** — Si dos unidades colisionan, se abre una escaramuza 1vs1 en tiempo real.
5. **Resolución** — El perdedor es eliminado; el ganador ocupa la casilla.
6. **Bucle** — Se repite hasta cumplir una condición de victoria.

---

## 🏰 El Tablero

| Elemento Original (Archon) | Adaptación |
|---|---|
| Tablero 9×9 | Mapa Fronterizo Peninsular 9×9 |
| 5 Puntos de Poder | 5 Bastiones Históricos (Castillos y Alcazabas) |
| Curación rápida e inmunidad | Refugio Seguro — la tropa descansa e es inmune a Órdenes Reales rivales |
| Casillas oscilantes (claro ↔ oscuro) | Territorios de Frontera — zonas de influencia cambiante entre la Cruz y la Media Luna |
| Casilla de color propio = ventaja | Dominio del Terreno — buff de moral en la arena 1vs1 |

---

## ⚔️ Tropas

Cada bando comienza con **18 tropas** divididas en **8 clases militares**:

| Equivalencia Archon | Bando Cristiano ✝️ | Bando Andalusí 🌙 | Ataque | Movimiento |
|---|---|---|---|---|
| Líder | Rey Cristiano | Emir / Califa | Espadón / Cimitarra (cuerpo a cuerpo) | Tierra |
| Vuelo 1 (Valkyrie) | Caballería Ligera | Jinete Bereber | Carga rápida con lanza | Flanqueo (Vuelo) |
| Vuelo 2 (Phoenix) | Almogávar | Arquero a Caballo | Jabalinas incendiarias / Flechas | Flanqueo (Vuelo) |
| Vuelo 3 (Djinni) | Infiltrado / Espía | Asesino de Élite | Cuerpo a cuerpo letal (emboscada) | Infiltración (Teleport) |
| Fuerte 1 (Golem) | Infantería Pesada | Guardia Negra | Maza / Hacha — lento pero brutal | Tierra |
| Fuerte 2 (Unicorn) | Caballería Pesada | Caballería Acorazada | Carga lineal / Proyectil de impacto | Tierra |
| Básica 1 (Knight) | Peón / Miliciano | Soldado Ghazí | Espada corta / Daga (básico) | Tierra |
| Básica 2 (Archer) | Ballestero | Arquero | Ballesta (lenta, letal) / Arco (rápido, menor daño) | Tierra |

### Estadísticas de Combate (0–100)

| Tropa | HP | Daño | Vel. Ataque | Recarga | Radio Mov. |
|---|---|---|---|---|---|
| Líder (Rey/Emir) | 80 | 75 | 60 | 40 | 3 |
| Caballería Ligera | 50 | 60 | 90 | 30 | 5 |
| Almogávar / Arq. Caballo | 60 | 85 | 50 | 70 | 4 |
| Infiltrado / Asesino | 40 | 90 | 100 | 20 | 5 |
| Infantería / Guardia | 100 | 95 | 20 | 90 | 2 |
| Caballería Pesada | 90 | 80 | 40 | 60 | 3 |
| Peón / Ghazí | 30 | 40 | 70 | 30 | 3 |
| Ballestero / Arquero | 20 | 60 | 80 | 50 | 3 |

### Poderes Únicos

| Tropa | Poder Especial |
|---|---|
| Almogávar / Arquero a Caballo | **Fuego Griego:** ataque de área que quema a enemigos cercanos y bloquea daño recibido brevemente. |
| Infiltrado / Asesino de Élite | **Espionaje Táctico:** copia literalmente las stats y el arma del enemigo en la arena. |

---

## 🔮 Órdenes Reales (Hechizos)

El Líder de cada bando puede ejecutar **7 Órdenes Reales**, cada una utilizable **una sola vez** por partida:

| Hechizo Original | Orden Histórica | Efecto |
|---|---|---|
| Teleport | Rutas Secretas | Mueve una tropa aliada a cualquier casilla libre. |
| Heal | Avituallamiento | Cura la barra de vida de una unidad al máximo. |
| Shift Time | Control de Fronteras | Altera el ciclo de oscilación de las casillas. |
| Exchange | Relevo de Guardia | Intercambia la posición de dos unidades propias. |
| Summon Elemental | Llamar Mercenarios | Invoca una tropa mercenaria temporal. |
| Revive | Llegada de Refuerzos | Resucita una tropa caída junto al Rey/Emir. |
| Imprison | Sitiar / Asediar | Bloquea a una tropa enemiga en su casilla. |

---

## 🤖 Inteligencia Artificial

La IA se implementa mediante un algoritmo **Minimax con poda alfa-beta** desarrollado en C++ puro, sin librerías externas. Incluye:
- Modo **Jugador vs. IA** con distintos estilos de juego.
- **Sugerencia de movimientos** para asistir al jugador humano.
- **Replay animado** de partidas ya realizadas.

---

## 🛠️ Tecnologías y Librerías

| Librería | Uso |
|---|---|
| **OpenGL / GLUT** | Representación gráfica del tablero, piezas e interacción básica |
| **ETSIDIlib** | Gestión de texturas, sonidos y fuentes (proporcionada por el profesor) |
| **STL (C++)** | Estructuras de datos: vectores, matrices, lógica del juego |

> **Nota:** No se utilizan frameworks completos de juego (Unity, Unreal, etc.), en cumplimiento de la normativa de la asignatura.

---

## 🚀 Instalación y Ejecución

1. Clona el repositorio:
   ```bash
   git clone https://github.com/<org>/<repo>.git
   ```
2. Abre el proyecto con **Visual Studio 2026**.
3. Asegúrate de tener las dependencias instaladas (OpenGL/GLUT y ETSIDIlib — ver instrucciones en la memoria del proyecto).
4. Compila y ejecuta en modo Release.

> Para la ejecución directa sin compilar, descarga el zip ejecutable desde el enlace incluido en la entrega de Moodle. Descomprímelo en cualquier carpeta de un equipo con Windows y Visual Studio 2026 instalado.

---

## 📁 Estructura del Repositorio

```
/
├── src/                  # Código fuente C++
│   ├── core/             # Lógica del juego (tablero, turnos, victoria)
│   ├── entities/         # Jerarquía de clases de tropas (POO)
│   ├── ai/               # Motor Minimax y sugerencias
│   ├── graphics/         # Renderizado OpenGL / GLUT
│   ├── audio/            # Gestión de sonidos (ETSIDIlib)
│   └── input/            # Input manager (ratón + teclado)
├── assets/               # Texturas, sprites y recursos de audio
├── docs/                 # Memoria del proyecto (PDF)
└── README.md
```

---

## 📋 Requisitos Mínimos Implementados

- [x] Doble escenario: tablero 9×9 + arena de combate a pantalla completa
- [x] ≥ 5 tipos de fichas por bando con reglas y stats propias
- [x] Dos agentes: Jugador vs. Jugador y Jugador vs. IA
- [x] Representación gráfica 2D/3D con interactividad por ratón y teclado
- [x] Órdenes Reales (equivalentes a los conjuros del original)
- [x] Detección de las 3 condiciones de fin de partida
- [x] Estructura Orientada a Objetos con jerarquía de clases (herencia, polimorfismo)
- [x] Temática original: Guerras de Al-Ándalus

## ✨ Requisitos Opcionales Implementados

- [x] Inteligencia Artificial con Minimax + poda alfa-beta
- [x] Sugerencia de movimientos al jugador
- [x] Menú inicial y pantalla final de juego
- [x] Gestión de rankings persistente (fichero)
- [x] Guardado y carga de partida
- [x] Escenarios históricos con variabilidad visual del tablero
- [x] Sonidos ambientales y circunstanciales
- [x] Dibujo traslúcido de casillas válidas como ayuda al jugador
- [x] Cambios de vista 2D/3D según fase de juego
- [x] Animaciones continuas de movimiento (tierra, vuelo, teletransporte)
- [x] Efectos especiales al moverse, ganar y perder
- [x] Interactividad completa por ratón (selección y movimiento de piezas)
- [x] Temporizadores y puntuación
- [x] Reproducción animada de partidas ya realizadas

---

*Trabajo realizado para la asignatura de Informática Industrial — ETSIDI, UPM. Curso 2025-2026.*
| **IA** | 🔴 Inteligencia que sugiera movimientos | [Por definir] |
| **Red** | 🔴 Juego por red local | [Por definir] |
