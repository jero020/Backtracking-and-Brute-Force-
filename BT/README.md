# Modulo de Backtracking

Implementacion en C++17 de busqueda con backtracking y poda por factibilidad,
sobre la politica de composicion de la Seccion 9.2 del enunciado (minimos de
minusculas, mayusculas, digitos y simbolos, mas la restriccion de no tener
caracteres repetidos consecutivos).

## Aviso: este modulo es un plan de contingencia

Este `main.cpp` fue escrito con asistencia de IA (Claude) por Jeronimo Velez
Acosta, como plan de contingencia mientras Camila Garcia Ortiz (responsable
del modulo BT) terminaba su propia version. **No reemplaza el trabajo de
Camila**: si ella entrega su propia implementacion, esa es la que debe
integrarse y presentarse como el modulo BT del equipo, y este archivo queda
solo como respaldo. Si no llega a tiempo, este es el codigo que se
entregaria en su lugar, y quien lo presente en la sustentacion oral (Seccion
16) debe poder explicarlo linea por linea, tal como exige el enunciado para
cualquier codigo generado con IA (Secciones 14 y 17).

## Funcionalidades

- Calculo de la semilla del equipo y derivacion de la politica de
  composicion (Seccion 9.1/9.2), con el ajuste de `minLower` cuando la suma
  de minimos supera la longitud.
- Backtracking con poda por factibilidad (minimos pendientes vs. posiciones
  restantes) y poda de la regla "no repetidos consecutivos".
- Version sin ninguna poda, para la comparacion de correctitud que exige la
  Seccion 8.2 (el numero de soluciones debe coincidir entre ambas).
- Menu interactivo con 4 opciones: validar la instancia de referencia,
  calcular semilla y politica, generar y correr las 5 variantes de la
  Seccion 9.2, y comparar con/sin poda sobre un alfabeto reducido.

## Compilacion

Desde la carpeta `BT`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o bt
```

Tambien puede compilarse mediante el archivo `CMakeLists.txt` incluido.

## Aviso importante sobre tiempos de ejecucion

La instancia de referencia oficial (n=6, alfabeto completo de 67 simbolos,
minLower=2 minUpper=1 minDigit=1 minSymbol=1) deja solo 1 caracter "libre"
de 6: la poda por factibilidad casi no actua hasta las ultimas posiciones,
asi que el arbol recorrido sigue siendo enorme. Correr la opcion 1
(referencia) o la variante (v) de la opcion 3 puede tardar varios minutos.
Esto no es un error del programa -- es justamente el fenomeno que la
Seccion 6.2 pide documentar (el peor caso de backtracking conserva la cota
exponencial). Para verificar rapido que la logica es correcta, use la
opcion 4 (comparar con poda vs. sin poda) sobre un alfabeto reducido, que
corre en menos de un segundo.

## Trabajo pendiente del equipo

- Confirmar con el docente si el alfabeto de simbolos son 67 o 69 caracteres
  (el enunciado dice "69 simbolos en total" pero minusculas+mayusculas+
  digitos+`!@#$%` suma 67; aqui se implemento literalmente lo descrito).
- Correr hasta el final la instancia de referencia y las 5 variantes de la
  Seccion 9.2 con tiempo suficiente, y guardar los resultados.
- Generar las graficas (nodos/tiempo vs. tamano) a partir de esos
  resultados.
- Pseudocodigo formal, analisis de complejidad (mejor/peor/promedio) y
  conclusiones para el informe tecnico.
- Si Camila entrega su propia version, reemplazar este `main.cpp` por el de
  ella y mover este archivo a una carpeta de respaldo en vez de borrarlo.

## Uso de IA declarado

Modulo generado con asistencia de IA (Claude) el 23-28 de agosto de 2026,
por Jeronimo Velez Acosta, como plan de contingencia porque el modulo BT
del repositorio compartido aun no tenia codigo. Incluye: modelamiento del
estado y la politica, funcion de factibilidad y poda, version sin poda para
comparacion, calculo de semilla/instancias de la Seccion 9.2, y el menu de
experimentacion. Esta declaracion debe completarse o ajustarse en el
informe final segun quien termine presentando este modulo (Seccion 14 y 17
del enunciado).
