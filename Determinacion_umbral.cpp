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

// Programa para determinar el par de puntos más cercanos en un plano.

using namespace std;

// ESTRUCTURAS.

struct Point
{
    int x, y;
};

// PROTOTIPOS.

// Generación de puntos aleatorios.
vector<Point> generarPuntos(int n, int maxCoordenada = 10000);
// Funciones generales.
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

    // Probar distintos tamaños de entrada.
    for (int n = nInicial; n <= nFinal; n += incremento)
    {
        double sumaTiempoFB = 0.0;
        double sumaTiempoDV = 0.0;

        bool resultadosCorrectos = true;

        // Repetir el experimento.
        for (int k = 1; k <= repeticiones; k++)
        {
            // Generar una entrada común para ambos algoritmos.
            vector<Point> puntos =
                generarPuntos(n, maxCoordenada);

            // Copias independientes, fuera de la medición.
            vector<Point> puntosFB = puntos;
            vector<Point> puntosDV = puntos;

            // Medir fuerza bruta.
            auto inicioFB = chrono::steady_clock::now();

            double distanciaFB =
                bruteForce(puntosFB.data(), n);

            auto finFB = chrono::steady_clock::now();

            double tiempoFB = chrono::duration<double, nano>(
                finFB - inicioFB
            ).count();

            // Medir divide y vencerás.
            auto inicioDV = chrono::steady_clock::now();

            double distanciaDV =
                closest(puntosDV.data(), n);

            auto finDV = chrono::steady_clock::now();

            double tiempoDV = chrono::duration<double, nano>(
                finDV - inicioDV
            ).count();

            sumaTiempoFB += tiempoFB;
            sumaTiempoDV += tiempoDV;

            // Verificar que ambos obtengan la misma distancia.
            bool coinciden =
                abs(distanciaFB - distanciaDV) < 1e-9;

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
                    << tiempoFB << ','
                    << tiempoDV << ','
                    << distanciaFB << ','
                    << distanciaDV << '\n';
        }

        // Promedios de tiempo para el tamaño actual.
        double promedioFB = sumaTiempoFB / repeticiones;
        double promedioDV = sumaTiempoDV / repeticiones;

        long long pares = 1LL * n * (n - 1) / 2;

        cout << left
             << setw(10) << n
             << setw(20) << pares
             << setw(22) << promedioFB
             << setw(22) << promedioDV
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

    // Find the smaller of two distances
    float d = min(dl, dr);

    // Build an array strip[] that contains points close (closer than d)
    // to the line passing through the middle point
    Point strip[n];
    int j = 0;
    for (int i = 0; i < n; i++)
        if (abs(P[i].x - midPoint.x) < d)
            strip[j] = P[i], j++;

    // Find the closest points in strip. Return the minimum of d and closest
    // distance is strip[]
    return min(d, stripClosest(strip, j, d));
}

// The main function that finds the smallest distance
// This method mainly uses closestUtil()
float closest(Point P[], int n)
{
    qsort(P, n, sizeof(Point), compareX);

    // Use recursive function closestUtil() to find the smallest distance
    return closestUtil(P, n);
}