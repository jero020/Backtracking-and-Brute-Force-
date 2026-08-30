#!/usr/bin/env bash
# Prueba automatizada minima exigida por la Seccion 11 (verificacion contra
# las instancias de referencia de la Seccion 9) para el binario unico
# ada_p1. Compila desde cero y corre, via el menu interactivo, casos ya
# validados manualmente por el equipo:
#   - FB: la instancia de demostracion de Javier (alfabeto
#     "0123456789ABCabc", longitud 4, objetivo "B7a3" en texto plano),
#     verificando que se encuentra en el intento 47060.
#   - FB: el ataque por diccionario (Seccion 8.1) contra el hash de
#     "admin" (palabra #81 del diccionario oficial en resources/diccionario.txt),
#     y la comparacion
#     fuerza bruta vs. diccionario sobre un alfabeto reducido ("adimn").
#   - BT: el calculo de semilla/politica del equipo (apellidos
#     sierra garcia velez -> semilla 1811, politica n=8 ajustada), y la
#     comparacion con poda vs. sin poda sobre un alfabeto reducido
#     (Seccion 8.2), verificando que el numero de soluciones coincide
#     entre ambas versiones.
#
# Automatizado con asistencia parcial de IA (Claude) y revisado por
# Jeronimo Velez Acosta (rol de QA, ver ROLES_EQUIPO.md); los valores de
# referencia que se comparan aqui salen de ejecuciones manuales ya
# verificadas por el equipo, no solo de lo que genero la IA.
set -e
cd "$(dirname "$0")/.."

echo "== Compilando ada_p1 =="
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o ada_p1 src/*.cpp

fallos=0

echo "== FB: instancia de demostracion (texto plano) =="
salida_fb=$(printf "1\n1\n0123456789ABCabc\n4\nB7a3\n5\n3\n" | ./ada_p1)
echo "$salida_fb" | grep -q "La contrasena es: B7a3" || { echo "FALLO: no encontro B7a3"; fallos=1; }
echo "$salida_fb" | grep -q "Intentos realizados: 47060" || { echo "FALLO: intentos != 47060"; fallos=1; }

echo "== FB: ataque por diccionario (Seccion 8.1) =="
hash_admin="8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918"
salida_dic=$(printf "1\n3\n%s\n5\n3\n" "$hash_admin" | ./ada_p1)
echo "$salida_dic" | grep -q "La contrasena es: admin" || { echo "FALLO: diccionario no encontro admin"; fallos=1; }
echo "$salida_dic" | grep -q "Intentos realizados: 81" || { echo "FALLO: intentos de diccionario != 81"; fallos=1; }

echo "== FB: comparacion fuerza bruta vs. diccionario (Seccion 8.1) =="
salida_comp_fb=$(printf "1\n4\nadimn\n5\n%s\n5\n3\n" "$hash_admin" | ./ada_p1)
echo "$salida_comp_fb" | grep -q 'Diccionario  : encontrada ("admin")' || { echo "FALLO: comparacion diccionario no encontro admin"; fallos=1; }
echo "$salida_comp_fb" | grep -q 'Fuerza bruta : encontrada ("admin")' || { echo "FALLO: comparacion fuerza bruta no encontro admin"; fallos=1; }

echo "== BT: semilla y politica del equipo =="
salida_semilla=$(printf "2\n2\nsierra garcia velez\n5\n3\n" | ./ada_p1)
echo "$salida_semilla" | grep -q "Semilla del equipo = 1811" || { echo "FALLO: semilla != 1811"; fallos=1; }
echo "$salida_semilla" | grep -q "minLower=2 minUpper=2 minDigit=3 minSymbol=1" || { echo "FALLO: politica incorrecta"; fallos=1; }

echo "== BT: comparacion con poda vs. sin poda (Seccion 8.2) =="
salida_comp=$(printf "2\n4\n8\n6\n2\n1\n1\n1\n5\n3\n" | ./ada_p1)
echo "$salida_comp" | grep -q "OK: el numero de soluciones coincide entre ambas versiones." || { echo "FALLO: con/sin poda no coinciden"; fallos=1; }

if [ "$fallos" -eq 0 ]; then
    echo "[tests/test_ada_p1.sh] TODAS LAS PRUEBAS PASARON"
    exit 0
else
    echo "[tests/test_ada_p1.sh] HAY PRUEBAS QUE FALLARON"
    exit 1
fi
