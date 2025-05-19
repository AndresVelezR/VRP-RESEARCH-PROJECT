# Leer coordenadas (solo para nodos, aunque no se usen para distancias)
with open('Coord.txt', 'r') as f:
       coords = [line.strip().split() for line in f]

   # Leer matriz de distancias
with open('Dist.txt', 'r') as f:
       distances = [list(map(float, line.strip().split())) for line in f]

with open('myinstance_explicit.vrp', 'w') as f:
       f.write('NAME : MyInstance\n')
       f.write('COMMENT : Generated with explicit distances\n')
       f.write('TYPE : CVRP\n')
       f.write('DIMENSION : 200\n')
       f.write('EDGE_WEIGHT_TYPE : EXPLICIT\n')
       f.write('EDGE_WEIGHT_FORMAT : FULL_MATRIX\n')
       f.write('CAPACITY : 12\n')
       f.write('NODE_COORD_SECTION\n')
       for i, (x, y) in enumerate(coords, 1):
           f.write(f'{i} {x} {y}\n')
       f.write('EDGE_WEIGHT_SECTION\n')
       for row in distances:
           f.write(' '.join(f'{d:.6f}' for d in row) + '\n')
       f.write('DEMAND_SECTION\n')
       f.write('1 0\n')  # Depósito
       for i in range(2, 201):
           f.write(f'{i} 1\n')  # Clientes
       f.write('DEPOT_SECTION\n')
       f.write('1\n')
       f.write('-1\n')
       f.write('EOF\n')
