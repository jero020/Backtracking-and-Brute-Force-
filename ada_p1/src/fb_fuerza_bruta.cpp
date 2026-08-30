#include "fb_fuerza_bruta.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include "third_party/picosha2.h"

namespace fb {

using namespace std;
using namespace chrono;

// Ruta por defecto del diccionario, relativa al directorio desde el que se
// ejecuta el binario (se espera correr "./ada_p1" desde la carpeta ada_p1/).
const string RUTA_DICCIONARIO_DEFECTO = "resources/diccionario.txt";

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

string normalizarHash(string hash) {
    transform(hash.begin(), hash.end(), hash.begin(), [](unsigned char caracter) {
        return static_cast<char>(tolower(caracter));
    });
    return hash;
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

// Version "silenciosa" de la fuerza bruta (no imprime nada): la usa la
// comparacion de la Seccion 8.1 para poder mostrar ambos resultados juntos
// en un solo bloque, en vez de la salida entremezclada que dejaria
// generarCombinaciones().
ResultadoBusqueda fuerzaBrutaSilenciosa(const string& alfabeto,
                                        int longitud,
                                        const string& objetivo,
                                        ModoComparacion modo) {
    ResultadoBusqueda resultado;
    string cadena(longitud, alfabeto[0]);

    while (true) {
        resultado.intentos++;

        const bool coincide = modo == ModoComparacion::TextoPlano
                                  ? cadena == objetivo
                                  : calcularSha256(cadena) == objetivo;

        if (coincide) {
            resultado.encontrada = true;
            resultado.candidata = cadena;
            return resultado;
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
            return resultado;
        }
    }
}

// Carga las palabras del diccionario, una por linea. Tolera lineas vacias y
// finales de linea estilo Windows (\r\n). No es un "wordlist" exhaustivo del
// espacio de busqueda: es una lista finita de candidatos plausibles.
vector<string> cargarDiccionario(const string& ruta) {
    vector<string> palabras;
    ifstream archivo(ruta);

    if (!archivo) {
        return palabras;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (!linea.empty()) {
            palabras.push_back(linea);
        }
    }

    return palabras;
}

ResultadoBusqueda ataquePorDiccionario(const vector<string>& palabras,
                                       const string& objetivo,
                                       ModoComparacion modo) {
    ResultadoBusqueda resultado;

    for (const auto& palabra : palabras) {
        resultado.intentos++;

        const bool coincide = modo == ModoComparacion::TextoPlano
                                  ? palabra == objetivo
                                  : calcularSha256(palabra) == objetivo;

        if (coincide) {
            resultado.encontrada = true;
            resultado.candidata = palabra;
            return resultado;
        }
    }

    return resultado;
}

void imprimirResultado(const string& etiqueta, const ResultadoBusqueda& resultado, long long microsegundos) {
    cout << etiqueta << ": "
         << (resultado.encontrada ? "encontrada" : "NO encontrada");
    if (resultado.encontrada) {
        cout << " (\"" << resultado.candidata << "\")";
    }
    cout << " | intentos=" << resultado.intentos
         << " | tiempo=" << fixed << setprecision(3) << microsegundos / 1000.0 << " ms\n";
}

void opcionTextoPlanoOSha256(ModoComparacion modo) {
    string alfabeto;
    int longitud = 0;
    string objetivo;

    cout << "Alfabeto: ";
    cin >> alfabeto;
    cout << "Longitud de la contrasena: ";
    cin >> longitud;

    if (alfabeto.empty() || !tieneCaracteresUnicos(alfabeto)) {
        cout << "El alfabeto no puede estar vacio ni contener caracteres repetidos.\n";
        return;
    }

    if (longitud <= 0) {
        cout << "La longitud debe ser mayor que cero.\n";
        return;
    }

    if (modo == ModoComparacion::TextoPlano) {
        cout << "Contrasena objetivo: ";
        cin >> objetivo;

        if (objetivo.size() != static_cast<size_t>(longitud)) {
            cout << "La contrasena objetivo debe tener la longitud indicada.\n";
            return;
        }
    } else {
        cout << "Hash SHA-256 objetivo (64 caracteres hexadecimales): ";
        cin >> objetivo;
        objetivo = normalizarHash(objetivo);

        if (!esHashSha256Valido(objetivo)) {
            cout << "El hash SHA-256 no tiene un formato valido.\n";
            return;
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

void opcionDiccionario() {
    cout << "Hash SHA-256 objetivo (64 caracteres hexadecimales): ";
    string objetivo;
    cin >> objetivo;
    objetivo = normalizarHash(objetivo);

    if (!esHashSha256Valido(objetivo)) {
        cout << "El hash SHA-256 no tiene un formato valido.\n";
        return;
    }

    const vector<string> palabras = cargarDiccionario(RUTA_DICCIONARIO_DEFECTO);
    if (palabras.empty()) {
        cout << "No se pudo abrir " << RUTA_DICCIONARIO_DEFECTO
             << " (o esta vacio). Verifique que existe y que el programa se"
             << " ejecuta desde la carpeta ada_p1/.\n";
        return;
    }

    const auto inicio = steady_clock::now();
    const ResultadoBusqueda resultado = ataquePorDiccionario(palabras, objetivo, ModoComparacion::Sha256);
    const auto fin = steady_clock::now();
    const auto duracion = duration_cast<microseconds>(fin - inicio);

    if (resultado.encontrada) {
        cout << "La contrasena es: " << resultado.candidata << '\n';
    } else {
        cout << "No se encontro la contrasena entre las " << palabras.size()
             << " palabras del diccionario (" << RUTA_DICCIONARIO_DEFECTO << ").\n"
             << "Esto NO significa que la contrasena no exista: el diccionario es una"
             << " lista finita de candidatos plausibles, no una enumeracion exhaustiva"
             << " del espacio de busqueda.\n";
    }

    cout << "Intentos realizados: " << resultado.intentos << '\n'
         << fixed << setprecision(3)
         << "Tiempo de ejecucion: " << duracion.count() / 1000.0 << " ms"
         << " (" << duracion.count() << " microsegundos)\n";
}

void opcionComparacionSeccion81() {
    cout << "Alfabeto (para la fuerza bruta pura): ";
    string alfabeto;
    cin >> alfabeto;
    cout << "Longitud de la contrasena: ";
    int longitud = 0;
    cin >> longitud;
    cout << "Hash SHA-256 objetivo (64 caracteres hexadecimales): ";
    string objetivo;
    cin >> objetivo;
    objetivo = normalizarHash(objetivo);

    if (alfabeto.empty() || !tieneCaracteresUnicos(alfabeto)) {
        cout << "El alfabeto no puede estar vacio ni contener caracteres repetidos.\n";
        return;
    }

    if (longitud <= 0) {
        cout << "La longitud debe ser mayor que cero.\n";
        return;
    }

    if (!esHashSha256Valido(objetivo)) {
        cout << "El hash SHA-256 no tiene un formato valido.\n";
        return;
    }

    const vector<string> palabras = cargarDiccionario(RUTA_DICCIONARIO_DEFECTO);
    if (palabras.empty()) {
        cout << "No se pudo abrir " << RUTA_DICCIONARIO_DEFECTO
             << " (o esta vacio). Verifique que existe y que el programa se"
             << " ejecuta desde la carpeta ada_p1/.\n";
        return;
    }

    cout << "\n=== Comparacion: fuerza bruta pura vs. ataque por diccionario (Seccion 8.1) ===\n";

    const auto inicioDic = steady_clock::now();
    const ResultadoBusqueda resultadoDic = ataquePorDiccionario(palabras, objetivo, ModoComparacion::Sha256);
    const auto finDic = steady_clock::now();
    imprimirResultado("Diccionario  ", resultadoDic, duration_cast<microseconds>(finDic - inicioDic).count());

    const auto inicioFB = steady_clock::now();
    const ResultadoBusqueda resultadoFB = fuerzaBrutaSilenciosa(alfabeto, longitud, objetivo, ModoComparacion::Sha256);
    const auto finFB = steady_clock::now();
    imprimirResultado("Fuerza bruta ", resultadoFB, duration_cast<microseconds>(finFB - inicioFB).count());

    cout << "\nNota: el ataque por diccionario solo prueba las " << palabras.size()
         << " palabras de " << RUTA_DICCIONARIO_DEFECTO << " -- no es exhaustivo. Si la"
         << " contrasena no esta en esa lista, el diccionario nunca la va a encontrar,"
         << " sin importar el hardware. La fuerza bruta cubre el espacio completo"
         << " (|alfabeto|^longitud) y garantiza encontrarla tarde o temprano, a costa de"
         << " mucho mas tiempo/intentos a medida que crece el espacio de busqueda.\n";
}

void ejecutarMenu() {
    while (true) {
        cout << "\n=== FUERZA BRUTA ===\n"
             << "1. Comparacion directa con texto plano\n"
             << "2. Comparacion mediante SHA-256\n"
             << "3. Ataque por diccionario (SHA-256)\n"
             << "4. Comparacion fuerza bruta vs. diccionario (Seccion 8.1)\n"
             << "5. Volver al menu principal\n"
             << "Seleccione una opcion: ";

        int opcion = 0;
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Debe ingresar un numero.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                opcionTextoPlanoOSha256(ModoComparacion::TextoPlano);
                break;
            case 2:
                opcionTextoPlanoOSha256(ModoComparacion::Sha256);
                break;
            case 3:
                opcionDiccionario();
                break;
            case 4:
                opcionComparacionSeccion81();
                break;
            case 5:
                cout << "Volviendo al menu principal.\n";
                return;
            default:
                cout << "Opcion invalida.\n";
                break;
        }
    }
}

} // namespace fb
