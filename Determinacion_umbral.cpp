#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <limits>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

using namespace std;

// ESTRUCTURAS.

struct Point
{
    int x, y;
};

// PROTOTIPOS.

vector<Point> generarPuntos(int n, int maxCoordenada = 10000);
float dist(Point p1, Point p2);
float min(float x, float y);

// Fuerza Bruta.
float bruteForce(Point P[], int n);

// Divide y vencerás.
int compareX(const void *a, const void *b);
int compareY(const void *a, const void *b);
float stripClosest(Point strip[], int size, float d);
float closestUtil(Point P[], int n);
float closest(Point P[], int n);
float combinar(Point P[], int n, Point midPoint, float dl, float dr);

float medirCombinacion(const vector<Point> &puntos, float &tiempoNs);

void regresionLineal(const vector<float> &x, const vector<float> &y, float &intercepto, float &pendiente, float &r2);

// MAIN

int main()
{
    const int nInicial = 10;
    const int nFinal = 1000;
    const int incremento = 10;
    const int repeticiones = 100;
    const int maxCoordenada = 10000;

    // Crear el archivo de resultados.
    ofstream archivo("resultados.csv");

    if (!archivo.is_open())
    {
        cerr << "Error al crear resultados.csv\n";
        return 1;
    }

    // Encabezados del CSV.
    archivo << "n,repeticion,pares_examinados,"
            << "tiempo_fb_ns,tiempo_dv_ns,"
            << "distancia_fb,distancia_dv\n";

    archivo << setprecision(12);

    cout << fixed << setprecision(6);
    cout << "\nCOMPARACION: FUERZA BRUTA VS. DIVIDE Y VENCERAS\n\n";

    cout << left
         << setw(10) << "n"
         << setw(20) << "Pares examinados"
         << setw(22) << "FB promedio (ns)"
         << setw(22) << "DV promedio (ns)"
         << setw(18) << "Resultados"
         << '\n';

    cout << string(92, '-') << '\n';

    vector<float> valoresN;
    vector<float> promediosH;
    vector<float> promediosG;

    // Probar distintos tamaños de entrada.
    for (int n = nInicial; n <= nFinal; n += incremento)
    {
        float sumaTiempoH = 0.0;
        float sumaTiempoG = 0.0;
        bool resultadosCorrectos = true;

        // Repetir el experimento.
        for (int k = 1; k <= repeticiones; k++)
        {
            // Generar una entrada común para ambos algoritmos.
            vector<Point> puntos = generarPuntos(n, maxCoordenada);



            // Copias independientes, fuera de la medición.
            vector<Point> puntosH = puntos;
            vector<Point> puntosG = puntos;

            // Medir fuerza bruta.
            auto inicioH = chrono::steady_clock::now();
            float distanciaH = bruteForce(puntosH.data(), n);
            auto finH = chrono::steady_clock::now();
            float tiempoH = chrono::duration<float, nano>(finH - inicioH).count();

            // Medir divide y vencerás.
            float tiempoG = 0.0;
            float distanciaG = medirCombinacion(puntosG, tiempoG);

            sumaTiempoH += tiempoH;
            sumaTiempoG += tiempoG;

            // Verificar que ambos obtengan la misma distancia.
            bool coinciden =
                abs(distanciaH - distanciaG) < 1e-9;

            if (!coinciden)
            {
                resultadosCorrectos = false;

                cerr << "Advertencia: resultados distintos para n="
                     << n << ", repeticion=" << k << '\n';
            }

            // Registrar cada repetición.
            long long pares = 1LL * n * (n - 1) / 2;

            archivo << n << ','
                    << k << ','
                    << pares << ','
                    << tiempoH << ','
                    << tiempoG << ','
                    << distanciaH << ','
                    << distanciaG << '\n';
        }

        // Promedios de tiempo para el tamaño actual.
        float promedioH = sumaTiempoH / repeticiones;
        float promedioG = sumaTiempoG / repeticiones;
        long long pares = 1LL * n * (n - 1) / 2;

        valoresN.push_back(static_cast<float>(n));
        promediosH.push_back(promedioH);
        promediosG.push_back(promedioG);

        cout << left
             << setw(10) << n
             << setw(20) << pares
             << setw(22) << promedioH
             << setw(22) << promedioG
             << setw(18)
             << (resultadosCorrectos ? "Coinciden" : "REVISAR")
             << '\n';
    }

    archivo.close();

    if (!archivo)
    {
        cerr << "Error al finalizar la escritura del CSV.\n";
        return 1;
    }

    float interceptoFB, pendienteFB, r2FB;
    float interceptoG, pendienteG, r2G;

    regresionLineal(
        valoresN, promediosH,
        interceptoFB, pendienteFB, r2FB);

    regresionLineal(
        valoresN, promediosG,
        interceptoG, pendienteG, r2G);

    cout << "\n========== REGRESION LINEAL ==========\n";
    cout << "Modelo: y = a + b*n\n\n";

    cout << "Fuerza bruta h(n):\n";
    cout << "  a = " << interceptoFB << '\n';
    cout << "  b = " << pendienteFB << '\n';
    cout << "  h(n) = " << interceptoFB << " + ("
         << pendienteFB << ")*n\n";
    cout << "  R^2 = " << r2FB << "\n\n";

    cout << "Combinacion g(n):\n";
    cout << "  a = " << interceptoG << '\n';
    cout << "  b = " << pendienteG << '\n';
    cout << "  g(n) = " << interceptoG << " + ("
         << pendienteG << ")*n\n";
    cout << "  R^2 = " << r2G << '\n';

    cout << "\nExperimentos finalizados.\n";
    cout << "Resultados guardados en resultados.csv\n";

    return 0;
}

// IMPLEMENTACIÓN DE FUNCIONES.

// Generación de puntos aleatorios.
vector<Point> generarPuntos(int n, int maxCoordenada)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribucion(0, maxCoordenada);

    vector<Point> puntos(n);

    for (int i = 0; i < n; i++)
    {
        puntos[i].x = distribucion(gen);
        puntos[i].y = distribucion(gen);
    }

    return puntos;
}
// Función para calcular la distancia euclidiana entre dos puntos
float dist(Point p1, Point p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
                (p1.y - p2.y) * (p1.y - p2.y));
}
// Función para encontrar el mínimo de dos valores float.
float min(float x, float y)
{
    return (x < y) ? x : y;
}

// Método de Fuerza Bruta.
// A Brute Force method to return the smallest distance between two points in P[] of size n
float bruteForce(Point P[], int n)
{
    float min = FLT_MAX;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (dist(P[i], P[j]) < min)
                min = dist(P[i], P[j]);
    return min;
}

// Método de Divide y Venceras.
// Función para ordenar un arreglo de puntos según la coordenada X
int compareX(const void *a, const void *b)
{
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    return (p1->x - p2->x);
}
// Función para ordenar un arreglo de puntos según la coordenada Y
int compareY(const void *a, const void *b)
{
    Point *p1 = (Point *)a, *p2 = (Point *)b;
    return (p1->y - p2->y);
}

// A utility function to find the distance between the closest points of
// strip of a given size. All points in strip[] are sorted according to
// y coordinate. They all have an upper bound on minimum distance as d.
// Note that this method seems to be a O(n^2) method, but it's a O(n)
// method as the inner loop runs at most 6 times
float stripClosest(Point strip[], int size, float d)
{
    float min = d; // Initialize the minimum distance as d

    qsort(strip, size, sizeof(Point), compareY);

    // Pick all points one by one and try the next points till the difference
    // between y coordinates is smaller than d.
    // This is a proven fact that this loop runs at most 6 times
    for (int i = 0; i < size; ++i)
        for (int j = i + 1; j < size && (strip[j].y - strip[i].y) < min; ++j)
            if (dist(strip[i], strip[j]) < min)
                min = dist(strip[i], strip[j]);

    return min;
}

// A recursive function to find the smallest distance. The array P contains
// all points sorted according to x coordinate
float closestUtil(Point P[], int n)
{
    // If there are 2 or 3 points, then use brute force
    if (n <= 3)
        return bruteForce(P, n);

    // Find the middle point
    int mid = n / 2;
    Point midPoint = P[mid];

    // Consider the vertical line passing through the middle point
    // calculate the smallest distance dl on left of middle point and
    // dr on right side
    float dl = closestUtil(P, mid);
    float dr = closestUtil(P + mid, n - mid);

    // Combinar ambas soluciones
    return combinar(P, n, midPoint, dl, dr);
}

// The main function that finds the smallest distance
// This method mainly uses closestUtil()
float closest(Point P[], int n)
{
    qsort(P, n, sizeof(Point), compareX);

    // Use recursive function closestUtil() to find the smallest distance
    return closestUtil(P, n);
}

float combinar(Point P[], int n, Point midPoint, float dl, float dr)
{
    float d = min(dl, dr);

    vector<Point> strip(n);
    int j = 0;

    for (int i = 0; i < n; i++)
    {
        double diferenciaX =
            abs(static_cast<double>(P[i].x) - midPoint.x);

        if (diferenciaX < d)
        {
            strip[j++] = P[i];
        }
    }

    return min(d, stripClosest(strip.data(), j, d));
}

float medirCombinacion(const vector<Point> &puntos, float &tiempoNs)
{
    vector<Point> ordenados = puntos;
    int n = static_cast<int>(ordenados.size());

    qsort(ordenados.data(), n, sizeof(Point), compareX);

    int mid = n / 2;
    Point midPoint = ordenados[mid];

    float dl = closestUtil(ordenados.data(), mid);
    float dr = closestUtil(ordenados.data() + mid, n - mid);

    auto inicio = chrono::steady_clock::now();
    float resultado = combinar(ordenados.data(), n, midPoint, dl, dr);
    auto fin = chrono::steady_clock::now();

    tiempoNs = chrono::duration<double, nano>(fin - inicio).count();
    return resultado;
}

// Regresión lineal por mínimos cuadrados: y = intercepto + pendiente*x.
void regresionLineal(
    const vector<float> &x,
    const vector<float> &y,
    float &intercepto,
    float &pendiente,
    float &r2)
{
    if (x.size() != y.size() || x.size() < 2)
    {
        intercepto = pendiente = r2 = numeric_limits<float>::quiet_NaN();
        cerr << "Error: se requieren al menos dos pares de datos para la regresion.\n";
        return;
    }

    const size_t n = x.size();
    long double sumaX = 0.0L;
    long double sumaY = 0.0L;
    long double sumaXY = 0.0L;
    long double sumaX2 = 0.0L;

    for (size_t i = 0; i < n; ++i)
    {
        sumaX += x[i];
        sumaY += y[i];
        sumaXY += static_cast<long double>(x[i]) * y[i];
        sumaX2 += static_cast<long double>(x[i]) * x[i];
    }

    long double denominador = n * sumaX2 - sumaX * sumaX;
    if (denominador == 0.0L)
    {
        intercepto = pendiente = r2 = numeric_limits<float>::quiet_NaN();
        cerr << "Error: no se puede ajustar la regresion; los valores de x son iguales.\n";
        return;
    }

    long double b = (n * sumaXY - sumaX * sumaY) / denominador;
    long double a = (sumaY - b * sumaX) / n;
    long double promedioY = sumaY / n;

    long double sr = 0.0L;
    long double st = 0.0L;

    for (size_t i = 0; i < n; ++i)
    {
        long double estimada = a + b * x[i];
        long double residuo = y[i] - estimada;
        long double diferencia = y[i] - promedioY;

        sr += residuo * residuo;
        st += diferencia * diferencia;
    }

    intercepto = static_cast<double>(a);
    pendiente = static_cast<double>(b);
    r2 = (st > 0.0L)
             ? static_cast<float>(1.0L - sr / st)
             : numeric_limits<float>::quiet_NaN();
}