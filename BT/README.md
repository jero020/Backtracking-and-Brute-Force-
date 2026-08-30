# Módulo de Backtracking (BT)

Prototipo del módulo de Backtracking para compartir e integrar en el repositorio general del equipo.

## Contenido

- `src/main.cpp`: Algoritmo de backtracking incremental con función de factibilidad, conteo de nodos visitados y podados.
- `CMakeLists.txt`: Configuración de compilación en C++17.
- `CONVERSACIONES_IA.md`: Registro de soporte y consultas de diseño/depuración realizadas con IA.

## Funcionalidades
-Evaluación de reglas de complejidad (mayúsculas, minúsculas, dígitos, símbolos y caracteres repetidos).
-Poda del árbol de búsqueda mediante función de factibilidad para optimizar la exploración.
-Conteo de nodos visitados, nodos podados y cantidad total de soluciones encontradas.
-Almacenamiento e impresión de una solución de ejemplo.
-Medición precisa del tiempo de ejecución.

## Compilación y ejecución

### Desde la carpeta `BT`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src/main.cpp -o backtracking
./backtracking

Tambien puede compilarse mediante el archivo `CMakeLists.txt` incluido.