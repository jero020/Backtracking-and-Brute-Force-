"""Genera results/fb_experimentacion_seccion8.png a partir de
fb_experimentacion_seccion8.csv (Seccion 8.1: tiempo vs. tamano del
espacio de busqueda, peor caso / no encontrada, para los alfabetos A1 y A2).

Automatizado con asistencia de IA (Claude), revisado por Jeronimo Velez
Acosta -- los datos que grafica vienen de ejecuciones reales del binario de
Javier (FB/main.cpp), no son inventados ni simulados.
"""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

filas = []
with open("fb_experimentacion_seccion8.csv", newline="", encoding="utf-8") as f:
    lector = csv.DictReader(f)
    for fila in lector:
        filas.append(fila)

a1 = [f for f in filas if f["alfabeto"] == "A1"]
a2 = [f for f in filas if f["alfabeto"] == "A2"]

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))

# --- Panel 1: tiempo vs. longitud n (muestra el "muro exponencial") ---
for datos, etiqueta, color in [(a1, "A1 (26 simbolos)", "#2563eb"), (a2, "A2 (36 simbolos)", "#dc2626")]:
    xs = [int(f["longitud"]) for f in datos]
    ys = [float(f["tiempo_ms"]) for f in datos]
    ax1.plot(xs, ys, marker="o", label=etiqueta, color=color)
ax1.set_yscale("log")
ax1.set_xlabel("Longitud de la contrasena (n)")
ax1.set_ylabel("Tiempo de ejecucion (ms, escala log)")
ax1.set_title("Crecimiento del tiempo vs. n\n(peor caso: contrasena no esta en el espacio)")
ax1.grid(True, which="both", linestyle="--", alpha=0.4)
ax1.legend()

# --- Panel 2: tiempo vs. tamano del espacio de busqueda (log-log) ---
todos = sorted(filas, key=lambda f: int(f["espacio_busqueda"]))
xs = [int(f["espacio_busqueda"]) for f in todos]
ys = [float(f["tiempo_ms"]) for f in todos]
colores = ["#2563eb" if f["alfabeto"] == "A1" else "#dc2626" for f in todos]
ax2.scatter(xs, ys, c=colores, zorder=3)
ax2.plot(xs, ys, color="#94a3b8", linewidth=1, zorder=2)
ax2.set_xscale("log")
ax2.set_yscale("log")
ax2.set_xlabel("Tamano del espacio de busqueda |Sigma|^n (escala log)")
ax2.set_ylabel("Tiempo de ejecucion (ms, escala log)")
ax2.set_title("Tiempo vs. tamano del espacio\n(cota teorica |Sigma|^n vs. medicion real)")
ax2.grid(True, which="both", linestyle="--", alpha=0.4)

fig.suptitle("Modulo FB -- Experimentacion Seccion 8 (peor caso, SHA-256)", fontsize=12)
fig.tight_layout(rect=[0, 0, 1, 0.94])
fig.savefig("fb_experimentacion_seccion8.png", dpi=150)
print("OK: fb_experimentacion_seccion8.png generado")
