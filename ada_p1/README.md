# ada_p1 — estructura exacta de la Sección 11

Esta carpeta es **adicional** a `FB/` y `BT/` (que siguen siendo los
binarios de referencia de cada módulo, ya probados). `ada_p1/` existe para
que el repositorio tenga, además, un punto de entrada que cumpla
literalmente lo que pide la Sección 11 del enunciado: un único binario
`ada_p1`, con la estructura `README.md / src/ / tests/ / resources/ /
results/ / report/`, compilado desde `src/main.cpp` + el resto de
`src/*.cpp`.

El código de `src/fb_fuerza_bruta.*` y `src/bt_backtracking.*` es el mismo
que ya está en `FB/main.cpp` y `BT/main.cpp` — solo se separó el antiguo
`int main()` de cada uno en funciones (`fb::ejecutarMenu()`,
`bt::ejecutarMenu()`) para que ambos módulos puedan vivir en un solo
binario sin chocar. La lógica algorítmica no cambió; ver `FB/README.md` y
`BT/README.md` para el detalle de cada módulo.

## Compilación

```bash
g++ -std=c++17 -O2 -o ada_p1 src/*.cpp
```

**Nota sobre la línea de compilación del enunciado.** La Sección 11 escribe
literalmente `g++ -std=c++17 -O2 -o ada_p1 src/main.cpp src/*.cpp`. Al
correrla tal cual, el shell expande `src/*.cpp` y eso *incluye* a
`src/main.cpp` — o sea que `main.cpp` termina pasándose dos veces y el
enlazador falla con `multiple definition of main`. Ya lo verificamos:
falla exactamente así. Usar solo `src/*.cpp` (sin repetir `src/main.cpp`)
compila limpio y da el mismo binario — probablemente la intención del
enunciado era solo aclarar que `main.cpp` está incluido, no que se liste
dos veces. Si el docente insiste en la línea exacta, avisen de este
detalle en la sustentación.

## Ejecución

```bash
./ada_p1
```

Presenta un menú: `1` entra al módulo FB, `2` al módulo BT, `3` sale. Cada
submenú es el mismo que ya tenían `FB/main.cpp` y `BT/main.cpp` de forma
independiente.

## Pruebas

```bash
tests/test_ada_p1.sh
```

Compila desde cero y verifica, vía el menú interactivo: (1) que el módulo
FB encuentra la contraseña de demostración de Javier en el número de
intentos ya documentado; (2) que el módulo BT calcula la semilla y la
política del equipo correctamente; (3) que la comparación con poda vs. sin
poda del módulo BT da el mismo número de soluciones en ambas versiones
(criterio de correctitud de la Sección 8.2).

## resources/ y results/

Por ahora vacías salvo un `.gitkeep`. El enunciado menciona que
InteractivaVirtual publicará una utilidad de referencia
(`resources/verificar_semilla.cpp`) para que cada equipo confirme su
semilla — pendiente de que se publique, no está incluida aquí. El
diccionario del curso (`resources/diccionario.txt`) y los resultados de
los experimentos (CSV, gráficas) también van aquí una vez estén listos —
ver `PENDIENTES.md` en la raíz del repositorio.

## report/

Aquí va `Informe.pdf` (Sección 12) cuando esté listo.

## Uso de IA

Este `ada_p1/` (la separación de `main()` en `fb::ejecutarMenu()` /
`bt::ejecutarMenu()`, el `main.cpp` unificado, y `tests/test_ada_p1.sh`) se
construyó con asistencia de IA (Claude) el 29-ago-2026, a partir del código
ya existente y probado de `FB/` y `BT/`, para cumplir la estructura exacta
de la Sección 11. La declaración formal de uso de IA para el informe
(Sección 17) queda pendiente de completar por el equipo — ver
`PENDIENTES.md`.
