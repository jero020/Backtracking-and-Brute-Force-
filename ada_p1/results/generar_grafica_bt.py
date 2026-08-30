"""Genera results/bt_referencia_y_variantes.png a partir de
bt_referencia_y_variantes.csv (Seccion 9.2: nodos visitados por instancia,
distinguiendo ejecuciones completas de muestras parciales de 60s para las
variantes que no terminan en tiempo razonable -- ver
report/bt_pseudocodigo_complejidad.md).

Automatizado con asistencia de IA (Claude), revisado por Jeronimo Velez
Acosta -- los datos vienen de ejecuciones reales, no son inventados.
"""
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

filas = []
with open("bt_referencia_y_variantes.csv", newline="", encoding="utf-8") as f:
    lector = csv.DictReader(f)
    for fila in lector:
        if fila["nodos"]:
            filas.append(fila)

etiquetas = [f"{f['instancia']}\n(n={f['n']})" for f in filas]
nodos = [int(f["nodos"]) for f in filas]
completos = [f["estado"] == "completado" for f in filas]
colores = ["#16a34a" if c else "#dc2626" for c in completos]

fig, ax = plt.subplots(figsize=(10, 5))
barras = ax.bar(etiquetas, nodos, color=colores)
ax.set_yscale("log")
ax.set_ylabel("Nodos visitados (con poda, escala log)")
ax.set_title("Modulo BT -- instancia de referencia y variantes (Seccion 9.2)")
ax.grid(True, axis="y", which="both", linestyle="--", alpha=0.4)

for barra, f in zip(barras, filas):
    nota = "completo" if f["estado"] == "completado" else "muestra 60s\n(no termino)"
    ax.text(barra.get_x() + barra.get_width() / 2, barra.get_height() * 1.15,
            nota, ha="center", va="bottom", fontsize=8)

verde = plt.Rectangle((0, 0), 1, 1, color="#16a34a", label="Ejecucion completa")
rojo = plt.Rectangle((0, 0), 1, 1, color="#dc2626", label="Muestra parcial (60s, no termino)")
ax.legend(handles=[verde, rojo])

plt.xticks(rotation=15, ha="right")
fig.tight_layout()
fig.savefig("bt_referencia_y_variantes.png", dpi=150)
print("OK: bt_referencia_y_variantes.png generado")
