# Modulo de Fuerza Bruta

Implementacion en C++17 de enumeracion exhaustiva para un alfabeto y una longitud configurables.

## Funcionalidades

- Comparacion directa con una contrasena sintetica, usada como demostracion.
- Comparacion requerida mediante un hash SHA-256 hexadecimal.
- Conteo de candidatos evaluados.
- Medicion del tiempo de ejecucion.
- Validacion del alfabeto, la longitud y el formato del hash.

## Compilacion

Desde la carpeta `FB`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o fuerza_bruta
```

Tambien puede compilarse mediante el archivo `CMakeLists.txt` incluido.

## Prueba de referencia del modulo

Para probar la busqueda de `B7a3` con el alfabeto `0123456789ABCabc`, longitud 4 y la opcion SHA-256, use:

```text
45e30cb7de804478b59781eafd535197fb0ab0e5450110beaf1c1cd3121a6d66
```

La busqueda encuentra la contrasena en el intento 47060.

## Dependencia

El calculo de SHA-256 usa `third_party/picosha2.h`, biblioteca de cabecera distribuida bajo licencia MIT. La licencia se conserva en `third_party/LICENSE-PicoSHA2.txt`.

## Trabajo pendiente del equipo

La comparacion por diccionario, las pruebas automatizadas y la experimentacion general deben integrarse con el resto del proyecto.
