
# Vehicle Routing Problem Solver

## Overview

The **Vehicle Routing Problem Solver** is a C++-based system designed to tackle the Capacitated Vehicle Routing Problem (CVRP) involving a depot and up to 200 customer nodes using fixed-capacity vehicles. It implements:

- **Hybrid Genetic Search (HGS)** with preprocessing and postprocessing
- **Nearest Neighbor + Clarke and Wright Savings + 2-OPT**
- **Google OR-Tools with Guided Local Search (GLS)**

This solver was built as part of an academic research project comparing different CVRP strategies for speed and solution quality.

---

## Features

- ✅**Hybrid Genetic Search (HGS)**
  - Metaheuristic-based solver
  - Preprocessing: *Nearest Neighbor* and *Clarke & Wright Savings*
  - Postprocessing: *2-OPT* local search
  - Scales up to 200 customers and supports flexible fleet sizes

- ⚡ **Greedy Heuristics**
  - *Nearest Neighbor* algorithm
  - Enhanced with *2-OPT* to improve route efficiency

- 🤖 **OR-Tools GLS**
  - Uses Google's optimization suite for robust and competitive solutions

- 🧩 **Flexible Input**
  - Supports `.vrp` (VRPLIB format) and `Coord.txt` (raw coordinate list)

---

## Project Structure

```

research\_project/
├── src/
│   ├── hgs/
│   │   ├── HGS-CVRP/              # HGS-based CVRP solver
│   │   │   ├── Program/           # Source code: Params.cpp, Genetic.cpp, etc.
│   │   │   ├── Coord.txt          # Input file (coordinates)
│   │   │   ├── CMakeLists.txt     # Build file
│   │   ├── HGS-CW-NN-2OPT/        # NN + CW + 2-OPT implementation
│   ├── or-tools-gls/
│   │   ├── vrp\_solution.cpp       # OR-Tools solver
│   │   ├── nearest\_n.cpp          # Nearest Neighbor algorithm
│   │   ├── ver\_second\_solution.cpp # Optional verification code
│   │   ├── txt/                   # Input/output directory
│   │   ├── CMakeLists.txt         # Build file
├── README.md                      # Documentation
└── LICENSE                        # MIT License

````

---

## Installation

### Prerequisites

- **C++17 Compiler** (GCC, Clang, MSVC)
- **CMake >= 3.15**
- **Google OR-Tools** (for `or-tools-gls` only)

---

### Build Instructions

#### Clone the Repository

```bash
git clone https://github.com/your-username/research_project.git
cd research_project
````

#### Build the HGS CVRP Solver

```bash
cd src/hgs/HGS-CVRP
mkdir build
cd build
cmake ..
make
```

#### Build the OR-Tools + NN Implementations

```bash
cd ../../../or-tools-gls
mkdir build
cd build
cmake ..
make
```

---

## Input Files

### Formats

* `Coord.txt`: One line per node (depot first), with `x y` coordinates
* `.vrp`: Standard VRPLIB files with node coords, demands, and capacity

### Where to Place Inputs

| Solver        | Input Location          |
| ------------- | ----------------------- |
| HGS           | `src/hgs/HGS-CVRP/`     |
| NN / OR-Tools | `src/or-tools-gls/txt/` |

---

## Usage

### HGS Solver

```bash
cd src/hgs/HGS-CVRP/build
./hgs_cvrp -i ../Coord.txt -o solution.txt -v
```

### OR-Tools GLS

```bash
cd src/or-tools-gls/build
./vrp_solution
```

### Nearest Neighbor + 2-OPT

```bash
cd src/or-tools-gls/build
./nearest_n
```

---

## Example Output

```bash
$ ./hgs_cvrp -i test_instance.vrp -o solution.txt -v
----- INSTANCE SUCCESSFULLY LOADED WITH 199 CLIENTS AND 20 VEHICLES
Total Distance: 11872.0 meters
Route for vehicle 0: 0 -> 12 -> 31 -> 45 -> 0
...

$ ./vrp_solution
Total Distance: 12345.0 meters
Route for vehicle 1: 0 -> 8 -> 24 -> 50 -> 0
...

$ ./nearest_n
Total Distance: 13478.0 meters
Route for vehicle 2: 0 -> 1 -> 2 -> 3 -> 0
...
```

---

## Contributors

* **Andrés Vélez Rendón**
* **Samuel Andrés Ariza Gómez**


```
```


