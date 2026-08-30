// ada_p1 -- punto de entrada unico exigido por la Seccion 11 del enunciado
// (g++ -std=c++17 -O2 -o ada_p1 src/main.cpp src/*.cpp). Este archivo no
// contiene logica algoritmica propia: solo presenta un menu que delega en
// los dos modulos, cada uno en su propio par de archivos:
//   - fb_fuerza_bruta.hpp/.cpp -- Modulo FB (Javier Andres Sierra Machado)
//   - bt_backtracking.hpp/.cpp -- Modulo BT (respaldo de Jeronimo Velez
//     Acosta mientras Camila Garcia Ortiz entrega el suyo; ver
//     ../README.md y ../../BT/README.md)
//
// Este ada_p1/ es una carpeta NUEVA, separada de FB/ y BT/ (que siguen
// siendo los binarios de referencia probados de cada modulo). Existe
// unicamente para que el repositorio tenga, ademas, un punto de entrada
// que cumpla literalmente la linea de compilacion de la Seccion 11.

#include <iostream>
#include <limits>

#include "fb_fuerza_bruta.hpp"
#include "bt_backtracking.hpp"

using namespace std;

int main() {
    while (true) {
        cout << "\n=== ADA - Practica 1: Fuerza Bruta y Backtracking ===\n"
             << "1. Modulo Fuerza Bruta (FB)\n"
             << "2. Modulo Backtracking (BT)\n"
             << "3. Salir\n"
             << "Seleccione una opcion: ";

        int opcion = 0;
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Debe ingresar un numero.\n";
            continue;
        }

        switch (opcion) {
            case 1: fb::ejecutarMenu(); break;
            case 2: bt::ejecutarMenu(); break;
            case 3: cout << "Programa finalizado.\n"; return 0;
            default: cout << "Opcion invalida.\n"; break;
        }
    }
}
