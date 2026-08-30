# Modulo de Fuerza Bruta

Implementacion en C++17 de enumeracion exhaustiva para un alfabeto y una longitud configurables.

## Funcionalidades

- Comparacion directa con una contrasena sintetica, usada como demostracion.
- Comparacion requerida mediante un hash SHA-256 hexadecimal.
- Ataque por diccionario contra `resources/diccionario.txt` (Seccion 8.1).
- Comparacion fuerza bruta pura vs. ataque por diccionario, reportando intentos, tiempo y por que el diccionario no es exhaustivo (Seccion 8.1).
- Conteo de candidatos evaluados.
- Medicion del tiempo de ejecucion.
- Validacion del alfabeto, la longitud y el formato del hash.

## Compilacion

Desde la carpeta `FB`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o fuerza_bruta
```

Tambien puede compilarse mediante el archivo `CMakeLists.txt` incluido.

**Importante:** el binario debe ejecutarse desde dentro de la carpeta `FB/`
(`./fuerza_bruta`), porque las opciones 3 y 4 del menu buscan el diccionario
en la ruta relativa `resources/diccionario.txt`.

## Menu

```
1. Comparacion directa con texto plano
2. Comparacion mediante SHA-256
3. Ataque por diccionario (SHA-256)
4. Comparacion fuerza bruta vs. diccionario (Seccion 8.1)
5. Salir
```

## Prueba de referencia del modulo

Para probar la busqueda de `B7a3` con el alfabeto `0123456789ABCabc`, longitud 4 y la opcion SHA-256, use:

```text
45e30cb7de804478b59781eafd535197fb0ab0e5450110beaf1c1cd3121a6d66
```

La busqueda encuentra la contrasena en el intento 47060.

Para probar el ataque por diccionario, la palabra `admin` (linea 81 del
diccionario oficial en `resources/diccionario.txt`) tiene hash SHA-256:

```text
8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918
```

y se encuentra en el intento 81.

## Dependencia

El calculo de SHA-256 usa `third_party/picosha2.h`, biblioteca de cabecera distribuida bajo licencia MIT. La licencia se conserva en `third_party/LICENSE-PicoSHA2.txt`.

## Diccionario

`resources/diccionario.txt` es el **diccionario oficial del curso**
(reemplazado el 30-ago-2026; antes habia un placeholder sintetico de 500
entradas). Ninguna de las 5 contrasenas reales del equipo (semilla 1811:
`axyh`, `2zgx`, `ubwnk`, `b8dm7`, `ovkbsl`) aparece en este diccionario --
es exactamente la demostracion que pide la Seccion 8.1 de que el ataque por
diccionario no es exhaustivo: contra esas 5 instancias, la opcion 3 del
menu reportaria "no encontrada" sin importar cuanto se repita la prueba,
mientras que la fuerza bruta (opciones 1/2) si las encuentra.

## Resultados e instancias del equipo

Las 5 instancias formales del equipo (Seccion 9.1, semilla 1811) y la
experimentacion de la Seccion 8 (tiempo vs. tamano del espacio de busqueda,
alfabetos A1 y A2) ya se corrieron con este mismo codigo -- ver
`ada_p1/results/fb_instancias_equipo.csv`,
`ada_p1/results/fb_experimentacion_seccion8.csv` y
`ada_p1/results/fb_experimentacion_seccion8.png`, y el pseudocodigo/analisis
de complejidad en `ada_p1/report/fb_pseudocodigo_complejidad.md`.

## Trabajo pendiente del equipo

- **Uso de IA:** el ataque por diccionario (opciones 3 y 4 del menu) se
  agrego el 29-ago-2026 con asistencia parcial de IA (Claude), a peticion
  de Jeronimo Velez Acosta (rol de QA), sobre el codigo original de Javier.
  Javier debe revisarlo, entenderlo y poder explicarlo en la sustentacion
  oral antes de que se declare como parte del entregable final; ver
  `ROLES_EQUIPO.md` y `PENDIENTES.md` en la raiz del repositorio.
- Las pruebas automatizadas y la experimentacion completa ya se integraron
  en `ada_p1/` (ver `ada_p1/tests/test_ada_p1.sh` y `ada_p1/results/`).
