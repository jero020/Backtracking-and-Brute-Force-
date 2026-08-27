#include <algorithm>
#include <chrono>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "third_party/picosha2.h"

using namespace std;
using namespace chrono;

enum class ModoComparacion {
    TextoPlano = 1,
    Sha256 = 2
};

string calcularSha256(const string& texto) {
    return picosha2::hash256_hex_string(texto);
}

bool tieneCaracteresUnicos(const string& alfabeto) {
    for (size_t i = 0; i < alfabeto.size(); i++) {
        if (alfabeto.find(alfabeto[i], i + 1) != string::npos) {
            return false;
        }
    }
    return true;
}

bool esHashSha256Valido(const string& hash) {
    return hash.size() == 64 && all_of(hash.begin(), hash.end(), [](unsigned char caracter) {
        return isxdigit(caracter) != 0;
    });
}

bool generarCombinaciones(const string& alfabeto,
                          int longitud,
                          const string& objetivo,
                          ModoComparacion modo,
                          unsigned long long& intentos) {
    string cadena(longitud, alfabeto[0]);

    while (true) {
        intentos++;

        const bool coincide = modo == ModoComparacion::TextoPlano
                                  ? cadena == objetivo
                                  : calcularSha256(cadena) == objetivo;

        if (coincide) {
            cout << "La contrasena es: " << cadena << '\n';
            return true;
        }

        int posicion = longitud - 1;
        while (posicion >= 0) {
            const size_t indice = alfabeto.find(cadena[posicion]);
            if (indice + 1 < alfabeto.size()) {
                cadena[posicion] = alfabeto[indice + 1];
                break;
            }

            cadena[posicion] = alfabeto[0];
            posicion--;
        }

        if (posicion < 0) {
            return false;
        }
    }
}

int main() {
    while (true) {
        cout << "\n=== FUERZA BRUTA ===\n"
             << "1. Comparacion directa con texto plano\n"
             << "2. Comparacion mediante SHA-256\n"
             << "3. Salir\n"
             << "Seleccione una opcion: ";

        int opcion = 0;
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Debe ingresar un numero.\n";
            continue;
        }

        if (opcion == 3) {
            cout << "Programa finalizado.\n";
            break;
        }

        if (opcion != 1 && opcion != 2) {
            cout << "Opcion invalida.\n";
            continue;
        }

        string alfabeto;
        int longitud = 0;
        string objetivo;

        cout << "Alfabeto: ";
        cin >> alfabeto;
        cout << "Longitud de la contrasena: ";
        cin >> longitud;

        if (alfabeto.empty() || !tieneCaracteresUnicos(alfabeto)) {
            cout << "El alfabeto no puede estar vacio ni contener caracteres repetidos.\n";
            continue;
        }

        if (longitud <= 0) {
            cout << "La longitud debe ser mayor que cero.\n";
            continue;
        }

        const ModoComparacion modo = opcion == 1
                                        ? ModoComparacion::TextoPlano
                                        : ModoComparacion::Sha256;

        if (modo == ModoComparacion::TextoPlano) {
            cout << "Contrasena objetivo: ";
            cin >> objetivo;

            if (objetivo.size() != static_cast<size_t>(longitud)) {
                cout << "La contrasena objetivo debe tener la longitud indicada.\n";
                continue;
            }
        } else {
            cout << "Hash SHA-256 objetivo (64 caracteres hexadecimales): ";
            cin >> objetivo;

            transform(objetivo.begin(), objetivo.end(), objetivo.begin(), [](unsigned char caracter) {
                return static_cast<char>(tolower(caracter));
            });

            if (!esHashSha256Valido(objetivo)) {
                cout << "El hash SHA-256 no tiene un formato valido.\n";
                continue;
            }
        }

        unsigned long long intentos = 0;
        const auto inicio = steady_clock::now();

        const bool encontrada = generarCombinaciones(
            alfabeto,
            longitud,
            objetivo,
            modo,
            intentos
        );

        const auto fin = steady_clock::now();
        const auto duracion = duration_cast<microseconds>(fin - inicio);

        if (!encontrada) {
            cout << "No se encontro la contrasena dentro del espacio de busqueda.\n";
        }

        cout << "Intentos realizados: " << intentos << '\n'
             << fixed << setprecision(3)
             << "Tiempo de ejecucion: " << duracion.count() / 1000.0 << " ms"
             << " (" << duracion.count() << " microsegundos)\n";
    }

    return 0;
}
