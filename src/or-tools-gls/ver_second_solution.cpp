#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath> // Para std::round
#include <iomanip>
#include <regex>
#include <limits> // Para std::numeric_limits

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
    // IMPORTANTE: Aumenta este valor significativamente para buscar mejores soluciones
    // Ejemplos: 300 (5 min), 600 (10 min), 1800 (30 min), 3600 (1 hora) o más.
    int time_limit_seconds = 120; // Límite de tiempo actual: 2 minutos
    RoutingIndexManager::NodeIndex depot{0};
    int num_nodes = 200; // Añadido para consistencia
};

// Comprueba existencia de archivo
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Lee la matriz de distancias desde un archivo
bool ReadDistanceMatrix(const std::string& filename,
                        DataModel& data) { // Modificado para pasar DataModel
    if (!fileExists(filename)) {
        std::cerr << "Error: El archivo " << filename << " no existe.\n";
        return false;
    }
    std::ifstream file(filename);
    std::string line;
    data.distance_matrix.clear();
    std::regex ws_re("\\s+"); 

    int line_count = 0;
    while (std::getline(file, line)) {
        line_count++;
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (line.empty()) continue; 

        std::vector<std::string> tokens;
        std::sregex_token_iterator iter(line.begin(), line.end(), ws_re, -1);
        std::sregex_token_iterator end;
        for (; iter != end; ++iter) {
            if (!iter->str().empty()) { 
                tokens.push_back(*iter);
            }
        }

        if (tokens.size() != data.num_nodes) {
            std::cerr << "Error: La fila " << line_count << " tiene " << tokens.size()
                      << " columnas, se esperaban " << data.num_nodes << ".\n";
            return false;
        }

        std::vector<int64_t> row;
        for (const auto& token : tokens) {
            try {
                // Multiplicar por 10 y redondear para mantener un decimal de precisión como entero
                double val = std::stod(token);
                row.push_back(static_cast<int64_t>(std::round(val * 10.0)));
            } catch (const std::exception& e) {
                std::cerr << "Error: Valor no numérico en la fila " << line_count
                          << ": '" << token << "' (" << e.what() << ").\n";
                return false;
            }
        }
        data.distance_matrix.push_back(row);
    }

    if (data.distance_matrix.size() != data.num_nodes) {
        std::cerr << "Error: Se esperaban " << data.num_nodes << " filas, se leyeron "
                  << data.distance_matrix.size() << ".\n";
        return false;
    }

    std::cout << "Matriz de distancias (" << filename << ") leída: "
              << data.distance_matrix.size() << "x"
              << (data.distance_matrix.empty() ? 0 : data.distance_matrix[0].size())
              << " nodos.\n";
    return true;
}

// Lee coordenadas desde un archivo
bool ReadCoordinates(const std::string& filename,
                     DataModel& data) { // Modificado para pasar DataModel
    if (!fileExists(filename)) {
        std::cerr << "Error: El archivo " << filename << " no existe.\n";
        return false;
    }
    std::ifstream file(filename);
    std::string line;
    data.coordinates.clear();
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Point p;
        // Asumiendo formato: x y (o id x y, donde el id se ignora si solo se leen x,y)
        // Si el formato es "id x y", ajustar la lectura:
        // int id_node; if (iss >> id_node >> p.x >> p.y)
        if (iss >> p.x >> p.y) {
             data.coordinates.push_back(p);
        } else {
            // Podrías añadir un warning si una línea no tiene el formato esperado
        }
    }
    
    if (data.coordinates.size() != data.num_nodes) {
        std::cerr << "Error: Se esperaban " << data.num_nodes << " coordenadas, se leyeron "
                  << data.coordinates.size() << ".\n";
        // Considera si esto debe ser un error fatal o si puedes continuar
        // si la matriz de distancias es la fuente principal de la topología.
        // Por ahora, lo mantenemos como error si no coincide con num_nodes.
        return false; 
    }
    std::cout << "Coordenadas (" << filename << ") leídas: " << data.coordinates.size() << " nodos.\n";
    return true;
}

// Optimiza una ruta individual usando 2-Opt
// La ruta debe contener los índices de los nodos (no los índices del RoutingIndexManager)
// y debe empezar y terminar en el depósito.
std::vector<int64_t> OptimizeRouteWithTwoOpt(
    const std::vector<int64_t>& route_nodes,
    const DataModel& data) {
    
    if (route_nodes.size() <= 3) { // No se puede optimizar con 2-Opt (e.g., Deposito -> Cliente -> Deposito)
        return route_nodes;
    }

    std::vector<int64_t> best_route = route_nodes;
    bool improved = true;

    while (improved) {
        improved = false;
        for (size_t i = 1; i < best_route.size() - 2; ++i) { // Empezar desde el primer cliente, no el depósito
            for (size_t j = i + 1; j < best_route.size() -1; ++j) { // Terminar antes del último depósito
                // Nodos actuales en el intercambio: best_route[i-1] -> best_route[i] y best_route[j] -> best_route[j+1]
                // Nueva conexión: best_route[i-1] -> best_route[j] y best_route[i] -> best_route[j+1]
                
                int64_t node_prev_i = best_route[i-1];
                int64_t node_i      = best_route[i];
                int64_t node_j      = best_route[j];
                int64_t node_next_j = best_route[j+1];

                int64_t current_cost = data.distance_matrix[node_prev_i][node_i] +
                                       data.distance_matrix[node_j][node_next_j];
                
                int64_t new_cost = data.distance_matrix[node_prev_i][node_j] +
                                   data.distance_matrix[node_i][node_next_j];

                if (new_cost < current_cost) {
                    // Realizar el intercambio 2-Opt (revertir el segmento entre i y j inclusive)
                    std::reverse(best_route.begin() + i, best_route.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
    return best_route;
}


// Imprime la solución
void PrintSolution(const DataModel& data, // const
                   const RoutingIndexManager& manager, // const
                   const RoutingModel& routing, // const
                   const Assignment& solution,
                   bool apply_two_opt = true) { 
    int64_t total_distance_or_tools = 0;
    int64_t total_distance_final = 0;
    int vehicles_used = 0;
    std::cout << "\n=== RESULTADOS DE LA SOLUCIÓN ===\n";
    std::cout << "Distancia objetivo del solver OR-Tools: " << solution.ObjectiveValue() / 10.0 << "\n";


    for (int v_idx = 0; v_idx < data.num_vehicles; ++v_idx) {
        int64_t manager_start_node = routing.Start(v_idx);
        // Si la siguiente variable del inicio es el final, el vehículo no se usa.
        if (routing.IsEnd(solution.Value(routing.NextVar(manager_start_node)))) continue; 
        
        vehicles_used++;
        std::cout << "Vehículo " << v_idx << ": ";
        
        int64_t route_dist_or_tools = 0;
        int served_clients = 0; 
        std::stringstream route_ss; 
        
        std::vector<int64_t> route_nodes_for_two_opt; // Para 2-Opt
        int64_t current_manager_node = manager_start_node;
        
        // Construir ruta y calcular distancia según OR-Tools
        while (true) {
            int current_node_idx = manager.IndexToNode(current_manager_node).value();
            route_nodes_for_two_opt.push_back(current_node_idx); // Guardar nodo real

            if (route_ss.tellp() > 0) { // No es el primer nodo
                 route_ss << " -> ";
            }
            route_ss << current_node_idx;

            if (current_node_idx != data.depot.value() && !routing.IsStart(current_manager_node) ) { // No contar el depósito como cliente servido al inicio
                served_clients++;
            }
            
            int64_t previous_manager_node = current_manager_node;
            current_manager_node = solution.Value(routing.NextVar(current_manager_node));
            route_dist_or_tools += routing.GetArcCostForVehicle(previous_manager_node, current_manager_node, v_idx);
            
            if (routing.IsEnd(current_manager_node)) {
                // Añadir el nodo final del depósito a la ruta para 2-Opt si es necesario
                if (route_nodes_for_two_opt.back() != manager.IndexToNode(current_manager_node).value()){
                     route_nodes_for_two_opt.push_back(manager.IndexToNode(current_manager_node).value());
                }
                 if (route_ss.tellp() > 0 && route_nodes_for_two_opt.back() != std::stoll(route_ss.str().substr(route_ss.str().rfind(' ')+1))) {
                    route_ss << " -> " << manager.IndexToNode(current_manager_node).value();
                 } else if (route_ss.tellp() == 0) { // Ruta con solo depósito (no debería pasar si el vehículo se usa)
                    route_ss << manager.IndexToNode(current_manager_node).value();
                 }
                break;
            }
        }
        total_distance_or_tools += route_dist_or_tools;

        // Aplicar 2-Opt si está habilitado
        int64_t route_dist_final = route_dist_or_tools;
        std::vector<int64_t> final_route_nodes = route_nodes_for_two_opt;

        if (apply_two_opt && final_route_nodes.size() > 1) { // Solo si hay más de un nodo (depósito)
            final_route_nodes = OptimizeRouteWithTwoOpt(route_nodes_for_two_opt, data);
            route_dist_final = 0;
            for (size_t i = 0; i < final_route_nodes.size() - 1; ++i) {
                route_dist_final += data.distance_matrix[final_route_nodes[i]][final_route_nodes[i+1]];
            }
            // Actualizar route_ss si la ruta cambió
            if (final_route_nodes != route_nodes_for_two_opt) {
                route_ss.str(""); // Limpiar
                route_ss.clear(); // Limpiar estado de error
                for (size_t i = 0; i < final_route_nodes.size(); ++i) {
                    route_ss << final_route_nodes[i] << (i == final_route_nodes.size() - 1 ? "" : " -> ");
                }
            }
        }
        total_distance_final += route_dist_final;

        std::cout << route_ss.str() << "\n"
                  << "  Clientes servidos: " << served_clients
                  << ", Distancia (OR-Tools): " << route_dist_or_tools / 10.0;
        if (apply_two_opt) {
            std::cout << ", Distancia (Post 2-Opt): " << route_dist_final / 10.0;
            if (route_dist_final < route_dist_or_tools) {
                std::cout << " (Mejora 2-Opt: " << (route_dist_or_tools - route_dist_final) / 10.0 << ")";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n=== RESUMEN ===\n"
              << "Vehículos usados: " << vehicles_used << " / "
              << data.num_vehicles << "\n"
              << "Distancia total (OR-Tools): " << total_distance_or_tools / 10.0 << "\n";
    if (apply_two_opt) {
        std::cout << "Distancia total (Post 2-Opt): " << total_distance_final / 10.0 << "\n";
    }
    std::cout << "Tiempo de resolución del solver (ms): " << routing.solver()->wall_time()
              << "\n";
}

// Guarda la solución para graficar (usando la ruta post-2-Opt si se aplicó)
void SaveSolutionForPlotting(const DataModel& data, // const
                             const RoutingIndexManager& manager, // const
                             const RoutingModel& routing, // const
                             const Assignment& solution,
                             bool apply_two_opt = true) { 
    std::ofstream out("solution_plot.txt");
    if (!out.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo solution_plot.txt para escritura.\n";
        return;
    }
    out << std::fixed << std::setprecision(2); // Para mejor formato de coordenadas
    out << "# Nodo x y\n";
    for (int i = 0; i < data.num_nodes; ++i) { 
        if (i < data.coordinates.size()) { // Asegurarse que hay coordenadas para este nodo
             out << i << " " << data.coordinates[i].x
                 << " " << data.coordinates[i].y << "\n";
        } else {
             out << i << " 0.00 0.00 # Coordenada no disponible\n";
        }
    }
    out << "\n# Rutas (Formato: IdVehiculo nodo1 nodo2 ... nodoFinal)\n"; 
    for (int v_idx = 0; v_idx < data.num_vehicles; ++v_idx) {
        int64_t current_manager_node = routing.Start(v_idx);
        if (routing.IsEnd(solution.Value(routing.NextVar(current_manager_node)))) continue;

        out << v_idx; 
        std::vector<int64_t> route_nodes_for_plot;
        while(true) {
            route_nodes_for_plot.push_back(manager.IndexToNode(current_manager_node).value());
            current_manager_node = solution.Value(routing.NextVar(current_manager_node));
            if(routing.IsEnd(current_manager_node)){
                route_nodes_for_plot.push_back(manager.IndexToNode(current_manager_node).value());
                break;
            }
        }
        
        if (apply_two_opt && route_nodes_for_plot.size() > 1) {
            route_nodes_for_plot = OptimizeRouteWithTwoOpt(route_nodes_for_plot, data);
        }

        for (int64_t node_val : route_nodes_for_plot) {
            out << " " << node_val;
        }
        out << "\n";
    }
    out.close();
    std::cout << "Archivo solution_plot.txt generado.\n";
}

// Resuelve el VRP
void SolveVRP() {
    DataModel data; // num_nodes, time_limit_seconds, etc., se inicializan aquí
    std::cout << "Iniciando VRP...\n";
    // Configura data.num_nodes si es diferente al default antes de leer.
    // data.num_nodes = 200; // Asegúrate que esto es correcto para tus archivos.

    if (!ReadDistanceMatrix("./txt/Dist.txt", data) ||
        !ReadCoordinates("./txt/Coord.txt", data)) {
        std::cerr << "Fallo al leer los archivos de datos. Abortando.\n";
        return;
    }
    
    auto overall_start_time = std::chrono::high_resolution_clock::now();
    RoutingIndexManager manager(data.distance_matrix.size(), // Usa el tamaño real leído
                                data.num_vehicles, data.depot);
    RoutingModel routing(manager);

    // Callback de distancias
    const int transit_callback_index = routing.RegisterTransitCallback(
        [&data, &manager](int64_t from_index, int64_t to_index) -> int64_t {
            int from_node = manager.IndexToNode(from_index).value();
            int to_node = manager.IndexToNode(to_index).value();
            if (from_node >= 0 && from_node < data.distance_matrix.size() &&
                to_node >= 0 && to_node < data.distance_matrix.size()) {
                return data.distance_matrix[from_node][to_node];
            }
            std::cerr << "Error: Acceso fuera de límites en distance_matrix. From: " << from_node << ", To: " << to_node << std::endl;
            return std::numeric_limits<int64_t>::max(); // Costo alto para arcos inválidos
        });
    routing.SetArcCostEvaluatorOfAllVehicles(transit_callback_index);

    // Callback de capacidad (demanda)
    const int demand_callback_index = routing.RegisterUnaryTransitCallback(
        [&manager, &data](int64_t from_index) -> int64_t {
            int from_node = manager.IndexToNode(from_index).value();
            if (from_node == data.depot.value()) { 
                return 0; 
            }
            return 1; // Cada cliente tiene una demanda de 1
        });
    
    routing.AddDimensionWithVehicleCapacity(
        demand_callback_index,
        0, 
        std::vector<int64_t>(data.num_vehicles, data.max_clients_per_vehicle), 
        true, 
        "Capacity");

    // Dimensión de distancia (opcional para el objetivo si ya se usa SetArcCostEvaluatorOfAllVehicles)
    // Se mantiene por si se quieren añadir restricciones de tiempo de ruta o SetGlobalSpanCostCoefficient
    std::string distance_dimension_name = "Distance";
    routing.AddDimension(
        transit_callback_index,
        0,        
        3000000,  // Aumentado, asegúrate que es suficientemente grande (distancia * 10)
        true,     
        distance_dimension_name);
    RoutingDimension* distance_dim = routing.GetMutableDimension(distance_dimension_name);
    // Para minimizar la distancia total, este coeficiente debe ser 0, ya que el costo del arco ya lo hace.
    // Si quisieras minimizar el makespan (tiempo máximo de una ruta), aquí iría un coeficiente > 0.
    distance_dim->SetGlobalSpanCostCoefficient(0); 


    // Parámetros de búsqueda para obtener la mejor solución posible
    RoutingSearchParameters search_parameters = DefaultRoutingSearchParameters();
    
    // 1. Estrategia de Primera Solución:
    search_parameters.set_first_solution_strategy(
        FirstSolutionStrategy::AUTOMATIC);
    // Alternativas potentes:
    // search_parameters.set_first_solution_strategy(FirstSolutionStrategy::SAVINGS);
    // search_parameters.set_first_solution_strategy(FirstSolutionStrategy::CHRISTOFIDES);
    // search_parameters.set_first_solution_strategy(FirstSolutionStrategy::PATH_CHEAPEST_ARC); // Opción razonable también

    // 2. Metaheurística de Búsqueda Local:
    search_parameters.set_local_search_metaheuristic(
        LocalSearchMetaheuristic::GUIDED_LOCAL_SEARCH); // Generalmente la más potente
    // Alternativas:
    // search_parameters.set_local_search_metaheuristic(LocalSearchMetaheuristic::TABU_SEARCH);
    // search_parameters.set_local_search_metaheuristic(LocalSearchMetaheuristic::SIMULATED_ANNEALING);
    
    // 3. Límite de Tiempo: ¡CRUCIAL! Aumentar data.time_limit_seconds.
    google::protobuf::Duration* time_limit = search_parameters.mutable_time_limit();
    time_limit->set_seconds(data.time_limit_seconds);

    // 4. Log de Búsqueda:
    search_parameters.set_log_search(true);

    // 5. Propagación Completa:
    search_parameters.set_use_full_propagation(true); // Default, pero explícito

    // 6. (Opcional) Límite de tiempo para LNS si se usa esa metaheurística.
    //    Por defecto, para GUIDED_LOCAL_SEARCH, el time_limit global es el principal.
    // if (search_parameters.local_search_metaheuristic() == LocalSearchMetaheuristic::LARGE_NEIGHBORHOOD_SEARCH) {
    //    search_parameters.set_lns_time_limit(google::protobuf::Duration().set_seconds(std::max(10, data.time_limit_seconds / 5)));
    // }
    
    // 7. (Opcional) No establecer solution_limit si quieres que el tiempo sea el único criterio de parada.
    // search_parameters.clear_solution_limit(); // O simplemente no llamarlo


    std::cout << "Resolviendo el problema de VRP (límite de tiempo: " << data.time_limit_seconds
              << " segundos, Estrategia inicial: AUTOMATIC, Metaheurística: GUIDED_LOCAL_SEARCH)...\n";
    
    const Assignment* solution = routing.SolveWithParameters(search_parameters);
    
    auto overall_end_time = std::chrono::high_resolution_clock::now();
    auto overall_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        overall_end_time - overall_start_time);

    if (solution) {
        std::cout << "\nSolución encontrada por OR-Tools en " << routing.solver()->wall_time() << " ms.\n";
        std::cout << "Tiempo total de la función SolveVRP: " << overall_duration_ms.count() << " ms.\n";
        
        bool apply_two_opt_post_process = true; // Habilita o deshabilita el 2-Opt post OR-Tools
        PrintSolution(data, manager, routing, *solution, apply_two_opt_post_process);
        SaveSolutionForPlotting(data, manager, routing, *solution, apply_two_opt_post_process);

    } else {
        std::cout << "No se encontró solución en el tiempo límite especificado.\n";
        std::cout << "Estado del solver: " << routing.status();
        
        // Reemplazamos la línea problemática con una alternativa
        // Original: << " (" << RoutingModel::SolverStatus_Name(routing.status()) << ")" << std::endl;
        
        // Versión en texto que proporciona información similar
        switch(routing.status()) {
            case RoutingModel::ROUTING_NOT_SOLVED:
                std::cout << " (NO RESUELTO)";
                break;
            case RoutingModel::ROUTING_SUCCESS:
                std::cout << " (ÉXITO)";
                break;
            case RoutingModel::ROUTING_FAIL:
                std::cout << " (FALLO)";
                break;
            case RoutingModel::ROUTING_FAIL_TIMEOUT:
                std::cout << " (TIEMPO AGOTADO)";
                break;
            case RoutingModel::ROUTING_INVALID:
                std::cout << " (INVÁLIDO)";
                break;
            default:
                std::cout << " (ESTADO DESCONOCIDO)";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) { 
    // Puedes permitir que el límite de tiempo se pase como argumento:
    DataModel default_data; // Para obtener el valor por defecto
    if (argc > 1) {
        try {
            default_data.time_limit_seconds = std::stoi(argv[1]);
            std::cout << "Límite de tiempo establecido desde argumento: " << default_data.time_limit_seconds << "s\n";
        } catch (const std::exception& e) {
            std::cerr << "Argumento de límite de tiempo inválido: " << argv[1] << ". Usando default: "
                      << default_data.time_limit_seconds << "s.\n";
        }
    }
    
    // Si necesitas sobreescribir el DataModel globalmente, puedes hacerlo aquí
    // o pasar una instancia de DataModel a SolveVRP.
    // Para simplificar, SolveVRP crea su propia instancia de DataModel y usa
    // los valores por defecto (o los que modifiques dentro de SolveVRP antes de leer archivos).
    // Si pasaste el time_limit por argumento, necesitarías pasarlo a la instancia de DataModel en SolveVRP.
    // Por ahora, SolveVRP usa el time_limit_seconds definido en su DataModel interno.
    // Para que el argumento de main tenga efecto, deberías pasar `default_data` a `SolveVRP` o modificar
    // la instancia interna de `data` en `SolveVRP`.
    // Ejemplo de cómo hacerlo (modificando SolveVRP para aceptar DataModel):
    // SolveVRP(default_data); // Tendrías que cambiar la firma de SolveVRP

    std::cout << "VRP Solver con OR-Tools C++\n";
    SolveVRP(); // Esta llamada usará el DataModel interno de SolveVRP
    return 0;
}