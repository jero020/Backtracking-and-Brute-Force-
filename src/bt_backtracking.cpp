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
// Motor de busqueda con poda y version sin poda (Seccion 8.2)
//
// Este es el algoritmo real de Camila Garcia Ortiz para el modulo BT (ver
// archivo_referencia/BT/main.cpp: clase Estado, factibilidad(),
// esSolucion() y bt()), adaptado con asistencia de IA para poder
// parametrizarse por alfabeto/longitud/politica en vez de las 7
// instancias fijas del prototipo original, y para poder correrse tambien
// "sin poda" (necesario para la Seccion 8.2). La regla de factibilidad
// no cambio: sigue siendo una condicion necesaria pero no suficiente
// (cuenta minimos de tipo de caracter que faltan contra posiciones
// restantes, sin distinguir cuales posiciones concretas quedan libres).
// ---------------------------------------------------------------------
namespace detalle {

// Estado parcial de la busqueda: el prefijo construido hasta el momento
// y un conteo incremental de cuantos caracteres de cada tipo contiene,
// para no tener que re-escanear el prefijo completo en cada nodo.
class Estado {
private:
    string prefijo;
    int lower;
    int upper;
    int digit;
    int symbol;

public:
    Estado() : lower(0), upper(0), digit(0), symbol(0) {}

    const string& getPrefijo() const { return prefijo; }
    int getL() const { return lower; }
    int getU() const { return upper; }
    int getD() const { return digit; }
    int getS() const { return symbol; }

    void agregarC(char c) {
        prefijo += c;
        switch (tipoCaracter(c)) {
            case 0: lower++; break;
            case 1: upper++; break;
            case 2: digit++; break;
            default: symbol++; break;
        }
    }

    void quitarC() {
        char c = prefijo.back();
        prefijo.pop_back();
        switch (tipoCaracter(c)) {
            case 0: lower--; break;
            case 1: upper--; break;
            case 2: digit--; break;
            default: symbol--; break;
        }
    }
};

bool factibilidad(const Estado& estado, const Politica& pol, int n) {
    const string& prefijo = estado.getPrefijo();
    size_t k = prefijo.length();

    if (pol.prohibirRepetidosConsecutivos && k >= 2) {
        if (prefijo[k - 1] == prefijo[k - 2]) return false;
    }

    int restantes = n - static_cast<int>(k);
    int faltaLower = max(0, pol.minLower - estado.getL());
    int faltaUpper = max(0, pol.minUpper - estado.getU());
    int faltaDigit = max(0, pol.minDigit - estado.getD());
    int faltaSymbol = max(0, pol.minSymbol - estado.getS());

    return (faltaLower + faltaUpper + faltaDigit + faltaSymbol) <= restantes;
}

bool esSolucion(const Estado& estado, const Politica& pol, int n) {
    if (static_cast<int>(estado.getPrefijo().length()) != n) return false;

    return estado.getL() >= pol.minLower && estado.getU() >= pol.minUpper &&
           estado.getD() >= pol.minDigit && estado.getS() >= pol.minSymbol;
}

void bt(const string& alfabeto, int n, Estado& estado, const Politica& pol,
        ResultadoBT& res, int limiteEjemplos) {
    res.nodos++;

    if (static_cast<int>(estado.getPrefijo().length()) == n) {
        if (esSolucion(estado, pol, n)) {
            res.soluciones++;
            if (static_cast<int>(res.ejemplos.size()) < limiteEjemplos) {
                res.ejemplos.push_back(estado.getPrefijo());
            }
        }
        return;
    }

    for (char c : alfabeto) {
        estado.agregarC(c);

        if (factibilidad(estado, pol, n)) {
            bt(alfabeto, n, estado, pol, res, limiteEjemplos);
        } else {
            res.nodosPodados++;
        }

        estado.quitarC();
    }
}

// Igual que bt(), pero sin llamar nunca a factibilidad(): recorre
// Sigma^0..Sigma^n completo y solo filtra al llegar a una hoja. Por eso
// aqui si hay que volver a revisar la regla de "sin repetidos
// consecutivos" al final (bt() la aplicaba de una vez en cada paso, como
// parte de la poda; aqui nunca se aplico durante la construccion).
void btSinPoda(const string& alfabeto, int n, Estado& estado, const Politica& pol,
                ResultadoBT& res) {
    res.nodos++;

    if (static_cast<int>(estado.getPrefijo().length()) == n) {
        bool repetidoConsecutivo = false;
        if (pol.prohibirRepetidosConsecutivos) {
            const string& prefijo = estado.getPrefijo();
            for (size_t i = 1; i < prefijo.size(); ++i) {
                if (prefijo[i] == prefijo[i - 1]) {
                    repetidoConsecutivo = true;
                    break;
                }
            }
        }
        if (!repetidoConsecutivo && esSolucion(estado, pol, n)) {
            res.soluciones++;
        }
        return;
    }

    for (char c : alfabeto) {
        estado.agregarC(c);
        btSinPoda(alfabeto, n, estado, pol, res);
        estado.quitarC();
    }
}

} // namespace detalle

ResultadoBT generarConPoda(const Politica& pol, const string& alfabeto, int limiteEjemplos) {
    ResultadoBT resultado;
    detalle::Estado estado;
    detalle::bt(alfabeto, pol.longitud, estado, pol, resultado, limiteEjemplos);
    return resultado;
}

ResultadoBT generarSinPoda(const Politica& pol, const string& alfabeto) {
    ResultadoBT resultado;
    detalle::Estado estado;
    detalle::btSinPoda(alfabeto, pol.longitud, estado, pol, resultado);
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
    cout << "Nodos podados: " << r.nodosPodados << "\n";
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

    cout << "\nCon poda:  nodos visitados=" << conPoda.nodos << "  nodos podados=" << conPoda.nodosPodados
         << "  soluciones=" << conPoda.soluciones << "  tiempo=" << msConPoda << " ms\n";
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
