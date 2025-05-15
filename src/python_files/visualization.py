# Script para visualizar la solución del VRP

import matplotlib.pyplot as plt
import numpy as np

def plot_vrp_solution(coordinates_file, solution_file):
    # Leer las coordenadas de los nodos
    nodes = {}  # Diccionario para almacenar las coordenadas por ID de nodo
    with open(solution_file, 'r') as f:
        lines = f.readlines()
    
    # Procesar las líneas del archivo
    coordinates_mode = False
    routes_mode = False
    
    # Primero extraemos las coordenadas
    for line in lines:
        line = line.strip()
        if line.startswith('#'):
            if "Coordenadas" in line:
                coordinates_mode = True
                routes_mode = False
                continue
            elif "Rutas" in line:
                coordinates_mode = False
                routes_mode = True
                continue
        
        if not line or line.startswith('#'):
            continue
            
        if coordinates_mode:
            parts = line.split()
            if len(parts) == 3:
                node_id = int(parts[0])
                x = float(parts[1])
                y = float(parts[2])
                nodes[node_id] = (x, y)
    
    # Extraer rutas de vehículos
    routes = []
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
            
        if routes_mode:
            parts = line.split()
            if len(parts) > 1:
                vehicle_id = int(parts[0])
                route = [float(node) for node in parts[1:]]
                routes.append((vehicle_id, route))
    
    # Crear la figura
    plt.figure(figsize=(12, 10))
    
    # Dibujar todos los nodos
    for node_id, (x, y) in nodes.items():
        if node_id == 0:  # Depósito
            plt.plot(x, y, 'r*', markersize=15, label='Depósito')
        else:  # Cliente
            plt.plot(x, y, 'bo', markersize=5)
    
    # Colores para las rutas (hasta 20 colores diferentes)
    colors = plt.cm.tab20(np.linspace(0, 1, 20))
    
    # Dibujar las rutas
    for i, (vehicle_id, route) in enumerate(routes):
        color = colors[vehicle_id % 20]
        
        # Conectar los puntos de la ruta
        for j in range(len(route) - 1):
            node1 = route[j]
            node2 = route[j + 1]
            if node1 in nodes and node2 in nodes:
                x1, y1 = nodes[node1]
                x2, y2 = nodes[node2]
                plt.plot([x1, x2], [y1, y2], '-', color=color, linewidth=1.5)
        
        # Etiqueta para la leyenda (solo para los primeros vehículos para no sobrecargar)
        if i < 5:  # Mostrar solo los primeros 5 vehículos en la leyenda
            plt.plot([], [], '-', color=color, label=f'Vehículo {vehicle_id}')
    
    plt.title('Solución del Problema de Ruteo de Vehículos (VRP)')
    plt.xlabel('Coordenada X')
    plt.ylabel('Coordenada Y')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(loc='upper right')
    
    # Guardar la figura
    plt.savefig('vrp_solution_plot.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("Gráfica de la solución guardada como 'vrp_solution_plot.png'")

if __name__ == "__main__":
    # Archivos de entrada
    plot_vrp_solution("Coord.txt", "solution_plot.txt")