# Módulo FB — Pseudocódigo y análisis de complejidad

> **Uso de IA:** este documento fue redactado con asistencia de IA (Claude), a
> petición de Jerónimo Vélez Acosta,
> (autor del Módulo FB) lo revise, corrija y adopte como propio antes de
> incluirlo en el informe final. Los datos de tiempo/intentos citados aquí
> vienen de ejecuciones reales de `FB/main.cpp` (ver
> `results/fb_experimentacion_seccion8.csv` y `fb_instancias_equipo.csv`), no
> están inventados.

## 1. Pseudocódigo — enumeración exhaustiva de Σⁿ

```
funcion FUERZA_BRUTA(Σ, n, objetivo, modo):
    // Σ: alfabeto (m = |Σ| símbolos distintos, sin repetidos)
    // n: longitud de la contraseña
    // objetivo: contraseña en texto plano, o hash SHA-256 de la contraseña
    // modo: TEXTO_PLANO | SHA256

    candidato ← [Σ[0], Σ[0], ..., Σ[0]]   // n copias de Σ[0], p.ej. "aaaa"
    intentos ← 0

    mientras verdadero:
        intentos ← intentos + 1

        si modo = TEXTO_PLANO:
            coincide ← (candidato = objetivo)
        sino:
            coincide ← (SHA256(candidato) = objetivo)

        si coincide:
            retornar (ENCONTRADA, candidato, intentos)

        // "odómetro" en base m: incrementa el último símbolo; si se
        // desborda, lo reinicia y arrastra el acarreo a la izquierda
        posicion ← n - 1
        mientras posicion ≥ 0:
            indice ← indice_en(Σ, candidato[posicion])
            si indice + 1 < m:
                candidato[posicion] ← Σ[indice + 1]
                salir del ciclo interno
            sino:
                candidato[posicion] ← Σ[0]
                posicion ← posicion − 1

        si posicion < 0:
            retornar (NO_ENCONTRADA, ∅, intentos)   // se recorrio Σⁿ completo
```

**Por qué no omite ni repite candidatos:** el ciclo interno implementa un
contador de base *m* con *n* dígitos (idéntico a contar en binario, pero en
base *m*), donde cada símbolo de Σ hace de "dígito". Empezando en
`Σ[0]Σ[0]...Σ[0]` (equivalente a 0) y terminando en `Σ[m-1]Σ[m-1]...Σ[m-1]`
(equivalente a mⁿ−1), este contador visita cada una de las mⁿ combinaciones
posibles exactamente una vez, en orden lexicográfico según el orden de Σ —
es la misma garantía que tiene contar de 0 a mⁿ−1 en base *m* sin saltarse
ni repetir ningún número.

## 2. Pseudocódigo — ataque por diccionario (Sección 8.1)

```
funcion ATAQUE_DICCIONARIO(palabras, objetivo, modo):
    // palabras: lista finita de D candidatos plausibles (resources/diccionario.txt)
    intentos ← 0
    para cada palabra en palabras:
        intentos ← intentos + 1
        si modo = TEXTO_PLANO:
            coincide ← (palabra = objetivo)
        sino:
            coincide ← (SHA256(palabra) = objetivo)
        si coincide:
            retornar (ENCONTRADA, palabra, intentos)
    retornar (NO_ENCONTRADA, ∅, intentos)
```

A diferencia de `FUERZA_BRUTA`, este algoritmo **no** recorre Σⁿ: solo
prueba las *D* palabras de la lista. Es mucho más rápido cuando la
contraseña sí está en la lista, pero **no es exhaustivo** — si la
contraseña no es una de esas *D* palabras, el algoritmo nunca la
encuentra, sin importar cuánto tiempo se le dé. La Sección 8.1 pide
justamente comparar esto contra la fuerza bruta pura, que sí cubre todo el
espacio.

## 3. Análisis de complejidad — `FUERZA_BRUTA`

Sea *m* = |Σ| (tamaño del alfabeto) y *n* la longitud de la contraseña.
Cada iteración hace una comparación directa (modo texto plano, costo
Θ(n)) o un hash SHA-256 (modo hash, costo Θ(n) también, ya que SHA-256
procesa la entrada por bloques de tamaño fijo — para las longitudes de
este proyecto, n ≤ 10, es efectivamente un costo constante y pequeño por
intento).

| Caso | Número de intentos | Justificación |
|---|---|---|
| Mejor caso | Θ(1) | La contraseña objetivo es el primer candidato generado (`Σ[0]ⁿ`). |
| Peor caso | Θ(mⁿ) | La contraseña es el último candidato, o no existe en el espacio — hay que recorrer las *mⁿ* combinaciones completas antes de poder afirmar "no encontrada". |
| Caso promedio | Θ(mⁿ) | Asumiendo que la posición del candidato correcto se distribuye uniformemente entre las *mⁿ* combinaciones, el número esperado de intentos es (mⁿ + 1)/2 — sigue siendo del mismo orden exponencial que el peor caso; solo cambia la constante (mitad), no la clase de complejidad. |

**Complejidad temporal total:** Θ(mⁿ) en los tres casos (con una constante
menor en el mejor caso, y una constante ~½ en el promedio frente al peor
caso), porque cada intento cuesta Θ(1) amortizado respecto al tamaño del
espacio (el costo por intento no depende de *mⁿ*, solo de *n*, que es
pequeño y fijo para cada instancia).

**Complejidad espacial:** Θ(n) — solo se mantiene la cadena candidata
actual (longitud *n*) y un contador de intentos; no se materializa nunca
el espacio completo de *mⁿ* combinaciones en memoria.

### Contraste teoría vs. medición (Sección 8, `results/fb_experimentacion_seccion8.csv`)

Se corrió `FB/main.cpp` (modo SHA-256, peor caso: contraseña objetivo
fuera del espacio, para forzar el recorrido completo) sobre 6
configuraciones — alfabetos A1 (26 símbolos) y A2 (36 símbolos), longitud
n ∈ {3,4,5}:

| Alfabeto | n | Espacio (mⁿ) | Intentos | Tiempo (ms) | µs/intento |
|---|---|---|---|---|---|
| A1 | 3 | 17 576 | 17 576 | 22.0 | 1.25 |
| A1 | 4 | 456 976 | 456 976 | 535.7 | 1.17 |
| A1 | 5 | 11 881 376 | 11 881 376 | 13 624.4 | 1.15 |
| A2 | 3 | 46 656 | 46 656 | 46.9 | 1.01 |
| A2 | 4 | 1 679 616 | 1 679 616 | 1 736.8 | 1.03 |
| A2 | 5 | 60 466 176 | 60 466 176 | 69 203.2 | 1.14 |

El costo por intento se mantiene aproximadamente constante (~1.0–1.25
µs), sin importar *m* o *n* — exactamente lo que predice el modelo
teórico Θ(mⁿ) con costo Θ(1) amortizado por intento. La gráfica
`fb_experimentacion_seccion8.png` muestra esto de dos formas: tiempo vs.
*n* en escala semi-logarítmica (una línea recta confirma crecimiento
exponencial en *n*), y tiempo vs. tamaño del espacio en escala log-log
(una línea de pendiente ≈1 confirma que el tiempo crece proporcional al
tamaño del espacio, tal como predice la teoría). Esto es el "muro
exponencial": entre A1 n=3 (17 576 combinaciones, 22 ms) y A1 n=5
(11 881 376 combinaciones, ~13.6 s) la longitud solo creció en 2, pero el
tiempo se multiplicó por ~620 veces.

Las 5 instancias formales del equipo (`fb_instancias_equipo.csv`,
semilla 1811) confirman el mismo patrón con espacios de búsqueda reales
del proyecto: la instancia 5 (A1, n=6, espacio ≈ 3.09×10⁸) tardó
~3 min 34 s en encontrarse (en el punto 176 112 676 de 308 915 776, un
57% del espacio) — el mismo módulo, sin ningún cambio de algoritmo, pasa
de milisegundos a minutos únicamente por aumentar *n* en un par de
unidades.

## 4. Complejidad — `ATAQUE_DICCIONARIO`

Sea *D* = número de palabras en el diccionario (D=500 en el diccionario
sintético actual del equipo; ver pendiente de reemplazarlo por el oficial
del curso).

| Caso | Número de intentos |
|---|---|
| Mejor caso | Θ(1) |
| Peor caso | Θ(D) |
| Caso promedio | Θ(D) |

Como *D* no depende de *mⁿ* (es una constante fija, típicamente mucho
menor que el espacio de búsqueda completo), el ataque por diccionario es
órdenes de magnitud más rápido que la fuerza bruta pura *cuando la
contraseña está en la lista* — pero pierde por completo la garantía de
encontrarla si no lo está. `results/fb_experimentacion_seccion8.csv` /
las pruebas de `tests/test_ada_p1.sh` muestran un caso positivo
(`admin`, encontrada en el intento 81 de 500, en <1 ms) y la comparación
directa contra la fuerza bruta sobre el mismo objetivo (opción 4 del menú
FB) para dejar ambos costos lado a lado.
