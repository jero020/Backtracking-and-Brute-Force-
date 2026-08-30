# Backtracking-and-Brute-Force-

Análisis de diferencias entre Backtracking y Brute force a la hora de descifrar una contraseña.

## Proyecto

Este repositorio compara dos estrategias para intentar descifrar una contraseña:

- Backtracking: búsqueda inteligente con poda y restricciones.
- Brute force: enumeración exhaustiva combinando un alfabeto y una longitud dadas.

La estructura del proyecto está organizada en carpetas para cada módulo:

- `BT/`: implementación basada en Backtracking.
- `FB/`: implementación basada en fuerza bruta.

## Módulo de Fuerza Bruta

Implementación en C++17 de enumeración exhaustiva para un alfabeto y una longitud configurables.

### Funcionalidades

- Comparación directa con una contraseña sintética, usada como demostración.
- Comparación requerida mediante un hash SHA-256 hexadecimal.
- Conteo de candidatos evaluados.
- Medición del tiempo de ejecución.
- Validación del alfabeto, la longitud y el formato del hash.

### Compilación

Desde la carpeta `FB`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o fuerza_bruta
```

También puede compilarse mediante el archivo `CMakeLists.txt` incluido.

### Prueba de referencia del módulo

Para probar la búsqueda de `B7a3` con el alfabeto `0123456789ABCabc`, longitud 4 y la opción SHA-256, use:

```text
45e30cb7de804478b59781eafd535197fb0ab0e5450110beaf1c1cd3121a6d66
```

La búsqueda encuentra la contraseña en el intento 47060.

### Dependencia

El cálculo de SHA-256 usa `third_party/picosha2.h`, biblioteca de cabecera distribuida bajo licencia MIT. La licencia se conserva en `third_party/LICENSE-PicoSHA2.txt`.

### Trabajo pendiente del equipo

La comparación por diccionario, las pruebas automatizadas y la experimentación general deben integrarse con el resto del proyecto.

## Módulo de Backtracking

Implementación en C++17 de búsqueda con backtracking y poda por
factibilidad, sobre la política de composición de la Sección 9.2 del
enunciado.

> **Aviso:** este `main.cpp` es un plan de contingencia escrito con
> asistencia de IA (Claude) por Jerónimo Vélez Acosta, mientras Camila
> García Ortiz (responsable del módulo BT) terminaba su propia versión. Si
> ella entrega la suya, esa es la que debe integrarse y presentarse como el
> módulo BT del equipo. Si no llega a tiempo, este es el código que se
> entregaría en su lugar, y quien lo presente en la sustentación oral debe
> poder explicarlo línea por línea (ver `BT/README.md` para el detalle
> completo).

### Funcionalidades

- Cálculo de la semilla del equipo y derivación de la política de
  composición (mínimos de minúsculas, mayúsculas, dígitos y símbolos, más
  la restricción de no repetidos consecutivos).
- Backtracking con poda por factibilidad y versión sin poda, para la
  comparación de correctitud que exige la Sección 8.2.
- Menú interactivo: validar la instancia de referencia, calcular semilla y
  política, correr las 5 variantes de la Sección 9.2, y comparar con/sin
  poda sobre un alfabeto reducido.

### Compilación

Desde la carpeta `BT`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o bt
```

También puede compilarse mediante el archivo `CMakeLists.txt` incluido.

### Trabajo pendiente del equipo

Confirmar con el docente si el alfabeto son 67 o 69 símbolos, correr hasta
el final la instancia de referencia y las 5 variantes con tiempo
suficiente, generar las gráficas, y redactar el análisis de complejidad
para el informe técnico. Ver `BT/README.md` para el detalle.
