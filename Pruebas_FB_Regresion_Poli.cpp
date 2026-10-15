#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sys/time.h>
#include <vector>
#include <random>

using namespace std;

// Estructura para representar un punto en el plano
struct Point {
    int x, y;
};

// Generación de puntos aleatorios
vector<Point> generarPuntos(int n, int maxCoordenada = 10000)
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

// Calcular la distancia entre dos puntos
double dist(Point p1, Point p2) {
    return sqrt(
        (p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y)
    );
}

// Método de fuerza bruta para encontrar la distancia mínima
double bruteForce(Point P[], int n) {
    double distanciaMinima = DBL_MAX;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double distancia = dist(P[i], P[j]);

            if (distancia < distanciaMinima)
                distanciaMinima = distancia;
        }
    }

    return distanciaMinima;
}

// Funcion para calcular la Regresion Polinomial
void regresionPolinomial(double x[], double y[], int n, double coeficientes[]) {

    const int grado = 2;

    double matrix[grado+1][grado+1];
    double solutions[grado+1];

    double sumX = 0;
    double sumXY = 0;

    cout << "------------SISTEMA DE ECUACIONES-------------" << endl;

    for (int i = 0; i < grado+1; ++i) {
        sumXY = 0;

        for (int k = 0; k < n; ++k) {
            sumXY += pow(x[k],i)*y[k];
        }

        solutions[i] = sumXY;

        for (int j = 0; j < grado+1; ++j) {
            if (i == 0 && j == 0) {
                matrix[i][j] = n;
            } else {
                sumX = 0;

                for (int k = 0; k < n; ++k) {
                    sumX += pow(x[k],(j+i));
                }

                matrix[i][j] = sumX;
            }

            cout << matrix[i][j] << ", ";
        }

        cout << "= " << solutions[i] << endl;
    }

    cout << "-------------------GAUSS SEIDEL---------------------" << endl;

    int size = grado + 1;
    double EPSILON = 0.0000001;

    double S[size];
    double OS[size];
    double E[size];

    for (int i = 0; i < size; ++i) {
        S[i] = 0;
        OS[i] = 0;
        E[i] = 0;
    }

    bool errorCheck = false;
    int counter = 0;

    while(!errorCheck){
        for(int i = 0; i < size; ++i){
            double sum = 0;

            for(int j = 0; j < size; ++j){
                if (j != i) {
                    sum += matrix[i][j]*S[j];
                }
            }

            OS[i] = S[i];
            S[i] = (solutions[i] - sum)/matrix[i][i];

            E[i] = (S[i] - OS[i]);
        }

        errorCheck = true;

        for(int i = 0; i < size; ++i){
            if (abs(E[i]) > EPSILON){
                errorCheck = false;
                break;
            }
        }

        counter++;
    }

    for (int i = 0; i < size; ++i){
        cout << " a" << i << ": " << S[i] << endl;
        coeficientes[i] = S[i];
    }

    cout << "it took " << counter << " times to terminate" << endl;

    cout << "------------------POLINOMIO---------------" << endl;

    for (int i = size-1; i >= 1; --i){
        cout << S[i] << "X^" << i << " + ";
    }

    cout << S[0] << endl;

    cout << "------------------ERRORES------------------" << endl;

    double e2[n];

    for (int i = 0; i < n; ++i){
        double Ycalculada = 0;

        for (int j = size-1; j >= 1; --j) {
            Ycalculada += S[j]*(pow(x[i], j));
        }

        Ycalculada += S[0];
        e2[i] = pow(y[i] - Ycalculada, 2);
    }

    double sr = 0;
    double sumY = solutions[0];
    double st = 0;

    for (int i = 0; i < n; ++i){
        sr += e2[i];
        st += pow(y[i] - (sumY/n), 2);
    }

    cout << "Sr: " << sr << endl;

    double sxy = sqrt(sr/(n-size));
    cout << "St: " << st << endl;

    double r2 = (st-sr)/st;
    double r = sqrt(r2);

    cout << "Error Estandar (Sx/y): " << sxy << endl;
    cout << "Coeficiente de determinacion (r2): " << r2 << endl;
    cout << "Coeficiente de correlacion (r): " << r << endl;
    cout << endl;
}

int main() {
    cout << fixed << setprecision(6);

    ofstream archivo("resultados_fuerza_bruta_polinomio.csv");

    if (!archivo.is_open()){
        cout << "No se pudo crear el archivo de resultados." << endl;
        return 1;
    }

    archivo << fixed << setprecision(12);
    archivo << "Puntos,Pares examinados,Tiempo promedio (s),Distancia promedio\n";

    double x[100];
    double y[100];

    int indice = 0;

    for (int n = 10; n <= 1000; n += 10){

        double sumaTiempos = 0;
        double sumaDistancia = 0;

        for(int k = 0; k < 50; k++){

            // Generar puntos usando la nueva función
            vector<Point> puntos = generarPuntos(n);

            struct timeval begin, end;

            gettimeofday(&begin, 0);

            double distancia = bruteForce(puntos.data(), n);

            gettimeofday(&end, 0);

            long seconds = end.tv_sec - begin.tv_sec;
            long microseconds = end.tv_usec - begin.tv_usec;

            double elapsed = seconds + microseconds * 1e-6;

            sumaTiempos += elapsed;
            sumaDistancia += distancia;
        }

        // Calcular los promedios de las 50 repeticiones
        double tiempoPromedio = sumaTiempos / 50;
        double distanciaPromedio = sumaDistancia / 50;

        long long pares = 1LL * n * (n - 1) / 2;

        x[indice] = n;
        y[indice] = tiempoPromedio;
        indice++;

        cout << n << "\t"
             << pares << "\t"
             << tiempoPromedio << "\t"
             << distanciaPromedio << endl;

        archivo << n << ","
                << pares << ","
                << tiempoPromedio << ","
                << distanciaPromedio << "\n";
    }

    archivo.close();

    double coeficientes[3];

    regresionPolinomial(x, y, 100, coeficientes);

    ofstream archivoRegresion("resultados_regresion_polinomial.csv");

    if (!archivoRegresion.is_open()) {
        cout << "No se pudo crear el archivo de regresion." << endl;
        return 1;
    }

    archivoRegresion << fixed << setprecision(12);
    archivoRegresion << "Puntos,Tiempo estimado (s)\n";

    for (int i = 0; i < 100; ++i){

        double tiempoEstimado =
            coeficientes[2] * x[i] * x[i] +
            coeficientes[1] * x[i] +
            coeficientes[0];

        archivoRegresion << x[i] << ","
                         << tiempoEstimado << "\n";
    }

    archivoRegresion.close();

    cout << "\n------Fin de las pruebas-------------" << endl;

    return 0;
}
