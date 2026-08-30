# ADA_P1_Garcia_Sierra_Velez

Practica 1 — Fuerza Bruta y Backtracking (Analisis y Diseno de Algoritmos).
Contraseñas bajo ataque (FB) y bajo diseño (BT): enumeracion exhaustiva
verificada por hash vs. construccion incremental con poda.

## Integrantes

Orden alfabetico por apellido (la semilla del equipo sale de esta lista):

- Garcia Ortiz, Camila — Modulo BT (Backtracking)
- Sierra Machado, Javier Andres — Modulo FB (Fuerza Bruta)
- Velez Acosta, Jeronimo — QA, verificacion y coordinacion

Semilla del equipo: **1811** (apellidos `garcia`, `sierra`, `velez`, un apellido por integrante).

## Estructura del proyecto

```
README.md
src/
  fb_fuerza_bruta.cpp / fb_fuerza_bruta.hpp   Modulo FB
  bt_backtracking.cpp / bt_backtracking.hpp   Modulo BT
  main.cpp                                     punto de entrada unico (menu)
  third_party/                                 picosha2.h (SHA-256, biblioteca de terceros)
tests/
  test_ada_p1.sh                     pruebas automatizadas
resources/
  diccionario.txt                    diccionario sintetico para el ataque de la Seccion 8.1
results/
  *.csv, *.png, generar_grafica_*.py resultados de la experimentacion
report/
  Informe.pdf                        informe tecnico (Seccion 12)
  informe_borrador.md, *_pseudocodigo_complejidad.md
archivo_referencia/
  FB/, BT/                           implementaciones historicas de cada modulo por separado,
                                      conservadas para trazabilidad; no son parte de la
                                      estructura evaluada por la Seccion 11
```

Los dos modulos se compilan juntos en un unico ejecutable, `ada_p1`, a partir de `src/*.cpp`.

## Compilacion

Con `make` (recomendado):

```bash
make
```

Linea equivalente sin `make`:

```bash
g++ -std=c++17 -O2 -o ada_p1 src/*.cpp
```

**Nota de reproducibilidad.** La linea de compilacion tal como aparece
literalmente en el enunciado (`g++ -std=c++17 -O2 -o ada_p1 src/main.cpp
src/*.cpp`) falla con "multiple definition of main", porque el glob
`src/*.cpp` ya incluye a `src/main.cpp`, y queda pasado dos veces al
compilador. La forma que si compila es la de arriba, sin repetir
`src/main.cpp` — probablemente la intencion del enunciado era solo aclarar
que `main.cpp` esta incluido, no que se liste dos veces.

## Ejecucion

```bash
./ada_p1
```

Presenta un menu: `1` entra al modulo FB, `2` al modulo BT, `3` sale.

## Pruebas automatizadas

```bash
make test
```

Compila desde cero y verifica, contra valores ya validados por el equipo:
la instancia de demostracion de FB, el calculo de semilla/politica de BT,
y que la version con poda y sin poda de BT coincidan en numero de
soluciones (criterio de correctitud de la Seccion 8.2).

## Estado del modulo BT dentro de ada_p1 (importante)

`src/bt_backtracking.cpp/.hpp` todavia refleja el plan de contingencia que
Jeronimo dejo mientras Camila terminaba su propia implementacion. Camila ya
entrego la version real del equipo (ver `archivo_referencia/BT/main.cpp`,
integrada al historial de git el 30 de agosto). **Falta adaptar esa version
al patron `bt::ejecutarMenu()` que espera `src/main.cpp`**, para que el
binario unico `ada_p1` corra el algoritmo real del equipo y no el de
respaldo. Hasta que eso se haga, el binario `ada_p1` compila y corre, pero
el submenu de BT no es el que el equipo va a sustentar como propio.

## Uso de Inteligencia Artificial

Declarado en detalle en la Seccion 17 del informe (`report/Informe.pdf`).
Referencias de apoyo por modulo: `archivo_referencia/FB/CONVERSACIONES_CHATGPT.md`
(Javier) y `archivo_referencia/BT/CONVERSACIONES_IA.md` (Camila).

## Informe tecnico

`report/Informe.pdf` — estructura completa segun la Seccion 12 del
enunciado, maximo 12 paginas sin contar portada.
