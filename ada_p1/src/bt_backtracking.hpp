#ifndef BT_BACKTRACKING_HPP
#define BT_BACKTRACKING_HPP

#include <cstdint>
#include <string>
#include <vector>

// Modulo de Backtracking (BT). Extraido de BT/main.cpp (implementacion de
// contingencia de Jeronimo Velez Acosta, ver BT/README.md sobre por que
// existe) para poder compilarse como parte del binario unico ada_p1 que
// exige la Seccion 11 del enunciado
// (g++ -std=c++17 -O2 -o ada_p1 src/main.cpp src/*.cpp). La logica
// algoritmica no cambio: solo se separo el antiguo `int main()` de
// BT/main.cpp en las funciones de esta cabecera + bt_backtracking.cpp, y
// se expuso bt::ejecutarMenu() para que src/main.cpp pueda invocarla.

namespace semilla {

std::string normalizarApellido(const std::string& s);
long long calcularSemilla(std::vector<std::string> apellidos);
std::vector<uint32_t> generarSecuenciaLCG(long long semillaInicial, size_t cantidad);

} // namespace semilla

namespace bt {

extern const std::string MINUSCULAS;
extern const std::string MAYUSCULAS;
extern const std::string DIGITOS;
extern const std::string SIMBOLOS;
extern const std::string ALFABETO_BASE;
// NOTA: el enunciado dice "69 simbolos en total"; minusculas(26) +
// mayusculas(26) + digitos(10) + simbolos(5) = 67. Se implementa lo
// descrito literalmente (67); confirmar con el docente si faltan 2.

struct Politica {
    int minLower = 0;
    int minUpper = 0;
    int minDigit = 0;
    int minSymbol = 0;
    bool prohibirRepetidosConsecutivos = true;
    int longitud = 8;
};

Politica derivarPolitica(long long semillaEquipo, int longitud, bool& ajustada);
int tipoCaracter(char c);

struct EstadoConteo {
    int lower = 0, upper = 0, digit = 0, symbol = 0;
};

struct ResultadoBT {
    long long nodos = 0;      // con poda: visitados; sin poda: generados
    long long soluciones = 0;
    std::vector<std::string> ejemplos;
};

void actualizarConteo(EstadoConteo& c, char ch);
bool esFactible(const Politica& pol, const EstadoConteo& c, int posicionesRestantes);
bool cumplePolitica(const Politica& pol, const EstadoConteo& c);

ResultadoBT generarConPoda(const Politica& pol, const std::string& alfabeto, int limiteEjemplos = 10);

// ADVERTENCIA: sin ninguna poda. Genera literalmente Sigma^0..Sigma^n. Para
// el alfabeto/longitud reales de esta practica (67 simbolos, n=8..10) es
// INTRATABLE. Usenla solo con alfabetos reducidos y n pequeno (opcion 4 del
// menu) para validar que el conteo de soluciones coincide con la version
// con poda -- ese es el chequeo de correctitud de la Seccion 8.2.
ResultadoBT generarSinPoda(const Politica& pol, const std::string& alfabeto);

// Menu interactivo del modulo BT (antes era el main() independiente de
// BT/main.cpp). Se invoca desde src/main.cpp.
void ejecutarMenu();

} // namespace bt

#endif // BT_BACKTRACKING_HPP
