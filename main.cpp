#include <iostream>
#include <cmath>
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

void AStar(int inicioX, int inicioY, int objetivoX, int objetivoY, int filas, int columnas, int mapa[5][5]) {
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
            return;
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

            if (mapa[vecino->x][vecino->y] == 1) {
                continue;
            }

            if (estaEnLista(listaCerrada, tamCerrada, vecino)) {
                continue;
            }

            vecino->g = actual->g + 1;
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
}

int main() {
    int mapa[5][5] = {
        {0, 0, 0, 0, 1},
        {0, 1, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0}
    };

    int inicioX = 0, inicioY = 0;
    int objetivoX = 4, objetivoY = 4;

    AStar(inicioX, inicioY, objetivoX, objetivoY, 5, 5, mapa);

    return 0;
}
