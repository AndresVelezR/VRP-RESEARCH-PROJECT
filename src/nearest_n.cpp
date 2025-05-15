#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <regex>
#include <cmath>
#include <cstdint>
#include <chrono>

struct Point {
    double x;
    double y;
};

struct DataModel {
    std::vector<std::vector<int64_t>> distance_matrix;
    std::vector<Point> coordinates;
    int max_clients_per_vehicle = 12;
    int depot = 0;
};

// Leer distancias (idéntico a tu código actual)
bool ReadDistanceMatrix(const std::string& filename, std::vector<std::vector<int64_t>>& distance_matrix) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    distance_matrix.clear();
    std::string line;
    std::regex ws_re("\\s+");

    while (std::getline(file, line)) {
        std::vector<int64_t> row;
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
        if (line.empty()) continue;

        std::sregex_token_iterator iter(line.begin(), line.end(), ws_re, -1);
        std::sregex_token_iterator end;
        for (; iter != end; ++iter) {
            if (!iter->str().empty()) {
                double val = std::stod(iter->str());
                row.push_back(static_cast<int64_t>(val + 0.5));
            }
        }
        distance_matrix.push_back(row);
    }
    return distance_matrix.size() == 200 && distance_matrix[0].size() == 200;
}

// Leer coordenadas
bool ReadCoordinates(const std::string& filename, std::vector<Point>& coordinates) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    coordinates.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Point p;
        if (iss >> p.x >> p.y) {
            coordinates.push_back(p);
        }
    }
    return coordinates.size() == 200;
}

void SolveNearestNeighborVRP(const DataModel& data) {
    std::vector<bool> visited(data.distance_matrix.size(), false);
    visited[data.depot] = true;

    int64_t total_distance = 0;
    int client_count = 199;
    int client_index = 0;
    int vehicle_id = 0;

    std::cout << "\n=== RESULTADOS VECINO MÁS PRÓXIMO ===\n";

    while (client_count > 0) {
        std::cout << "Vehículo " << vehicle_id << ": ";
        int current = data.depot;
        int served = 0;
        int64_t route_distance = 0;

        std::cout << current;

        while (served < data.max_clients_per_vehicle && client_count > 0) {
            int next = -1;
            int64_t min_dist = std::numeric_limits<int64_t>::max();
            for (int i = 1; i < data.distance_matrix.size(); ++i) {
                if (!visited[i] && data.distance_matrix[current][i] < min_dist) {
                    next = i;
                    min_dist = data.distance_matrix[current][i];
                }
            }

            if (next == -1) break;  // No hay más nodos visitables
            route_distance += min_dist;
            std::cout << " -> " << next;
            visited[next] = true;
            current = next;
            served++;
            client_count--;
        }

        // Volver al depósito
        route_distance += data.distance_matrix[current][data.depot];
        std::cout << " -> " << data.depot;
        std::cout << "\n  Clientes servidos: " << served
                  << ", Distancia de la ruta: " << route_distance << "\n";
        total_distance += route_distance;
        vehicle_id++;
    }

    std::cout << "\n=== RESUMEN VECINO MÁS PRÓXIMO ===\n";
    std::cout << "Vehículos usados: " << vehicle_id << "\n";
    std::cout << "Distancia total: " << total_distance << "\n";
}

int main() {
    DataModel data;

    std::cout << "== VRP Vecino Más Próximo ==\n";
    if (!ReadDistanceMatrix("./txt/Dist.txt", data.distance_matrix)) {
        std::cerr << "Error al leer Dist.txt\n";
        return 1;
    }

    if (!ReadCoordinates("./txt/Coord.txt", data.coordinates)) {
        std::cerr << "Error al leer Coord.txt\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    SolveNearestNeighborVRP(data);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nTiempo de ejecución total: " << duration_ms.count() << " ms\n";

    return 0;
}
