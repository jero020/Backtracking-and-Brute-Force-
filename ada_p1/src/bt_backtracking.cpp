#include "bt_backtracking.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>

using namespace std;
using namespace chrono;

// ---------------------------------------------------------------------
// Semilla del equipo (Seccion 9.1 / 9.2)
// ---------------------------------------------------------------------
namespace semilla {

string normalizarApellido(const string& s) {
    string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if (c == ' ') continue;
        out += static_cast<char>(tolower(c));
    }
    return out;
}

long long calcularSemilla(vector<string> apellidos) {
    for (auto& a : apellidos) a = normalizarApellido(a);
    sort(apellidos.begin(), apellidos.end());

    string concatenado;
    for (const auto& a : apellidos) concatenado += a;

    long long suma = 0;
    for (unsigned char c : concatenado) suma += c;

    return suma % 100000;
}

vector<uint32_t> generarSecuenciaLCG(long long semillaInicial, size_t cantidad) {
    vector<uint32_t> xs;
    xs.reserve(cantidad);
    uint64_t x = static_cast<uint64_t>(semillaInicial);
    xs.push_back(static_cast<uint32_t>(x));
    for (size_t i = 1; i < cantidad; ++i) {
        x = (1103515245ULL * x + 12345ULL) % (1ULL << 31);
        xs.push_back(static_cast<uint32_t>(x));
    }
    return xs;
}

} // namespace semilla

// ---------------------------------------------------------------------
// Politica de contrasenas (Seccion 9.2)
// ---------------------------------------------------------------------
namespace bt {

const string MINUSCULAS = "abcdefghijklmnopqrstuvwxyz";
const string MAYUSCULAS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string DIGITOS    = "0123456789";
const string SIMBOLOS   = "!@#$%";
const string ALFABETO_BASE = MINUSCULAS + MAYUSCULAS + DIGITOS + SIMBOLOS;

Politica derivarPolitica(long long semillaEquipo, int longitud, bool& ajustada) {
    Politica p;
    p.longitud = longitud;
    p.minLower = 2 + static_cast<int>(semillaEquipo % 3);
    p.minUpper = 1 + static_cast<int>(semillaEquipo % 2);
    p.minDigit = 1 + static_cast<int>(semillaEquipo % 3);
    p.minSymbol = 1;
    p.prohibirRepetidosConsecutivos = true;

    ajustada = false;
    int suma = p.minLower + p.minUpper + p.minDigit + p.minSymbol;
    if (suma > p.longitud) {
        p.minLower = max(0, p.minLower - (suma - p.longitud));
        ajustada = true;
    }
    return p;
}

int tipoCaracter(char c) {
    if (MINUSCULAS.find(c) != string::npos) return 0;
    if (MAYUSCULAS.find(c) != string::npos) return 1;
    if (DIGITOS.find(c) != string::npos) return 2;
    return 3;
}

// ---------------------------------------------------------------------
// Backtracking con poda y version sin poda (Seccion 8.2)
// ---------------------------------------------------------------------
void actualizarConteo(EstadoConteo& c, char ch) {
    switch (tipoCaracter(ch)) {
        case 0: c.lower++; break;
        case 1: c.upper++; break;
        case 2: c.digit++; break;
        default: c.symbol++; break;
    }
}

bool esFactible(const Politica& pol, const EstadoConteo& c, int posicionesRestantes) {
    int faltanLower  = max(0, pol.minLower  - c.lower);
    int faltanUpper  = max(0, pol.minUpper  - c.upper);
    int faltanDigit  = max(0, pol.minDigit  - c.digit);
    int faltanSymbol = max(0, pol.minSymbol - c.symbol);
    return (faltanLower + faltanUpper + faltanDigit + faltanSymbol) <= posicionesRestantes;
}

bool cumplePolitica(const Politica& pol, const EstadoConteo& c) {
    return c.lower >= pol.minLower && c.upper >= pol.minUpper && c.digit >= pol.minDigit &&
           c.symbol >= pol.minSymbol;
}

namespace detalle {

void backtrackConPoda(const Politica& pol, const string& alfabeto, string& prefijo,
                       EstadoConteo conteo, char ultimo, ResultadoBT& resultado,
                       int limiteEjemplos) {
    resultado.nodos++;

    if (static_cast<int>(prefijo.size()) == pol.longitud) {
        if (cumplePolitica(pol, conteo)) {
            resultado.soluciones++;
            if (static_cast<int>(resultado.ejemplos.size()) < limiteEjemplos) {
                resultado.ejemplos.push_back(prefijo);
            }
        }
        return;
    }

    int restantesTrasEste = pol.longitud - static_cast<int>(prefijo.size()) - 1;

    for (char c : alfabeto) {
        if (pol.prohibirRepetidosConsecutivos && c == ultimo) continue; // poda: regla local

        EstadoConteo nuevoConteo = conteo;
        actualizarConteo(nuevoConteo, c);

        if (!esFactible(pol, nuevoConteo, restantesTrasEste)) continue; // poda: factibilidad

        prefijo.push_back(c);
        backtrackConPoda(pol, alfabeto, prefijo, nuevoConteo, c, resultado, limiteEjemplos);
        prefijo.pop_back();
    }
}

void backtrackSinPoda(const Politica& pol, const string& alfabeto, string& prefijo,
                       ResultadoBT& resultado) {
    resultado.nodos++;

    if (static_cast<int>(prefijo.size()) == pol.longitud) {
        EstadoConteo conteo;
        bool repetidoConsecutivo = false;
        for (size_t i = 0; i < prefijo.size(); ++i) {
            actualizarConteo(conteo, prefijo[i]);
            if (i > 0 && pol.prohibirRepetidosConsecutivos && prefijo[i] == prefijo[i - 1]) {
                repetidoConsecutivo = true;
            }
        }
        if (!repetidoConsecutivo && cumplePolitica(pol, conteo)) {
            resultado.soluciones++;
        }
        return;
    }

    for (char c : alfabeto) {
        prefijo.push_back(c);
        backtrackSinPoda(pol, alfabeto, prefijo, resultado);
        prefijo.pop_back();
    }
}

} // namespace detalle

ResultadoBT generarConPoda(const Politica& pol, const string& alfabeto, int limiteEjemplos) {
    ResultadoBT resultado;
    string prefijo;
    prefijo.reserve(static_cast<size_t>(pol.longitud));
    EstadoConteo conteo;
    detalle::backtrackConPoda(pol, alfabeto, prefijo, conteo, '\0', resultado, limiteEjemplos);
    return resultado;
}

ResultadoBT generarSinPoda(const Politica& pol, const string& alfabeto) {
    ResultadoBT resultado;
    string prefijo;
    prefijo.reserve(static_cast<size_t>(pol.longitud));
    detalle::backtrackSinPoda(pol, alfabeto, prefijo, resultado);
    return resultado;
}

} // namespace bt

// ---------------------------------------------------------------------
// Menu interactivo
// ---------------------------------------------------------------------
namespace {

void imprimirPolitica(const bt::Politica& p, bool ajustada) {
    cout << "n=" << p.longitud << " minLower=" << p.minLower << " minUpper=" << p.minUpper
         << " minDigit=" << p.minDigit << " minSymbol=" << p.minSymbol
         << " sinRepetidosConsecutivos=" << (p.prohibirRepetidosConsecutivos ? "si" : "no");
    if (ajustada) cout << "  [AJUSTADA: minLower se redujo porque la suma de minimos > n]";
    cout << "\n";
}

vector<string> pedirApellidos() {
    cout << "Ingrese los 3 apellidos del equipo separados por espacio (sin tildes): ";
    vector<string> apellidos;
    for (int i = 0; i < 3; ++i) {
        string a;
        cin >> a;
        apellidos.push_back(a);
    }
    return apellidos;
}

void opcionReferencia() {
    cout << "\n=== Instancia de referencia BT (n=6, minLower=2, minUpper=1, minDigit=1, "
            "minSymbol=1, alfabeto completo de 67 simbolos) ===\n";
    cout << "AVISO: esta instancia deja muy poco margen (1 caracter libre de 6), asi que la "
            "poda casi no actua hasta el final y puede tardar varios minutos. No esta "
            "colgado -- para una verificacion rapida usen antes la opcion 4 con un alfabeto "
            "reducido (por ejemplo: tamAlfabeto=8, n=6, minLower=2, minUpper=1, minDigit=1, "
            "minSymbol=1).\n"
         << flush;

    bt::Politica p;
    p.longitud = 6;
    p.minLower = 2;
    p.minUpper = 1;
    p.minDigit = 1;
    p.minSymbol = 1;

    auto inicio = steady_clock::now();
    auto r = bt::generarConPoda(p, bt::ALFABETO_BASE, 5);
    auto fin = steady_clock::now();
    auto ms = duration_cast<milliseconds>(fin - inicio).count();

    cout << "Nodos visitados: " << r.nodos << "\n";
    cout << "Soluciones encontradas: " << r.soluciones << "\n";
    cout << "Tiempo: " << ms << " ms\n";
    cout << "Ejemplos: ";
    for (auto& e : r.ejemplos) cout << e << " ";
    cout << "\n";
    cout << (r.soluciones > 0 ? "OK: la implementacion genera soluciones validas.\n"
                               : "ERROR: no se genero ninguna solucion.\n");
}

void opcionSemilla() {
    auto apellidos = pedirApellidos();
    long long s = semilla::calcularSemilla(apellidos);
    cout << "Semilla del equipo = " << s << "\n";
    bool ajustada = false;
    auto pol = bt::derivarPolitica(s, 8, ajustada);
    cout << "Politica del equipo (n=8): ";
    imprimirPolitica(pol, ajustada);
}

void opcionVariantes() {
    cout << "AVISO: varias de estas 5 variantes corren sobre el alfabeto completo (67 "
            "simbolos) con margenes muy ajustados entre requisitos y longitud, asi que la "
            "poda actua tarde y algunas -- sobre todo (i) n=8 y (v) sin restricciones -- "
            "pueden tardar bastante (minutos) segun el computador. No es un error; es el "
            "mismo fenomeno que la instancia de referencia (ver opcion 1). Dejenlo corriendo "
            "con tiempo de sobra en vez de interrumpirlo.\n"
         << flush;
    auto apellidos = pedirApellidos();
    long long s = semilla::calcularSemilla(apellidos);

    bool a1 = false, a2 = false, a3 = false;
    bt::Politica v1 = bt::derivarPolitica(s, 8, a1);   // (i) completa, n=8
    bt::Politica v2 = bt::derivarPolitica(s, 6, a2);   // (ii) misma politica, n=6
    bt::Politica v3 = bt::derivarPolitica(s, 10, a3);  // (iii) misma politica, n=10

    bt::Politica v4; // (iv) relajada: solo minLower=1, n=8
    v4.longitud = 8;
    v4.minLower = 1;

    bt::Politica v5; // (v) sin restricciones de composicion, n=6 (poda nula)
    v5.longitud = 6;

    struct Variante { const char* nombre; bt::Politica pol; };
    vector<Variante> variantes = {
        {"i_completa_n8", v1}, {"ii_completa_n6", v2}, {"iii_completa_n10", v3},
        {"iv_relajada_n8", v4}, {"v_sin_restricciones_n6", v5},
    };

    ofstream out("results/bt_variantes.csv");
    out << "variante,n,minLower,minUpper,minDigit,minSymbol,nodos_visitados,soluciones,"
           "tiempo_ms\n";

    for (const auto& v : variantes) {
        cout << "Corriendo variante " << v.nombre << " ... " << flush;
        if (string(v.nombre) == "v_sin_restricciones_n6") {
            cout << "\n  AVISO: esta variante casi no tiene poda real, puede tardar mucho. "
                    "Se deja corriendo; si no termina, denle mas tiempo aparte.\n  ";
        }
        auto inicio = steady_clock::now();
        auto r = bt::generarConPoda(v.pol, bt::ALFABETO_BASE, 3);
        auto fin = steady_clock::now();
        auto ms = duration_cast<milliseconds>(fin - inicio).count();

        cout << "nodos=" << r.nodos << " soluciones=" << r.soluciones << " tiempo=" << ms
             << " ms\n";
        out << v.nombre << "," << v.pol.longitud << "," << v.pol.minLower << "," << v.pol.minUpper
            << "," << v.pol.minDigit << "," << v.pol.minSymbol << "," << r.nodos << ","
            << r.soluciones << "," << ms << "\n";
    }
    cout << "\nGuardado en results/bt_variantes.csv\n";
}

void opcionComparacion() {
    cout << "Tamano del alfabeto reducido (1-" << bt::ALFABETO_BASE.size() << "): ";
    int tamAlfabeto;
    cin >> tamAlfabeto;
    if (tamAlfabeto < 1 || tamAlfabeto > static_cast<int>(bt::ALFABETO_BASE.size())) {
        cout << "Valor invalido.\n";
        return;
    }
    int n, minLower, minUpper, minDigit, minSymbol;
    cout << "Longitud n: ";
    cin >> n;
    cout << "minLower minUpper minDigit minSymbol: ";
    cin >> minLower >> minUpper >> minDigit >> minSymbol;

    string alfabetoReducido = bt::ALFABETO_BASE.substr(0, static_cast<size_t>(tamAlfabeto));

    bt::Politica p;
    p.longitud = n;
    p.minLower = minLower;
    p.minUpper = minUpper;
    p.minDigit = minDigit;
    p.minSymbol = minSymbol;

    cout << "Alfabeto reducido (" << tamAlfabeto << " simbolos): " << alfabetoReducido << "\n";
    imprimirPolitica(p, false);

    auto t0 = steady_clock::now();
    auto conPoda = bt::generarConPoda(p, alfabetoReducido);
    auto t1 = steady_clock::now();
    auto sinPoda = bt::generarSinPoda(p, alfabetoReducido);
    auto t2 = steady_clock::now();

    auto msConPoda = duration_cast<milliseconds>(t1 - t0).count();
    auto msSinPoda = duration_cast<milliseconds>(t2 - t1).count();

    double reduccion = sinPoda.nodos > 0
                            ? 100.0 * (1.0 - static_cast<double>(conPoda.nodos) /
                                                  static_cast<double>(sinPoda.nodos))
                            : 0.0;

    cout << "\nCon poda:  nodos visitados=" << conPoda.nodos << "  soluciones=" << conPoda.soluciones
         << "  tiempo=" << msConPoda << " ms\n";
    cout << "Sin poda:  nodos generados=" << sinPoda.nodos << "  soluciones=" << sinPoda.soluciones
         << "  tiempo=" << msSinPoda << " ms\n";
    cout << "Reduccion del espacio de busqueda: " << reduccion << "%\n";
    cout << (conPoda.soluciones == sinPoda.soluciones
                 ? "OK: el numero de soluciones coincide entre ambas versiones.\n"
                 : "ERROR: el numero de soluciones NO coincide.\n");
}

} // namespace

namespace bt {

void ejecutarMenu() {
    while (true) {
        cout << "\n=== BACKTRACKING ===\n"
             << "1. Validar instancia de referencia comun\n"
             << "2. Calcular semilla y politica del equipo\n"
             << "3. Generar y correr las 5 variantes (Seccion 9.2)\n"
             << "4. Comparar con poda vs. sin poda (alfabeto reducido)\n"
             << "5. Volver al menu principal\n"
             << "Seleccione una opcion: ";

        int opcion = 0;
        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Entrada invalida. Debe ingresar un numero.\n";
            continue;
        }

        if (opcion == 5) {
            cout << "Volviendo al menu principal.\n";
            break;
        }

        switch (opcion) {
            case 1: opcionReferencia(); break;
            case 2: opcionSemilla(); break;
            case 3: opcionVariantes(); break;
            case 4: opcionComparacion(); break;
            default: cout << "Opcion invalida.\n"; break;
        }
    }
}

} // namespace bt
