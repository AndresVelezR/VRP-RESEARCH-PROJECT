# Vehicle Routing Problem Solver

## Overview
The **Vehicle Routing Problem Solver** is a C++-based project designed to solve the Vehicle Routing Problem (VRP) for a set of customers with a fixed vehicle capacity. The project implements two algorithms: a metaheuristic approach using Google's OR-Tools with Guided Local Search (GLS) and a greedy Nearest Neighbor (NN) heuristic. The solver processes input data from distance matrices (`Dist.txt`) and coordinate files (`Coord.txt`) to compute optimal or near-optimal routes for a fleet of vehicles.

This project was developed as part of an academic or research effort to compare the performance of advanced optimization tools against simple heuristics in solving VRP instances.

## Features
- **OR-Tools Implementation**: Utilizes OR-Tools with Guided Local Search to find high-quality solutions for VRP instances with up to 200 nodes.
- **Nearest Neighbor Heuristic**: Implements a fast, greedy algorithm that constructs routes by selecting the closest unvisited customer, respecting vehicle capacity constraints (12 customers per vehicle).
- **Flexible Input Handling**: Reads distance matrices (`Dist.txt`) and coordinate files (`Coord.txt`) to support various problem instances.
- **Scalable Design**: Handles large datasets efficiently with configurable parameters for solution quality and runtime.

## Repository Structure
```
vrp-solver/
├── src/
│   ├── vrp_solution.cpp    # OR-Tools implementation with GLS
│   ├── nearest_n.cpp       # Nearest Neighbor heuristic implementation
├── data/
│   ├── Dist.txt           # Distance matrix input file
│   ├── Coord.txt          # Coordinates input file
├── README.md              # Project documentation (this file)
└── LICENSE                # License file (MIT License)
```

## Installation
### Prerequisites
- **C++ Compiler**: GCC, Clang, or MSVC (C++17 or later).
- **OR-Tools**: Google's OR-Tools library (version 9.8 or later).
- **CMake**: For building the project (version 3.10 or later).

### Setup
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/vrp-solver.git
   cd vrp-solver
   ```

2. **Install OR-Tools**:
   Follow the [OR-Tools installation guide](https://developers.google.com/optimization/install) for C++. Ensure the library is linked correctly in your build system.

3. **Build the Project**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Prepare Input Files**:
   - Place `Dist.txt` and `Coord.txt` in the `data/` directory.
   - Ensure `Dist.txt` contains a valid distance matrix (in meters) and `Coord.txt` contains node coordinates.

5. **Run the Solver**:
   ```bash
   ./vrp_solution  # Run OR-Tools solver
   ./nearest_n     # Run Nearest Neighbor heuristic
   ```

## Usage
- **Input Files**:
  - `Dist.txt`: A square matrix of floating-point distances (in meters) between nodes. The matrix must be symmetric with zero diagonal entries.
  - `Coord.txt`: A list of (x, y) coordinates for each node (optional, used for visualization or distance computation if needed).
- **Output**: The solver outputs the total distance (in meters) and the routes for each vehicle, respecting the capacity constraint of 12 customers per vehicle.
- **Configuration**:
  - Modify `vrp_solution.cpp` to adjust OR-Tools parameters (e.g., time limit, first solution strategy).
  - Adjust `nearest_n.cpp` for custom heuristic behavior.

Example:
```bash
$ ./vrp_solution
Total Distance: 15000.0 meters
Route for vehicle 0: 0 -> 1 -> 3 -> 5 -> 0
...

$ ./nearest_n
Total Distance: 16500.0 meters
Route for vehicle 0: 0 -> 2 -> 4 -> 6 -> 0
...
```

## Contributors
- Andrés Vélez Rendón
- Samuel Andrés Ariza Gomez

