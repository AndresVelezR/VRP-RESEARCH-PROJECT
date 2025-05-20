import numpy as np
import os

def generate_vrp_file(coord_file_path, dist_file_path, output_file_path, num_vehicles, max_clients_per_vehicle):
    """
    Genera un archivo .vrp en formato estándar a partir de archivos de coordenadas y distancias.
    Compatible con el algoritmo HGS-CVRP.
    
    Args:
        coord_file_path (str): Ruta al archivo de coordenadas.
        dist_file_path (str): Ruta al archivo de distancias.
        output_file_path (str): Ruta donde se guardará el archivo .vrp.
        num_vehicles (int): Número de vehículos disponibles.
        max_clients_per_vehicle (int): Máximo número de clientes por vehículo.
    """
    # Leer archivo de coordenadas
    try:
        # Intentar leer con numpy (si está en formato tabular)
        coordinates = np.loadtxt(coord_file_path, delimiter=None)
    except:
        # Si falla, leer como texto y procesar manualmente
        with open(coord_file_path, 'r') as f:
            lines = f.readlines()
            coordinates = []
            for line in lines:
                parts = line.strip().split()
                if len(parts) >= 2:  # Asegurar que hay al menos 2 números (x, y)
                    try:
                        x = float(parts[0])
                        y = float(parts[1])
                        coordinates.append([x, y])
                    except ValueError:
                        continue  # Ignorar líneas que no contienen coordenadas válidas
            coordinates = np.array(coordinates)
    
    # Verificar si tenemos coordenadas
    if len(coordinates) == 0:
        raise ValueError("No se pudieron leer las coordenadas del archivo.")
    
    # Número total de nodos (1 depósito + n clientes)
    num_nodes = len(coordinates)
    
    # Establecer la capacidad basada en el número máximo de clientes por vehículo
    capacity = max_clients_per_vehicle
    
    # El primer nodo es el depósito, el resto son los clientes
    depot_index = 0
    
    # Calcular el número de vehículos necesarios
    # Según el enunciado, cada vehículo puede atender máximo 12 clientes
    # y hay 199 clientes en total
    min_vehicles_needed = (num_nodes - 1 + max_clients_per_vehicle - 1) // max_clients_per_vehicle
    k_value = min(num_vehicles, min_vehicles_needed)
    
    # Generar archivo .vrp
    with open(output_file_path, 'w') as f:
        # Sección de metadatos
        base_name = os.path.basename(output_file_path).split('.')[0]
        f.write(f"NAME : \t{base_name}-n{num_nodes}-k{k_value}\t\n")
        f.write(f"COMMENT : \t\"Generado a partir de archivos de coordenadas para HGS-CVRP\"\t\n")
        f.write(f"TYPE : \tCVRP\t\n")
        f.write(f"DIMENSION : \t{num_nodes}\t\n")
        f.write(f"EDGE_WEIGHT_TYPE : \tEUC_2D\t\n")
        f.write(f"CAPACITY : \t{capacity}\t\n")
        
        # Sección de coordenadas
        f.write("NODE_COORD_SECTION\t\t\n")
        for i in range(num_nodes):
            node_id = i + 1  # Los IDs de nodos empiezan en 1
            x, y = coordinates[i]
            f.write(f"{node_id}\t{x}\t{y}\n")
        
        # Sección de demanda
        f.write("DEMAND_SECTION\t\t\n")
        for i in range(num_nodes):
            node_id = i + 1
            # El depósito tiene demanda 0, los clientes tienen demanda 1
            demand = 0 if i == depot_index else 1
            f.write(f"{node_id}\t{demand}\t\n")
        
        # Sección de depósito
        f.write("DEPOT_SECTION\t\t\n")
        f.write(f"\t{depot_index + 1}\t\n")  # El ID del depósito, ajustado para empezar en 1
        f.write("\t-1\t\n")
        f.write("EOF\t\t\n")

if __name__ == "__main__":
    # Parámetros del problema
    num_vehicles = 20
    max_clients_per_vehicle = 12
    
    # Rutas de archivos
    coord_file_path = "Coord.txt"
    dist_file_path = "Dist.txt"  # Aunque ya no se usa directamente para la matriz
    output_file_path = "instance.vrp"
    
    # Generar archivo .vrp
    generate_vrp_file(coord_file_path, dist_file_path, output_file_path, num_vehicles, max_clients_per_vehicle)
    
    print(f"Archivo .vrp generado exitosamente en {output_file_path}")
    print("Este archivo es compatible con el algoritmo HGS-CVRP.")