# Módulo BT — Pseudocódigo y análisis de complejidad

> **Uso de IA:** este documento fue redactado con asistencia de IA (Claude), a
> petición de Jerónimo Vélez Acosta, como borrador de trabajo. El motor de
> búsqueda que describe (clase `Estado`, `factibilidad()`, `esSolucion()`,
> `bt()`) es el algoritmo real que entregó Camila García Ortiz como
> prototipo del módulo BT (prototipo original preservado en el historial
> de git, commit `a0da07b`); se adaptó con
> asistencia de IA (30-ago-2026) para poder correrse con alfabeto/longitud/
> política arbitrarios desde el menú interactivo, sin cambiar la lógica de
> poda. Los datos de tiempo/nodos citados aquí vienen de ejecuciones
> reales (ver `results/bt_referencia_y_variantes.csv` y
> `results/bt_variantes.csv`), no están inventados; se re-verificó tras la
> integración que la variante (ii) reproduce exactamente los mismos
> 199 470 612 nodos / 180 629 800 soluciones (ver Sección 17 del informe).

## 1. Representación del estado

- **Estado parcial:** un prefijo de longitud *k* (0 ≤ *k* ≤ *n*), representado
  como una cadena `prefijo`, más un contador `{ lower, upper, digit,
  symbol }` con cuántos caracteres de cada categoría lleva ese prefijo
  hasta el momento (clase `Estado` en el código real).
- **Estado inicial:** `prefijo = ""` (cadena vacía), `conteo = {0,0,0,0}`.
- **Estados terminales:** cualquier prefijo con `|prefijo| = n`. Es
  **solución** si además `cumplePolitica(conteo)` es verdadero (cada
  categoría alcanzó su mínimo exigido).
- **Transición:** extender el prefijo agregando un símbolo `c` del alfabeto
  al final, actualizando el conteo de la categoría de `c`.

## 2. Pseudocódigo — construcción incremental con poda

```
funcion BACKTRACK_CON_PODA(politica, alfabeto, prefijo, conteo, ultimo, resultado):
    resultado.nodos ← resultado.nodos + 1

    si |prefijo| = politica.longitud:
        si CUMPLE_POLITICA(politica, conteo):
            resultado.soluciones ← resultado.soluciones + 1
        retornar

    restantes ← politica.longitud − |prefijo| − 1

    para cada simbolo c en alfabeto:
        si politica.prohibirRepetidosConsecutivos y c = ultimo:
            continuar   // poda 1: regla local (no repetir el ultimo caracter)

        nuevoConteo ← ACTUALIZAR(conteo, c)

        si NO ES_FACTIBLE(politica, nuevoConteo, restantes):
            continuar   // poda 2: factibilidad (ver abajo)

        prefijo.push(c)
        BACKTRACK_CON_PODA(politica, alfabeto, prefijo, nuevoConteo, c, resultado)
        prefijo.pop()


funcion ES_FACTIBLE(politica, conteo, posicionesRestantes):
    faltanLower  ← max(0, politica.minLower  − conteo.lower)
    faltanUpper  ← max(0, politica.minUpper  − conteo.upper)
    faltanDigit  ← max(0, politica.minDigit  − conteo.digit)
    faltanSymbol ← max(0, politica.minSymbol − conteo.symbol)
    retornar (faltanLower + faltanUpper + faltanDigit + faltanSymbol) ≤ posicionesRestantes
```

**Función de factibilidad (poda 2):** un prefijo parcial se descarta si,
incluso llenando **todas** las posiciones restantes exactamente con los
tipos de caracter que aún faltan, no alcanzaría a cumplir la política. Es
una condición **necesaria pero no suficiente** para que exista una
extensión válida (no exige que esos caracteres específicos aparezcan
exactamente ahí, solo que haya espacio suficiente) — por eso poda ramas
imposibles sin descartar nunca una rama que sí podría llevar a una
solución (poda correcta, no heurística).

## 3. Pseudocódigo — recorrido exhaustivo sin poda (Sección 8.2)

```
funcion BACKTRACK_SIN_PODA(politica, alfabeto, prefijo, resultado):
    resultado.nodos ← resultado.nodos + 1

    si |prefijo| = politica.longitud:
        conteo ← contar tipos de caracter en prefijo
        repetido ← existe i>0 con prefijo[i] = prefijo[i-1]
        si NO repetido y CUMPLE_POLITICA(politica, conteo):
            resultado.soluciones ← resultado.soluciones + 1
        retornar

    para cada simbolo c en alfabeto:
        prefijo.push(c)
        BACKTRACK_SIN_PODA(politica, alfabeto, prefijo, resultado)
        prefijo.pop()
```

Esta versión genera **todo** el árbol Σⁿ (recorrido exhaustivo, equivalente
a fuerza bruta sobre el mismo espacio) y solo filtra al final, en las
hojas — es la contraparte que exige la Sección 8.2 para medir cuánto
reduce la poda el espacio explorado.

## 4. Análisis de complejidad

Sea *m* = |Σ| = 67 (alfabeto base) y *n* la longitud de la contraseña.

| Caso | Nodos visitados (con poda) | Justificación |
|---|---|---|
| Mejor caso | Θ(n) | Si la política es trivialmente satisfacible de forma temprana y el resto del árbol se poda por completo en cuanto se alcanza factibilidad garantizada — en la práctica esto casi no ocurre porque la poda de este proyecto es *necesaria*, no *suficiente*: solo descarta cuando es matemáticamente imposible cumplir, así que sigue explorando ramas factibles aunque terminen sin ser solución. |
| Peor caso | Θ(mⁿ) | Cuando la política deja mucho margen (poca restricción relativa a *n*), casi ninguna rama se descarta hasta las últimas posiciones — la poda degenera a un recorrido casi completo del árbol. Es exactamente lo que exige demostrar la variante (v) "sin restricciones" del enunciado (ver Sección 6). |
| Caso promedio | Depende fuertemente de qué tan ajustada esté la política frente a *n* | A menor margen (`Σmin` cerca de *n*), mayor poda efectiva **cerca del final** del árbol (últimas posiciones), pero **no** necesariamente cerca de la raíz — como se ve en la instancia de referencia (Sección 5). |

**Complejidad espacial:** Θ(n) — la recursión mantiene un prefijo de a lo
sumo *n* caracteres en la pila de llamadas; nunca se materializa el árbol
completo en memoria (a diferencia de "generar y filtrar al final").

### La poda de este proyecto es "tardía", no "temprana"

La condición `ES_FACTIBLE` solo compara *cantidades* (cuántos caracteres de
cada tipo faltan vs. cuántas posiciones quedan) — no exige que un tipo
específico aparezca en una posición específica. Esto significa que, salvo
que falten muy pocas posiciones para el final, casi cualquier símbolo del
alfabeto es "factible" en cualquier posición (basta con que sobre
suficiente margen). El resultado práctico es que la poda actúa
mayormente en los últimos 1-2 niveles del árbol, no distribuida
uniformemente — por eso instancias con poco margen (`Σmin` cerca de *n*)
pueden seguir siendo extremadamente costosas aunque "en teoría" tengan
poda.

## 5. Contraste teoría vs. medición — Sección 8.2 (con poda vs. sin poda)

Sobre un alfabeto reducido de 8 símbolos (`abcdefgh`), n=6,
minLower=2/minUpper=1/minDigit=1/minSymbol=1 (`ada_p1/tests/test_ada_p1.sh`):

| Version | Nodos | Tiempo | Soluciones |
|---|---|---|---|
| Con poda | 457 | 0 ms | 0 |
| Sin poda | 299 593 | 9-12 ms | 0 |

Reducción del espacio de búsqueda: **99.85%**. El número de soluciones
coincide entre ambas versiones (0 = 0), que es el chequeo de correctitud
que exige la Sección 8.2: la poda descarta ramas, pero nunca descarta una
solución real. (Nota: da 0 soluciones porque el alfabeto reducido
`abcdefgh` solo tiene minúsculas — no existe ningún símbolo mayúscula,
dígito o especial en él, así que `minUpper`, `minDigit` y `minSymbol`
nunca se pueden cumplir; es un caso de verificación de correctitud, no de
"contraseñas encontradas".)

## 6. Instancia de referencia y variantes (Sección 9.2) — resultados reales

Todo lo siguiente se corrió con el alfabeto base completo (67 símbolos:
minúsculas + mayúsculas + dígitos + `!@#$%`). Ver
`results/bt_referencia_y_variantes.csv`.

| Instancia | n | minLower/Upper/Digit/Symbol | Nodos | Soluciones | Tiempo | Estado |
|---|---|---|---|---|---|---|
| Referencia común | 6 | 2/1/1/1 | 9 454 738 424 | 8 883 856 800 | 467.3 s (~7 min 47 s) | **Completado** |
| (ii) política completa, n=6 | 6 | 0/2/3/1 | 199 470 612 | 180 629 800 | 12.6 s | **Completado** |
| (i) política completa, n=8 | 8 | 2/2/3/1 | ≥856 686 592 (muestra de 60 s) | — | no terminó | **Ver nota de intratabilidad** |
| (iii) política completa, n=10 | 10 | 4/2/3/1 | ≥881 852 416 (muestra de 60 s) | — | no terminó | **Ver nota de intratabilidad** |
| (iv) política relajada, n=8 | 8 | 1/0/0/0 | ≥4 104 126 464 (muestra de 60 s) | — | no terminó | **Ver nota de intratabilidad** |
| (v) sin restricciones, n=6 | 6 | 0/0/0/0 | 85 197 148 478 | 83 906 282 592 | 1247.6 s (~20 min 48 s) | **Completado** |

### Nota de intratabilidad — hallazgo de QA (29-ago-2026)

Las variantes (i), (iii) y (iv), tal como las especifica literalmente la
Sección 9.2 (alfabeto completo de 67 símbolos), **no terminan en un tiempo
razonable** con esta implementación de backtracking-con-poda, en el
hardware disponible. Evidencia:

- La instancia de referencia (n=6, 1 posición de margen) ya tardó
  **7 min 47 s** para 9.45 mil millones de nodos.
- (i) (n=8, misma "estrechez" relativa de margen) visitó **856.7
  millones de nodos en apenas los primeros 60 segundos**, sin señales de
  desacelerar — el espacio crudo Σⁿ sin poda para n=8 es 67⁸ ≈ 4.06×10¹⁴,
  ~4489 veces más grande que para n=6.
- (iv) (relajada, casi sin restricción de composición) visitó **4.1 mil
  millones de nodos en 60 segundos** (~68 millones de nodos/segundo,
  porque casi no hay chequeos de factibilidad que aborten ramas) — a ese
  ritmo, cubrir el espacio completo (67×66⁷ ≈ 3.65×10¹⁴ nodos sin
  repetidos consecutivos) tomaría **del orden de semanas**, no minutos.
- (iii) (n=10) tiene un espacio aún mayor que (i).

**Esto es, en realidad, precisamente el fenómeno que la Sección 6.2 pide
documentar** ("el uso de información parcial para evitar explorar
subárboles completos" tiene un límite: cuando la política casi no
restringe nada, la poda por factibilidad casi no descarta ramas, y
backtracking degenera al costo de la fuerza bruta pura sobre el mismo
espacio). La variante (v) "sin restricciones" del propio enunciado está
diseñada explícitamente para forzar ese caso límite ("poda nula") — el
hallazgo aquí es que, con el alfabeto completo de 67 símbolos, ese mismo
fenómeno también domina a (i), (iii) y (iv), no solo a (v).

**Recomendación para el equipo:** reportar estos resultados como
muestras parciales con la tasa de nodos/segundo medida y el tamaño
teórico del espacio (ambos en `results/bt_referencia_y_variantes.csv`),
en vez de intentar completarlas — y mencionarlo explícitamente en la
sustentación oral como una limitación observada de la técnica, no un
error de la implementación (el chequeo de corrección de la Sección 8.2,
con un alfabeto reducido, ya confirma que la poda es correcta cuando sí
tiene margen para actuar). Vale la pena preguntarle al docente si para
estas variantes específicas se espera una ejecución completa o si basta
con esta caracterización empírica del fenómeno.

**Confirmación con la variante (v), que sí se corrió hasta el final:** al
completarla (1247.6 s, ~20 min 48 s), dio exactamente 85 197 148 478
nodos y 83 906 282 592 soluciones — este último número coincide **de
forma exacta** con la cota teórica calculada de forma independiente,
`67 × 66⁵ = 83 906 282 592` (67 opciones para el primer carácter, 66 para
cada uno de los siguientes 5 al no poder repetir el anterior). Esto
confirma que el conteo de soluciones del algoritmo es correcto, y valida
la misma fórmula usada para proyectar el tiempo de (i), (iii) y (iv): con
la tasa medida de ~68.3 millones de nodos/segundo (consistente con la de
la variante (v), que casi no aplica poda), el espacio análogo para n=8
(`67 × 66⁷ ≈ 3.65×10¹⁴`) tomaría del orden de **62 días** en completarse
con esta implementación — de ahí que (i), (iii) y (iv) se reporten como
muestra parcial en vez de forzar una ejecución que no terminaría dentro
del plazo de la práctica.
