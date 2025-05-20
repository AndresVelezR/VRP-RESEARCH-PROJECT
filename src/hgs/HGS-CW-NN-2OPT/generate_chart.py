import matplotlib.pyplot as plt
import numpy as np
import re

# Función para leer las coordenadas desde Coord.txt
def leer_coordenadas(archivo):
    coordenadas = []
    with open(archivo, 'r') as f:
        for linea in f:
            x, y = map(float, linea.strip().split())
            coordenadas.append((x, y))
    return np.array(coordenadas)

# Función para leer las rutas desde mySolution.sol
def leer_rutas(archivo):
    rutas = []
    costo = 0
    with open(archivo, 'r') as f:
        for linea in f:
            if linea.startswith('Route'):
                nodos = list(map(int, re.findall(r'\d+', linea)[1:]))
                rutas.append(nodos)
            elif linea.startswith('Cost'):
                costo = float(linea.split()[1])
    return rutas, costo

# Leer coordenadas y rutas
coordenadas = leer_coordenadas('Coord.txt')
rutas, costo_total = leer_rutas('mySolution.sol')

# Crear paleta de colores
colores = plt.cm.tab20(np.linspace(0, 1, len(rutas)))

# 1. Gráfico general con todas las rutas
plt.figure(figsize=(12, 8))
plt.scatter(coordenadas[1:, 0], coordenadas[1:, 1], c='blue', label='Clientes', s=30, alpha=0.5)
plt.scatter(coordenadas[0, 0], coordenadas[0, 1], c='red', label='Depósito', s=100, marker='s')
for i, ruta in enumerate(rutas):
    ruta = [nodo - 1 for nodo in ruta]
    ruta_completa = [0] + ruta + [0]
    x = coordenadas[ruta_completa, 0]
    y = coordenadas[ruta_completa, 1]
    plt.plot(x, y, c=colores[i], label=f'Ruta {i+1}', linewidth=1.5, alpha=0.7)
plt.title(f'Solución CVRP - Costo Total: {costo_total}', fontsize=14)
plt.xlabel('Coordenada X')
plt.ylabel('Coordenada Y')
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True)
plt.tight_layout()
plt.savefig('cvrp_general.png')

# 2. Subgráficos individuales para cada ruta
n_rutas = len(rutas)
cols = 3
filas = (n_rutas + cols - 1) // cols
fig, axes = plt.subplots(filas, cols, figsize=(15, 5 * filas))
axes = axes.flatten() if n_rutas > 1 else [axes]

for i, ruta in enumerate(rutas):
    # Convertir índices a 0-based
    ruta = [nodo - 1 for nodo in ruta]
    ruta_completa = [0] + ruta + [0]
    
    # Graficar solo los nodos de la ruta y el depósito
    ax = axes[i]
    nodos_ruta = coordenadas[ruta]
    ax.scatter(nodos_ruta[:, 0], nodos_ruta[:, 1], c='blue', label='Clientes', s=50)
    ax.scatter(coordenadas[0, 0], coordenadas[0, 1], c='red', label='Depósito', s=100, marker='s')
    
    # Graficar la ruta
    x = coordenadas[ruta_completa, 0]
    y = coordenadas[ruta_completa, 1]
    ax.plot(x, y, c=colores[i], label=f'Ruta {i+1}', linewidth=2)
    for j in range(len(ruta_completa)-1):
        ax.arrow(x[j], y[j], x[j+1]-x[j], y[j+1]-y[j], 
                 color=colores[i], length_includes_head=True, 
                 head_width=0.5, head_length=0.5, alpha=0.7)
    
    ax.set_title(f'Ruta {i+1}')
    ax.set_xlabel('Coordenada X')
    ax.set_ylabel('Coordenada Y')
    ax.grid(True)
    ax.legend()

# Desactivar ejes vacíos
for i in range(len(rutas), len(axes)):
    axes[i].axis('off')

plt.tight_layout()
plt.savefig('cvrp_rutas_individuales.png')
plt.show()