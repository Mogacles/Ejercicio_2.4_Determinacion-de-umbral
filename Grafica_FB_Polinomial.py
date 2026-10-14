import pandas as pd
import matplotlib.pyplot as plt

datos = pd.read_csv("resultados_fuerza_bruta.csv")
regresion = pd.read_csv("resultados_regresion.csv")

plt.scatter(
    datos["Puntos"],
    datos["Tiempo promedio (s)"],
    color = "blue",
    label="Tiempos medidos"
)

plt.plot(
    regresion["Puntos"],
    regresion["Tiempo estimado (s)"],
    color = "red",
    label="Regresion polinomial"
)

plt.xlabel("Cantidad de puntos (n)")
plt.ylabel("Tiempo promedio (segundos)")
plt.title("Fuerza bruta: regresion polinomial")
plt.legend()
plt.grid(True)
plt.show()
