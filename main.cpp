#include <iostream>
#include <cmath>
#include <limits>
#include <queue>
#include <fstream>
#include <cstdlib>

using namespace std;

const int INF = numeric_limits<int>::max();
const int FILAS = 10;
const int COLUMNAS = 10;

class Nodo {
public:
    int x, y;
    int g, h, f;
    Nodo* padre;

    Nodo(int x = 0, int y = 0) {
        this->x = x;
        this->y = y;
        g = h = f = 0;
        padre = nullptr;
    }
};

int heuristicaMultivariada(Nodo* actual, Nodo* objetivo, int congestion, int seguridad, int huellaCarbono, float alpha, float beta, float gamma) {
    int distancia = abs(actual->x - objetivo->x) + abs(actual->y - objetivo->y);
    return static_cast<int>(alpha * distancia + beta * congestion + gamma * seguridad + huellaCarbono);
}

void exportarAGraphviz(int mapa[FILAS][COLUMNAS], Nodo* camino) {
    ofstream archivo("grafo.dot");
    archivo << "graph G {\n";
    
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            if (j + 1 < COLUMNAS) {
                archivo << "    \"(" << i << "," << j << ")\" -- \"(" << i << "," << (j + 1) << ")\" [color=black];\n";
            }
            if (i + 1 < FILAS) {
                archivo << "    \"(" << i << "," << j << ")\" -- \"(" << (i + 1) << "," << j << ")\" [color=black];\n";
            }
        }
    }

    Nodo* actual = camino;
    while (actual != nullptr && actual->padre != nullptr) {
        archivo << "    \"(" << actual->x << "," << actual->y << ")\" -- \"(" << actual->padre->x << "," << actual->padre->y << ")\" [color=red, penwidth=2.0];\n";
        actual = actual->padre;
    }

    archivo << "}\n";
    archivo.close();
    cout << "Grafo exportado a 'grafo.dot'. Visualizando...\n";

    // Visualizar el grafo automáticamente
    system("dot -Tpng grafo.dot -o grafo.png");
    system("xdg-open grafo.png"); // Cambiar por "start" en Windows o "open" en MacOS
}

void reconstruirCamino(Nodo* actual) {
    cout << "Camino encontrado:\n";
    while (actual != nullptr) {
        cout << "(" << actual->x << ", " << actual->y << ")\n";
        actual = actual->padre;
    }
}

bool AStar(int inicioX, int inicioY, int objetivoX, int objetivoY, int filas, int columnas, int mapa[FILAS][COLUMNAS], int congestion[FILAS][COLUMNAS], int seguridad[FILAS][COLUMNAS], int huellaCarbono[FILAS][COLUMNAS], float alpha, float beta, float gamma) {
    Nodo nodoInicio(inicioX, inicioY);
    Nodo nodoObjetivo(objetivoX, objetivoY);

    auto comparar = [](Nodo* a, Nodo* b) { return a->f > b->f; };
    priority_queue<Nodo*, deque<Nodo*>, decltype(comparar)> listaAbierta(comparar);
    bool listaCerrada[FILAS][COLUMNAS] = {false};

    listaAbierta.push(new Nodo(nodoInicio));

    while (!listaAbierta.empty()) {
        Nodo* actual = listaAbierta.top();
        listaAbierta.pop();

        if (actual->x == nodoObjetivo.x && actual->y == nodoObjetivo.y) {
            reconstruirCamino(actual);
            exportarAGraphviz(mapa, actual);
            return true;
        }

        if (listaCerrada[actual->x][actual->y]) continue;
        listaCerrada[actual->x][actual->y] = true;

        Nodo vecinos[4] = {
            Nodo(actual->x - 1, actual->y), Nodo(actual->x + 1, actual->y),
            Nodo(actual->x, actual->y - 1), Nodo(actual->x, actual->y + 1)
        };

        for (Nodo& vecino : vecinos) {
            if (vecino.x < 0 || vecino.x >= filas || vecino.y < 0 || vecino.y >= columnas) continue;
            if (listaCerrada[vecino.x][vecino.y] || mapa[vecino.x][vecino.y] == INF) continue;

            vecino.g = actual->g + mapa[vecino.x][vecino.y];
            vecino.h = heuristicaMultivariada(&vecino, &nodoObjetivo, congestion[vecino.x][vecino.y], seguridad[vecino.x][vecino.y], huellaCarbono[vecino.x][vecino.y], alpha, beta, gamma);
            vecino.f = vecino.g + vecino.h;
            vecino.padre = actual;

            listaAbierta.push(new Nodo(vecino));
        }
    }

    cout << "No se encontró un camino.\n";
    return false;
}

void evaluarRobustez(int mapa[FILAS][COLUMNAS], int congestion[FILAS][COLUMNAS], int seguridad[FILAS][COLUMNAS], int huellaCarbono[FILAS][COLUMNAS]) {
    float alphas[] = {1.0, 0.8, 1.2};
    float betas[] = {0.5, 0.7, 0.3};
    float gammas[] = {0.3, 0.4, 0.2};

    cout << "Evaluando configuraciones...\n";
    for (float alpha : alphas) {
        for (float beta : betas) {
            for (float gamma : gammas) {
                cout << "Configuración: alpha=" << alpha << ", beta=" << beta << ", gamma=" << gamma << "\n";
                AStar(0, 0, 9, 9, FILAS, COLUMNAS, mapa, congestion, seguridad, huellaCarbono, alpha, beta, gamma);
                cout << "-----------------------------------------\n";
            }
        }
    }
}

void mostrarMapa(int mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            cout << mapa[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    int mapa[FILAS][COLUMNAS] = {
        {1, 1, 2, 1, 1, 2, 1, 1, 3, 1},
        {1, 3, 1, 1, 2, 3, 1, 1, 2, 1},
        {2, 1, 2, 1, 1, 2, 1, 1, 1, 1},
        {1, 2, 3, 1, 2, 1, 2, 1, 1, 1},
        {1, 1, 1, 2, 1, 1, 2, 1, 1, 1},
        {1, 1, 2, 1, 2, 1, 1, 1, 1, 1},
        {1, 3, 1, 2, 1, 1, 1, 2, 1, 2},
        {2, 1, 1, 1, 1, 2, 1, 1, 1, 1},
        {1, 1, 2, 3, 1, 1, 2, 1, 1, 1},
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 1}
    };

    int congestion[FILAS][COLUMNAS] = {
        {1, 3, 4, 2, 1, 5, 1, 1, 3, 2},
        {2, 1, 5, 3, 2, 1, 3, 1, 4, 2},
        {1, 4, 1, 1, 5, 3, 2, 2, 3, 1},
        {3, 5, 2, 1, 3, 2, 1, 3, 2, 4},
        {1, 1, 2, 4, 1, 3, 4, 2, 1, 3},
        {3, 1, 5, 2, 1, 1, 3, 2, 1, 2},
        {4, 2, 3, 1, 2, 1, 1, 1, 5, 2},
        {1, 1, 1, 4, 2, 3, 2, 2, 1, 1},
        {3, 4, 5, 2, 1, 1, 3, 4, 2, 1},
        {2, 1, 1, 1, 3, 2, 1, 1, 1, 1}
    };

    int seguridad[FILAS][COLUMNAS] = {
        {1, 3, 2, 2, 1, 2, 3, 1, 1, 2},
        {3, 1, 4, 3, 2, 1, 3, 2, 3, 1},
        {1, 4, 1, 2, 3, 2, 2, 2, 3, 1},
        {2, 2, 3, 1, 3, 2, 1, 2, 3, 1},
        {1, 1, 1, 2, 2, 3, 2, 1, 1, 1},
        {1, 2, 2, 3, 1, 2, 2, 2, 3, 1},
        {3, 2, 3, 2, 1, 1, 1, 1, 2, 3},
        {2, 1, 1, 3, 2, 1, 3, 1, 1, 1},
        {3, 2, 3, 1, 2, 1, 2, 3, 2, 1},
        {1, 1, 1, 1, 1, 1, 1, 2, 1, 1}
    };

    int huellaCarbono[FILAS][COLUMNAS] = {
        {1, 2, 3, 2, 1, 3, 2, 1, 3, 2},
        {3, 1, 4, 3, 2, 1, 3, 2, 4, 1},
        {1, 4, 1, 2, 3, 2, 3, 3, 3, 2},
        {3, 5, 2, 1, 3, 2, 1, 3, 2, 3},
        {1, 1, 3, 3, 1, 3, 4, 2, 2, 3},
        {3, 2, 4, 2, 1, 2, 3, 2, 2, 1},
        {4, 3, 3, 1, 2, 1, 1, 2, 3, 2},
        {1, 2, 3, 4, 3, 2, 3, 3, 1, 2},
        {3, 4, 5, 3, 2, 3, 3, 4, 2, 1},
        {2, 2, 2, 3, 3, 3, 2, 2, 2, 1}
    };

    mostrarMapa(mapa);
    evaluarRobustez(mapa, congestion, seguridad, huellaCarbono);

    return 0;
}
