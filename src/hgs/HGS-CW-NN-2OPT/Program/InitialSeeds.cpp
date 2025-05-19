#include "InitialSeeds.h"
     #include <vector>
     #include <algorithm>
     #include <cmath>

     Individual ClarkeWrightSeed(Params &params) {
         Individual ind(params);
         // Implementación simplificada de Clarke-Wright
         std::vector<std::vector<int>> routes;
         std::vector<bool> visited(params.nbClients + 1, false);
         visited[0] = true;

         // Calcular ahorros
         struct Saving {
             int i, j;
             double saving;
         };
         std::vector<Saving> savings;
         for (int i = 1; i <= params.nbClients; ++i) {
             for (int j = i + 1; j <= params.nbClients; ++j) {
                 double dist_i0 = sqrt(pow(params.instance.x_coords[i] - params.instance.x_coords[0], 2) +
                                       pow(params.instance.y_coords[i] - params.instance.y_coords[0], 2));
                 double dist_j0 = sqrt(pow(params.instance.x_coords[j] - params.instance.x_coords[0], 2) +
                                       pow(params.instance.y_coords[j] - params.instance.y_coords[0], 2));
                 double dist_ij = sqrt(pow(params.instance.x_coords[i] - params.instance.x_coords[j], 2) +
                                       pow(params.instance.y_coords[i] - params.instance.y_coords[j], 2));
                 savings.push_back({i, j, dist_i0 + dist_j0 - dist_ij});
             }
         }
         std::sort(savings.begin(), savings.end(), [](const Saving &a, const Saving &b) {
             return a.saving > b.saving;
         });

         // Construir rutas
         for (const auto &s : savings) {
             if (!visited[s.i] && !visited[s.j]) {
                 routes.push_back({0, s.i, s.j, 0});
                 visited[s.i] = visited[s.j] = true;
             } else if (!visited[s.i] || !visited[s.j]) {
                 for (auto &route : routes) {
                     if (route.size() - 2 < 12) { // Respetar capacidad
                         if (!visited[s.i]) {
                             route.insert(route.end() - 1, s.i);
                             visited[s.i] = true;
                         } else if (!visited[s.j]) {
                             route.insert(route.end() - 1, s.j);
                             visited[s.j] = true;
                         }
                     }
                 }
             }
         }

         // Añadir clientes no visitados
         for (int i = 1; i <= params.nbClients; ++i) {
             if (!visited[i]) {
                 for (auto &route : routes) {
                     if (route.size() - 2 < 12) {
                         route.insert(route.end() - 1, i);
                         visited[i] = true;
                         break;
                     }
                 }
             }
         }

         // Convertir a chromT
         int pos = 0;
         for (const auto &route : routes) {
             for (size_t i = 1; i < route.size() - 1; ++i) {
                 ind.chromT[pos++] = route[i];
             }
         }
         ind.eval.nbRoutes = routes.size();
         return ind;
     }

     Individual NearestNeighborSeed(Params &params) {
         Individual ind(params);
         std::vector<bool> visited(params.nbClients + 1, false);
         visited[0] = true;
         std::vector<std::vector<int>> routes(20, {0}); // 20 vehículos
         int routeIdx = 0;

         for (int i = 0; i < params.nbClients; ++i) {
             int current = routes[routeIdx].back();
             double minDist = 1e30;
             int next = -1;
             for (int j = 1; j <= params.nbClients; ++j) {
                 if (!visited[j]) {
                     double dist = sqrt(pow(params.instance.x_coords[current] - params.instance.x_coords[j], 2) +
                                        pow(params.instance.y_coords[current] - params.instance.y_coords[j], 2));
                     if (dist < minDist) {
                         minDist = dist;
                         next = j;
                     }
                 }
             }
             if (next == -1 || routes[routeIdx].size() - 1 >= 12) {
                 routes[routeIdx].push_back(0);
                 routeIdx = (routeIdx + 1) % 20;
                 if (next == -1) break;
             }
             routes[routeIdx].push_back(next);
             visited[next] = true;
         }

         // Convertir a chromT
         int pos = 0;
         for (const auto &route : routes) {
             for (size_t i = 1; i < route.size() - 1; ++i) {
                 ind.chromT[pos++] = route[i];
             }
         }
         ind.eval.nbRoutes = 20;
         return ind;
     }

     Individual RandomKeySeed(Params &params) {
         Individual ind(params);
         std::vector<int> clients(params.nbClients);
         for (int i = 0; i < params.nbClients; ++i) clients[i] = i + 1;
         std::shuffle(clients.begin(), clients.end(), params.ran);
         for (int i = 0; i < params.nbClients; ++i) ind.chromT[i] = clients[i];
         ind.eval.nbRoutes = 20;
         return ind;
     }
