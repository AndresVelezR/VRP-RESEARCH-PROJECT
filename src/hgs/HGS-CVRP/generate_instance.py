with open('Coord.txt', 'r') as f:
       coords = [line.strip().split() for line in f]

with open('myinstance.vrp', 'w') as f:
       f.write('NAME : MyInstance\n')
       f.write('COMMENT : Generated for practice\n')
       f.write('TYPE : CVRP\n')
       f.write('DIMENSION : 200\n')
       f.write('EDGE_WEIGHT_TYPE : EUC_2D\n')
       f.write('CAPACITY : 12\n')
       f.write('NODE_COORD_SECTION\n')
       for i, (x, y) in enumerate(coords, 1):
           f.write(f'{i} {x} {y}\n')
       f.write('DEMAND_SECTION\n')
       f.write('1 0\n')  # Depot has demand 0
       for i in range(2, 201):
           f.write(f'{i} 1\n')  # Each customer has demand 1
       f.write('DEPOT_SECTION\n')
       f.write('1\n')
       f.write('-1\n')
       f.write('EOF\n')
