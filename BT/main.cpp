#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;
using namespace chrono;

class Estado
{
private:
    string prefijo;
    int lower;
    int upper;
    int digit;
    int symbol;

    int charType(char c)
    {
        unsigned char uc = static_cast<unsigned char>(c);

        if (islower(uc))
            return 0;

        if (isupper(uc))
            return 1;

        if (isdigit(uc))
            return 2;

        return 3;
    }

public:
    Estado()
    {
        prefijo = "";
        lower = 0;
        upper = 0;
        digit = 0;
        symbol = 0;
    }

    string getPrefijo() const { return prefijo; }
    int getL() const { return lower; }
    int getU() const { return upper; }
    int getD() const { return digit; }
    int getS() const { return symbol; }

    void agregarC(char c)
    {
        prefijo += c;
        switch (charType(c))
        {
        case 0:
            lower++;
            break;
        case 1:
            upper++;
            break;
        case 2:
            digit++;
            break;
        case 3:
            symbol++;
            break;
        }
    }
    void quitarC()
    {
        char c = prefijo.back();
        prefijo.pop_back();
        switch (charType(c))
        {
        case 0:
            lower--;
            break;
        case 1:
            upper--;
            break;
        case 2:
            digit--;
            break;
        case 3:
            symbol--;
            break;
        }
    }
};

struct Politica
{
    int minLower;
    int minUpper;
    int minDigit;
    int minSymbol;
    bool noConsecutivos;
};

struct Resultados
{
    long long nodosVisitados;
    long long solucionesCantidad;
    long long nodosPodados;
    bool primera;
    string ejemplo;
};

bool factibilidad(const Estado &estado, const Politica &pol, size_t n)
{
    const string &prefijo = estado.getPrefijo();
    size_t k = prefijo.length();

    if (pol.noConsecutivos && k >= 2)
    {
        if (prefijo[k - 1] == prefijo[k - 2])
            return false;
    }
    int restantes = n - k;
    int faltaLower = max(0, pol.minLower - estado.getL());
    int faltaUpper = max(0, pol.minUpper - estado.getU());
    int faltaDigit = max(0, pol.minDigit - estado.getD());
    int faltaSymbol = max(0, pol.minSymbol - estado.getS());

    int caracteresNecesarios =
        faltaLower + faltaUpper + faltaDigit + faltaSymbol;

    if (caracteresNecesarios > restantes)
    {
        return false;
    }
    return true;
}

bool esSolucion(const Estado &estado, const Politica &pol, size_t n)
{
    if (estado.getPrefijo().length() != n)
        return false;

    return estado.getL() >= pol.minLower &&
           estado.getU() >= pol.minUpper &&
           estado.getD() >= pol.minDigit &&
           estado.getS() >= pol.minSymbol;
}

void bt(const string &abc, size_t n, Estado &estado,
        const Politica &pol, Resultados &res)
{
    res.nodosVisitados++;

    if (estado.getPrefijo().length() == n)
    {
        if (esSolucion(estado, pol, n))
        {
            res.solucionesCantidad++;

            // Solo imprimir la primera solución encontrada
            if (!res.primera)
            {
                res.ejemplo = estado.getPrefijo();

                res.primera = true;
            }
        }

        return;
    }

    // Backtracking completo
    for (char c : abc)
    {
        estado.agregarC(c);

        if (factibilidad(estado, pol, n))
        {
            bt(abc, n, estado, pol, res);
        }
        else
        {
            res.nodosPodados++;
        }

        estado.quitarC();
    }
}

int main()
{
    const string ABC_TEST = "aA1!";

    const string ABC =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%";

    // Políticas
    Politica test{1, 1, 1, 1, true};

    Politica pol;
    pol.minLower = 2;
    pol.minUpper = 2;
    pol.minDigit = 3;
    pol.minSymbol = 1;
    pol.noConsecutivos = true;

    Politica relajada;
    relajada.minLower = 1;
    relajada.minUpper = 0;
    relajada.minDigit = 0;
    relajada.minSymbol = 0;
    relajada.noConsecutivos = false;

    Politica sinRestricciones;
    sinRestricciones.minLower = 0;
    sinRestricciones.minUpper = 0;
    sinRestricciones.minDigit = 0;
    sinRestricciones.minSymbol = 0;
    sinRestricciones.noConsecutivos = false;

    Politica comun;
    comun.minLower = 2;
    comun.minUpper = 1;
    comun.minDigit = 1;
    comun.minSymbol = 1;
    comun.noConsecutivos = true;

    int opcion;

    cout << "1. Instancia de prueba (n=4)\n";
    cout << "2. Instancia I (n=8, politica original)\n";
    cout << "3. Instancia II (n=6, politica original)\n";
    cout << "4. Instancia III (n=10, politica original)\n";
    cout << "5. Instancia IV (n=8, politica relajada)\n";
    cout << "6. Instancia V (n=6, sin restricciones)\n";
    cout << "7. Instancia comun (n=6)\n";
    cout << "0. Salir\n";
    cout << "Seleccione una opcion: ";

    cin >> opcion;

    Estado estado;

    Resultados res;
    res.solucionesCantidad = 0;
    res.nodosVisitados = 0;
    res.nodosPodados = 0;
    res.primera = false;

    int n = 0;
    const string *alfabeto = nullptr;
    const Politica *politica = nullptr;

    switch (opcion)
    {
    case 1:
        alfabeto = &ABC_TEST;
        n = 4;
        politica = &test;
        break;

    case 2:
        alfabeto = &ABC;
        n = 8;
        politica = &pol;
        break;

    case 3:
        alfabeto = &ABC;
        n = 6;
        politica = &pol;
        break;

    case 4:
        alfabeto = &ABC;
        n = 10;
        politica = &pol;
        break;

    case 5:
        alfabeto = &ABC;
        n = 8;
        politica = &relajada;
        break;

    case 6:
        alfabeto = &ABC;
        n = 6;
        politica = &sinRestricciones;
        break;

    case 7:
        alfabeto = &ABC;
        n = 6;
        politica = &comun;
        break;

    case 0:
        cout << "Programa finalizado.\n";
        return 0;

    default:
        cout << "Opcion invalida.\n";
        return 1;
    }

    cout << "\nEjecutando instancia...\n";

    auto inicio = high_resolution_clock::now();

    bt(*alfabeto, n, estado, *politica, res);

    auto fin = high_resolution_clock::now();

    auto duracion = duration_cast<microseconds>(fin - inicio);

    cout << "Resultados: \n";

    if (res.primera)
    {
        cout << "Ejemplo de solucion valida: "
             << res.ejemplo << endl;
    }
    else
    {
        cout << "No se encontro ninguna solucion valida." << endl;
    }

    cout << "Numero de soluciones: "
         << res.solucionesCantidad << endl;

    cout << "Nodos visitados: "
         << res.nodosVisitados << endl;

    cout << "Nodos podados: "
         << res.nodosPodados << endl;

    cout << "Tiempo de ejecucion: "
         << duracion.count()
         << " microsegundos" << endl;

    return 0;
}
