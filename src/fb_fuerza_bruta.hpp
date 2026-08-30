#ifndef FB_FUERZA_BRUTA_HPP
#define FB_FUERZA_BRUTA_HPP

#include <string>
#include <vector>

// Modulo de Fuerza Bruta (FB), implementacion original de Javier Andres
// Sierra Machado (ver FB/main.cpp). Este archivo solo declara las mismas
// funciones para que src/main.cpp pueda enlazarlas dentro del binario
// unico ada_p1 que exige la Seccion 11 del enunciado
// (g++ -std=c++17 -O2 -o ada_p1 src/*.cpp): el antiguo `int main()` de
// FB/main.cpp se volvio fb::ejecutarMenu(), y el resto del codigo (logica
// y comentarios) se mantuvo igual en fb_fuerza_bruta.cpp.
//
// El ataque por diccionario (Seccion 8.1) se agrego el 29-ago-2026 con
// asistencia parcial de IA (Claude), revisado por Jeronimo Velez Acosta.
// Ver ../README.md para el detalle de por que existe esta carpeta ademas
// de FB/ y BT/.

namespace fb {

enum class ModoComparacion {
    TextoPlano = 1,
    Sha256 = 2
};

// Ruta por defecto del diccionario, relativa al directorio desde el que se
// ejecuta el binario (se espera correr "./ada_p1" desde la carpeta ada_p1/).
extern const std::string RUTA_DICCIONARIO_DEFECTO;

struct ResultadoBusqueda {
    bool encontrada = false;
    std::string candidata;
    unsigned long long intentos = 0;
};

std::string calcularSha256(const std::string& texto);
bool tieneCaracteresUnicos(const std::string& alfabeto);
bool esHashSha256Valido(const std::string& hash);
std::string normalizarHash(std::string hash);

bool generarCombinaciones(const std::string& alfabeto,
                          int longitud,
                          const std::string& objetivo,
                          ModoComparacion modo,
                          unsigned long long& intentos);

ResultadoBusqueda fuerzaBrutaSilenciosa(const std::string& alfabeto,
                                        int longitud,
                                        const std::string& objetivo,
                                        ModoComparacion modo);

std::vector<std::string> cargarDiccionario(const std::string& ruta);

ResultadoBusqueda ataquePorDiccionario(const std::vector<std::string>& palabras,
                                       const std::string& objetivo,
                                       ModoComparacion modo);

// Menu interactivo del modulo FB (equivalente al main() de FB/main.cpp,
// adaptado para devolver el control al menu principal de ada_p1 en vez de
// terminar el programa). Se invoca desde src/main.cpp.
void ejecutarMenu();

} // namespace fb

#endif // FB_FUERZA_BRUTA_HPP
