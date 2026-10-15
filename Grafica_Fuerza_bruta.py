import pandas as pd
import matplotlib.pyplot as plt

# Leer los resultados generados por C++
datos = pd.read_csv("resultados_fuerza_bruta.csv")

# Mostrar los datos
print(datos)

# Graficar el tiempo promedio en función de n
plt.plot(
    datos["Puntos"],
    datos["Tiempo promedio 10 iteraciones (s)"],
    marker="o",
    label="Fuerza bruta"
)

plt.xlabel("Cantidad de puntos (n)")
plt.ylabel("Tiempo promedio (segundos)")
plt.title("Tiempo de ejecución del algoritmo de fuerza bruta")
plt.legend()
plt.grid(True)

plt.show()