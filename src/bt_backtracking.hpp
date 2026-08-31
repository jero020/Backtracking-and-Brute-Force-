#ifndef BT_BACKTRACKING_HPP
#define BT_BACKTRACKING_HPP

#include <cstdint>
#include <string>
#include <vector>

// Modulo de Backtracking (BT). El motor de busqueda de este archivo (clase
// Estado + factibilidad() + esSolucion() + bt(), ver bt_backtracking.cpp)
// es el algoritmo que entrego Camila Garcia Ortiz como prototipo del
// modulo BT (prototipo original preservado en el historial de git, commit
// a0da07b; conversaciones de apoyo en report/conversaciones_ia_bt.md).
// Con asistencia de IA (Claude, 30-ago-2026) se adapto para poder
// parametrizarse por alfabeto/longitud/politica desde el menu interactivo
// de ada_p1 en vez de las 7 instancias fijas del prototipo original, y
// para exponer una version "sin poda" que permite la comparacion de
// correctitud de la Seccion 8.2. La logica de poda (factibilidad) no
// cambio frente al prototipo de Camila. Antes de esta integracion,
// ada_p1 corria un algoritmo de respaldo distinto escrito por Jeronimo
// Velez Acosta (ver Seccion 17 del informe para el detalle).
//
// Ademas se conserva aqui la logica de semilla/politica del equipo
// (namespace semilla, y bt::derivarPolitica), que es infraestructura
// compartida independiente del motor de busqueda.

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

struct ResultadoBT {
    long long nodos = 0;         // con poda: visitados; sin poda: generados
    long long nodosPodados = 0;  // solo con poda: nodos descartados por factibilidad()
    long long soluciones = 0;
    std::vector<std::string> ejemplos;
};

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
