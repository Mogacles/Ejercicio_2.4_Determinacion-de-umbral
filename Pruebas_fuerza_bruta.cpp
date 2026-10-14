#include <iostream> 
#include <cfloat> 
#include <cstdlib> 
#include <cmath>  
#include <ctime>
#include <iomanip>
#include <sys/time.h>
#include <fstream>

using namespace std;

// Estructura para representar un punto en el plano
struct Point {
    int x, y;
};

// Calcular la distancia entre dos puntos
float dist(Point p1, Point p2){
    return sqrt(
        (p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y)
    );
}

// Método de fuerza bruta para encontrar la distancia mínima
float bruteForce(Point P[], int n){ 
	float distanciaMinima = FLT_MAX; 

	for (int i = 0; i < n; ++i){
		for (int j = i + 1; j < n; ++j){
			float distancia = dist(P[i], P[j]); 
			if (distancia < distanciaMinima){ 
				distanciaMinima = distancia; 
			} 
		} 
	} 
	return distanciaMinima; 
}

int main() {
    srand(time(NULL));// Inicializar el generador de números aleatorios

    ofstream archivo("resultados_fuerza_bruta.csv"); // Creacion de archivo csv.
    if (!archivo.is_open()) {
        cout << "Error al crear el archivo CSV." << endl;
        return 1;
    }
    archivo << "Puntos,Pares examinados,Tiempo promedio 10 iteraciones (s),Distancia promedio entre puntos\n";

    // Encabezado de la tabla 
    cout << fixed << setprecision(6);

    cout << "\nPRUEBAS DEL ALGORITMO DE FUERZA BRUTA\n\n";

    cout << left << setw(12) << "Puntos (n)" << setw(18) << "Pares examinados" 
        << setw(25) << "Tiempo promedio 10 iteraciones (s)" << setw(28) << "Menor Distancia entre puntos" << endl;

    cout << string(55, '-') << endl;

    for (int n = 10; n <= 1000; n += 10){ // Tamaños de n 
        Point *P = new Point[n]; // Reserva de memoria 

        double sumaTiempos = 0;
        double sumaDistancia = 0;

        for (int k = 0; k < 10; k++){

            for (int i = 0; i < n; i++){ 
                P[i].x = rand() % 10000; 
                P[i].y = rand() % 10000;
            }

            struct timeval begin, end;

            gettimeofday(&begin, 0); // <- Inicio de medicion del tiempo

            sumaDistancia += bruteForce(P, n);

            gettimeofday(&end, 0); // <- Fin de medicion de tiempo

            // Calcular el tiempo transcurrido
            long seconds = end.tv_sec - begin.tv_sec;
            long microseconds = end.tv_usec - begin.tv_usec;

            double elapsed = seconds + microseconds * 1e-6;

            sumaTiempos += elapsed;
        }

        double tiempoPromedio = sumaTiempos / 10;
        double distanciaPromedio = sumaDistancia / 10;

        // Calcular la cantidad de pares examinados
        long long pares = 1LL * n * (n - 1) / 2;

        // Mostrar los resultados
        cout << left
            << setw(12) << n
            << setw(20) << pares
            << setw(25) << tiempoPromedio
            << setw(20) << distanciaPromedio
            << endl;

        // Guardar datos en archivo csv.
        archivo << n << ","
                << pares << ","
                << tiempoPromedio << ","
                << distanciaPromedio << "\n";

        delete[] P; // Liberar la memoria
    }

    cout<<"\n------Fin de las pruebas-------------"<<endl;

    archivo.close(); // Cerrar archivo

    return 0;
}