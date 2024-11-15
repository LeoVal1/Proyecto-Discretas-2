#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int MAX_NODOS = 100;

class Nodo {
public:
    int x, y;
    int g, h, f;
    Nodo* padre;

    Nodo(int x = 0, int y = 0) {
        this->x = x;
        this->y = y;
        g = h = f = 0;
        padre = NULL;
    }
};

int heuristica(Nodo* actual, Nodo* objetivo) {
    return abs(actual->x - objetivo->x) + abs(actual->y - objetivo->y);
}

bool estaEnLista(Nodo* lista[], int tam, Nodo* nodo) {
    for (int i = 0; i < tam; i++) {
        if (lista[i]->x == nodo->x && lista[i]->y == nodo->y) {
            return true;
        }
    }
    return false;
}

Nodo* encontrarEnLista(Nodo* lista[], int tam, Nodo* nodo) {
    for (int i = 0; i < tam; i++) {
        if (lista[i]->x == nodo->x && lista[i]->y == nodo->y) {
            return lista[i];
        }
    }
    return NULL;
}

Nodo* encontrarMenorF(Nodo* lista[], int tam) {
    Nodo* mejor = lista[0];
    for (int i = 1; i < tam; i++) {
        if (lista[i]->f < mejor->f) {
            mejor = lista[i];
        }
    }
    return mejor;
}

void reconstruirCamino(Nodo* actual) {
    Nodo* temp = actual;
    cout << "Camino encontrado:\n";
    while (temp != NULL) {
        cout << "(" << temp->x << ", " << temp->y << ")\n";
        temp = temp->padre;
    }
}

bool AStar(int inicioX, int inicioY, int objetivoX, int objetivoY, int filas, int columnas, int mapa[5][5]) {
    Nodo nodoInicio(inicioX, inicioY);
    Nodo nodoObjetivo(objetivoX, objetivoY);

    Nodo* listaAbierta[MAX_NODOS];
    Nodo* listaCerrada[MAX_NODOS];

    int tamAbierta = 0;
    int tamCerrada = 0;

    listaAbierta[tamAbierta++] = new Nodo(nodoInicio);

    while (tamAbierta > 0) {
        Nodo* actual = encontrarMenorF(listaAbierta, tamAbierta);

        if (actual->x == nodoObjetivo.x && actual->y == nodoObjetivo.y) {
            reconstruirCamino(actual);
            return true;
        }

        for (int i = 0; i < tamAbierta; i++) {
            if (listaAbierta[i] == actual) {
                for (int j = i; j < tamAbierta - 1; j++) {
                    listaAbierta[j] = listaAbierta[j + 1];
                }
                tamAbierta--;
                break;
            }
        }

        listaCerrada[tamCerrada++] = actual;

        Nodo vecinos[4] = {
            Nodo(actual->x - 1, actual->y),
            Nodo(actual->x + 1, actual->y),
            Nodo(actual->x, actual->y - 1),
            Nodo(actual->x, actual->y + 1)
        };

        for (int i = 0; i < 4; i++) {
            Nodo* vecino = &vecinos[i];

            if (vecino->x < 0 || vecino->x >= filas || vecino->y < 0 || vecino->y >= columnas) {
                continue;
            }

            if (mapa[vecino->x][vecino->y] == 2) {
                continue;
            }

            if (estaEnLista(listaCerrada, tamCerrada, vecino)) {
                continue;
            }

            vecino->g = actual->g + mapa[vecino->x][vecino->y];
            vecino->h = heuristica(vecino, &nodoObjetivo);
            vecino->f = vecino->g + vecino->h;
            vecino->padre = actual;

            Nodo* nodoEnAbierta = encontrarEnLista(listaAbierta, tamAbierta, vecino);
            if (nodoEnAbierta != NULL) {
                if (vecino->g < nodoEnAbierta->g) {
                    nodoEnAbierta->g = vecino->g;
                    nodoEnAbierta->f = vecino->f;
                    nodoEnAbierta->padre = actual;
                }
            } else {
                listaAbierta[tamAbierta++] = new Nodo(*vecino);
            }
        }
    }

    cout << "No se encontró un camino.\n";
    return false;
}

void simularTrafico(int filas, int columnas, int mapa[5][5]) {
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (mapa[i][j] == 1) {
                int cambio = rand() % 3 - 1;
                mapa[i][j] = max(1, mapa[i][j] + cambio);
            }
        }
    }
}

void mostrarMapa(int filas, int columnas, int mapa[5][5]) {
    cout << "Mapa actualizado:\n";
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            cout << mapa[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {
    srand(time(0));

    int mapa[5][5] = {
        {1, 1, 2, 1, 1},
        {1, 3, 1, 1, 1},
        {1, 1, 1, 2, 1},
        {2, 1, 1, 2, 1},
        {1, 2, 1, 1, 1}
    };

    int inicioX, inicioY;
    int objetivoX, objetivoY;
    cout << "Inicio en x: "; cin >> inicioX;
    cout << "Inicio en y: "; cin >> inicioY;
    cout << "Objetivo en x: "; cin >> objetivoX;
    cout << "Objetivo en y: "; cin >> objetivoY;

    while (true) {
        simularTrafico(5, 5, mapa);
        mostrarMapa(5, 5, mapa);
        if (!AStar(inicioX, inicioY, objetivoX, objetivoY, 5, 5, mapa)) {
            cout << "No se encontró un camino. Simulación continua.\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
