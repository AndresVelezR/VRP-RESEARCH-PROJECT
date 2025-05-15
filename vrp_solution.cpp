#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <regex> 

// OR-Tools
#include "ortools/constraint_solver/routing.h"
#include "ortools/constraint_solver/routing_enums.pb.h"
#include "ortools/constraint_solver/routing_index_manager.h"
#include "ortools/constraint_solver/routing_parameters.h"

// Protobuf Duration
#include "google/protobuf/duration.pb.h"

using namespace operations_research;

// Estructuras de datos
struct Point {
    double x;
    double y;
};

struct DataModel {
    std::vector<std::vector<int64_t>> distance_matrix;
    std::vector<Point> coordinates;
    int num_vehicles = 20;
    int max_clients_per_vehicle = 12;
    int time_limit_seconds = 120;
    RoutingIndexManager::NodeIndex depot{0};
};

// Comprueba existencia de archivo
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}


bool ReadDistanceMatrix(const std::string& filename,
                        std::vector<std::vector<int64_t>>& distance_matrix) {
    if (!fileExists(filename)) {
        std::cerr << "Error: El archivo " << filename << " no existe.\n";
        return false;
    }
    std::ifstream file(filename);
    std::string line;
    distance_matrix.clear();
    std::regex ws_re("\\s+"); // Coincide con uno o más espacios en blanco (incluye tabulaciones)

    int line_count = 0;
    while (std::getline(file, line)) {
        line_count++;
        // Eliminar espacios en blanco al inicio y al final
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (line.empty()) continue; // Ignorar líneas vacías

        // Dividir la línea en tokens usando cualquier cantidad de espacios o tabulaciones
        std::vector<std::string> tokens;
        std::sregex_token_iterator iter(line.begin(), line.end(), ws_re, -1);
        std::sregex_token_iterator end;
        for (; iter != end; ++iter) {
            if (!iter->str().empty()) { // Ignorar tokens vacíos
                tokens.push_back(*iter);
            }
        }

        if (tokens.size() != 200) {
            std::cerr << "Error: La fila " << line_count << " tiene " << tokens.size()
                      << " columnas, se esperaban 200.\n";
            return false;
        }

        std::vector<int64_t> row;
        for (const auto& token : tokens) {
            try {
                // Convertir a int64_t, redondeando el valor flotante
                int64_t value = static_cast<int64_t>(std::stod(token) * 10);//int64_t value = static_cast<int64_t>(std::stod(token) + 0.5);
                row.push_back(value);
            } catch (const std::exception& e) {
                std::cerr << "Error: Valor no numérico en la fila " << line_count
                          << ": '" << token << "' (" << e.what() << ").\n";
                return false;
            }
        }
        distance_matrix.push_back(row);
    }

    if (distance_matrix.size() != 200) {
        std::cerr << "Error: Se esperaban 200 filas, se leyeron "
                  << distance_matrix.size() << ".\n";
        return false;
    }

    std::cout << "el dist.txt tiene -> " << distance_matrix.size() << " filas y tiene -> "
              << distance_matrix[0].size() << " columnas en la fila 0 y en la 1 tiene -> "
              << distance_matrix[1].size() << "\n";
    return true;
}

// Lee coordenadas
bool ReadCoordinates(const std::string& filename,
                     std::vector<Point>& coordinates) {
    if (!fileExists(filename)) {
        std::cerr << "Error: El archivo " << filename << " no existe.\n";
        return false;
    }
    std::ifstream file(filename);
    std::string line;
    coordinates.clear();
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Point p;
        if (iss >> p.x >> p.y) coordinates.push_back(p);
    }
    // Verificación de 200 coordenadas (manteniendo la lógica original)
    if (coordinates.size() != 200) {
        std::cerr << "Error: Se esperaban 200 coordenadas, se leyeron "
                  << coordinates.size() << ".\n";
        return false;
    }
    return true;
}

// Imprime la solución por vehículo
void PrintSolution( DataModel& data,
                    RoutingIndexManager& manager,
                    RoutingModel& routing,
                    const Assignment& solution) { // Se hizo const Assignment&
    int64_t total_distance = 0;
    int vehicles_used = 0;
    std::cout << "\n=== RESULTADOS DE LA SOLUCIÓN ===\n";
    for (int v = 0; v < data.num_vehicles; ++v) {
        int64_t index = routing.Start(v);
        // Si la siguiente variable del inicio es el final, el vehículo no se usa.
        if (routing.IsEnd(solution.Value(routing.NextVar(index)))) continue; 
        
        vehicles_used++;
        std::cout << "Vehículo " << v << ": ";
        int64_t route_dist = 0;
        int served_clients = 0; // Renombrado para claridad
        std::stringstream route_ss; // Renombrado para claridad
        route_ss << manager.IndexToNode(index).value();
        
        int64_t previous_index = index;
        index = solution.Value(routing.NextVar(index));

        while (!routing.IsEnd(index)) {
            route_ss << " -> " << manager.IndexToNode(index).value();
            // Solo contar clientes servidos (nodos que no son el depósito)
            // Asumiendo que el depósito es el nodo 0 y no cuenta como "servido" en este contexto.
            // La lógica original de 'served++' ya hacía esto implícitamente si el depósito no estaba en medio de la ruta.
            // El callback de capacidad (node == 0 ? 0 : 1) indica que los nodos cliente tienen demanda 1.
            served_clients++; 
            route_dist += routing.GetArcCostForVehicle(previous_index, index, v);
            previous_index = index;
            index = solution.Value(routing.NextVar(index));
        }
        // Añadir costo del último arco hacia el depósito final
        route_dist += routing.GetArcCostForVehicle(previous_index, index, v);
        // No se añade "->" al nodo final (depósito) en la impresión, lo cual es común.
        // route_ss << " -> " << manager.IndexToNode(index).value(); // Opcional si quieres mostrar el regreso al depot

        std::cout << route_ss.str() << "\n"
                  << "  Clientes servidos: " << served_clients
                  << ", Distancia de la ruta: " << route_dist << "\n";
        total_distance += route_dist;
    }
    std::cout << "\n=== RESUMEN ===\n"
              << "Vehículos usados: " << vehicles_used << " / "
              << data.num_vehicles << "\n"
              << "Distancia total de todas las rutas: " << total_distance / 10  << "\n"
              << "Tiempo de resolución del solver (ms): " << routing.solver()->wall_time()
              << "\n";
}

// Guarda solución para graficar
void SaveSolutionForPlotting(DataModel& data,
                              RoutingIndexManager& manager,
                              RoutingModel& routing,
                              const Assignment& solution) { // Se hizo const Assignment&
    std::ofstream out("solution_plot.txt");
    if (!out.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo solution_plot.txt para escritura.\n";
        return;
    }
    out << "# Nodo x y\n";
    for (size_t i = 0; i < data.coordinates.size(); ++i) { // Usar size_t para el bucle
        out << i << " " << data.coordinates[i].x
            << " " << data.coordinates[i].y << "\n";
    }
    out << "\n# Rutas (Formato: IdVehiculo nodo1 nodo2 ... nodoFinal)\n"; // Clarificación del formato
    for (int v = 0; v < data.num_vehicles; ++v) {
        int64_t idx = routing.Start(v);
        // Si la siguiente variable del inicio es el final, el vehículo no se usa.
        if (routing.IsEnd(solution.Value(routing.NextVar(idx)))) continue;

        out << v; // Id del vehículo
        while (!routing.IsEnd(idx)) {
            out << " " << manager.IndexToNode(idx).value();
            idx = solution.Value(routing.NextVar(idx));
        }
        out << " " << manager.IndexToNode(idx).value(); // Imprime el nodo final (depósito)
        out << "\n";
    }
    out.close();
    std::cout << "Archivo solution_plot.txt generado.\n";
}

// Resuelve el VRP
void SolveVRP() {
    DataModel data;
    std::cout << "Iniciando VRP...\n";
    if (!ReadDistanceMatrix("Dist.txt", data.distance_matrix) ||
        !ReadCoordinates("Coord.txt", data.coordinates)) {
        std::cerr << "Fallo al leer los archivos de datos. Abortando.\n";
        return;
    }
    
    std::cout << "Número de nodos leídos: " << data.distance_matrix.size() << "\n";

    auto overall_start_time = std::chrono::high_resolution_clock::now();
    RoutingIndexManager manager(data.distance_matrix.size(),
                                data.num_vehicles, data.depot);
    RoutingModel routing(manager);

    // Callback de distancias
    const int transit_callback_index = routing.RegisterTransitCallback(
        [&data, &manager](int64_t from_index, int64_t to_index) -> int64_t {
            int from_node = manager.IndexToNode(from_index).value();
            int to_node = manager.IndexToNode(to_index).value();
            return data.distance_matrix[from_node][to_node];
        });
    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);

    // Callback de capacidad (demanda)
    const int demand_callback_index = routing.RegisterUnaryTransitCallback(
        [&manager, &data](int64_t from_index) -> int64_t {
            int from_node = manager.IndexToNode(from_index).value();
            if (from_node == data.depot.value()) { // Usar data.depot.value() para comparación
                return 0; // El depósito no tiene demanda
            }
            return 1; // Cada cliente tiene una demanda de 1
        });
    
    routing.AddDimensionWithVehicleCapacity(
        demand_callback_index,
        0, // Sin holgura (slack) para la capacidad
        std::vector<int64_t>(data.num_vehicles, data.max_clients_per_vehicle), // Capacidades por vehículo
        true, // Iniciar acumulador a cero en el depósito
        "Capacity");

    // Dimensión de distancia (opcional si solo se usa para SetGlobalSpanCostCoefficient con 0)
    // Puede ser útil para otras restricciones o para algunas heurísticas.
    std::string distance_dimension_name = "Distance";
    routing.AddDimension(
        transit_callback_index,
        0,        // Sin holgura (slack)
        300000,   // Un valor suficientemente grande para la distancia máxima por ruta si no hay límite explícito
        true,     // Iniciar acumulador de distancia a cero en el depósito
        distance_dimension_name);
    RoutingDimension* distance_dimension = routing.GetMutableDimension(distance_dimension_name);
    // distance_dimension->SetGlobalSpanCostCoefficient(100); // Original
    distance_dimension->SetGlobalSpanCostCoefficient(0); // CORREGIDO/SUGERIDO
                                                         // Esto evita que la distancia se cuente dos veces en el objetivo
                                                         // si el objetivo principal es minimizar la distancia total
                                                         // establecida por SetArcCostEvaluatorOfAllVehicles.

    // Parámetros de búsqueda
    RoutingSearchParameters search_parameters = DefaultRoutingSearchParameters();
    search_parameters.set_first_solution_strategy(
        FirstSolutionStrategy::PATH_CHEAPEST_ARC);
    search_parameters.set_local_search_metaheuristic(
        LocalSearchMetaheuristic::GUIDED_LOCAL_SEARCH);
    search_parameters.set_log_search(true);
    
    google::protobuf::Duration* time_limit = search_parameters.mutable_time_limit();
    time_limit->set_seconds(data.time_limit_seconds);

    std::cout << "Resolviendo el problema de VRP (límite de tiempo: " << data.time_limit_seconds
              << " segundos)...\n";
    
    const Assignment* solution = routing.SolveWithParameters(search_parameters);
    
    auto overall_end_time = std::chrono::high_resolution_clock::now();
    auto overall_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        overall_end_time - overall_start_time);

    if (solution) {
        std::cout << "Solución encontrada en " << overall_duration_ms.count() << " ms (tiempo total de la función SolveVRP).\n";
        PrintSolution(data, manager, routing, *solution);
        SaveSolutionForPlotting(data, manager, routing, *solution);
    } else {
        std::cout << "No se encontró solución en el tiempo límite especificado.\n";
        std::cout << "Estado del solver: " << routing.status() << std::endl;

    }
}

int main(int argc, char** argv) { // Añadido argc, argv por convención
    std::cout << "VRP Solver con OR-Tools C++\n";
    SolveVRP();
    return 0;
}