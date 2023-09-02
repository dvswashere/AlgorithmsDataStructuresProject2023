/*
 * Algorithms and data structures project for 2022/23 academic year at PoliMi.
 * Shortest path between two stations in a highway.
 *
 * Designed, implemented and tested by: Devis Nishku 10753403
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define MAX_CARS_PER_STATION 512
#define STD_PATH_SIZE 64

typedef enum {RED, BLACK} Color;

typedef struct {
    int32_t km;
    int32_t carsInStation;
    int32_t *cars;
} Station;

typedef struct stationNode {
    Station *station;
    struct stationNode *left;
    struct stationNode *right;
    struct stationNode *parent;
    Color color;
} StationNode;

typedef struct {
    int32_t pathSize;
    int32_t sizeFactor;
    Station *stations;
} Path;

/*
 * The cars are stored as a heap using an array in each station.
 */

int32_t maxAutonomy(Station *station) {
    if (station->carsInStation == 0)
        return 0;
    return station->cars[0];
}

bool insertCar(Station *station, int32_t car) {
    if (station->carsInStation == MAX_CARS_PER_STATION)
        return false;

    station->cars[station->carsInStation] = car;
    station->carsInStation++;
    return true;
}

bool removeCar(Station *station, int32_t car) {
    if (station->carsInStation == 0)
        return false;

    for (int32_t i = 0; i < station->carsInStation; i++) {
        if (station->cars[i] == car) {
            station->cars[i] = station->cars[station->carsInStation - 1];
            station->carsInStation--;
            return true;
        }
    }

    return false;
}

void maxHeapify(Station *station, int32_t n) {
    int32_t left = (n << 1) + 1;
    int32_t right = (n << 1) + 2;
    int32_t largest;

    if (left < station->carsInStation && station->cars[left] > station->cars[n])
        largest = left;
    else
        largest = n;

    if (right < station->carsInStation && station->cars[right] > station->cars[largest])
        largest = right;

    if (largest != n) {
        int32_t tmp = station->cars[n];
        station->cars[n] = station->cars[largest];
        station->cars[largest] = tmp;
        maxHeapify(station, largest);
    }
}

void buildMaxHeap(Station *station) {
    for (int32_t i = station->carsInStation/2; i >= 0; i--)
        maxHeapify(station, i);
}



/*
 * The stations are stored as a red-black tree nodes.
 */

void leftRotate(StationNode **root, StationNode *x) {
    if (x == NULL || x->right == NULL)
        return;

    StationNode *y = x->right;
    x->right = y->left;

    if (y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == NULL)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rightRotate(StationNode **root, StationNode *y) {
    if (y == NULL || y->left == NULL)
        return;

    StationNode *x = y->left;
    y->left = x->right;

    if (x->right != NULL)
        x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == NULL)
        *root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

void insertFixup(StationNode **root, StationNode *z) {
    if (z == NULL)
        return;

    while (z != *root && z->parent != NULL && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            StationNode *y = z->parent->parent->right;

            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(root, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(root, z->parent->parent);
            }
        }
        else {
            StationNode *y = z->parent->parent->left;

            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(root, z);
                }

                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(root, z->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
}

void insertStation(StationNode **root, Station *station) {
    StationNode *z = (StationNode *) malloc(sizeof(StationNode));
    z->station = station;
    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;
    z->color = RED;

    StationNode *y = NULL;
    StationNode *x = *root;

    while (x != NULL) {
        y = x;

        if (z->station->km < x->station->km)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if (y == NULL)
        *root = z;
    else if (z->station->km < y->station->km)
        y->left = z;
    else
        y->right = z;

    insertFixup(root, z);
}

void transplant(StationNode **root, StationNode *u, StationNode *v) {
    if (u->parent == NULL)
        *root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    if (v != NULL)
        v->parent = u->parent;
}

StationNode* minimumNode(StationNode *node) {
    while (node != NULL && node->left != NULL)
        node = node->left;
    return node;
}

void removeFixup(StationNode **root, StationNode *x) {
    if (x == NULL)
        return;

    while (x != *root && x->parent != NULL && x->color == BLACK) {
        if (x == x->parent->left) {
            StationNode *w = x->parent->right;
            if (w == NULL)
                return;

            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(root, x->parent);
                w = x->parent->right;
            }

            if (w == NULL)
                return;

            if ((w->left == NULL || w->left->color == BLACK) && (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL)
                        w->left->color = BLACK;

                    w->color = RED;
                    rightRotate(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL)
                    w->right->color = BLACK;
                leftRotate(root, x->parent);
                x = *root;
            }
        } else {
            StationNode *w = x->parent->left;
            if (w == NULL)
                return;

            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }

            if (w == NULL)
                return;

            if ((w->right == NULL || w->right->color == BLACK) && (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL)
                        w->right->color = BLACK;

                    w->color = RED;
                    leftRotate(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;

                if (w->left != NULL)
                    w->left->color = BLACK;

                rightRotate(root, x->parent);
                x = *root;
            }
        }
    }

    if (x != NULL)
        x->color = BLACK;
}

void removeStation(StationNode **root, StationNode *z) {
    if (z == NULL)
        return;

    StationNode *y = z;
    StationNode *x;
    Color yOriginalColor = y->color;

    if (z->left == NULL) {
        x = z->right;
        transplant(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        transplant(root, z, z->left);
    } else {
        y = minimumNode(z->right);
        yOriginalColor = y->color;
        x = y->right;

        if (y->parent == z && x != NULL)
            x->parent = y;
        else {
            transplant(root, y, y->right);
            y->right = z->right;

            if (y->right != NULL)
                y->right->parent = y;
        }

        transplant(root, z, y);
        y->left = z->left;

        if (y->left != NULL)
            y->left->parent = y;
        y->color = z->color;
    }

    if (yOriginalColor == BLACK)
        removeFixup(root, x);

    if (z->station != NULL)
        free(z->station->cars);
    free(z->station);
    free(z);
}

StationNode* searchStation(StationNode *root, int32_t km) {
    if (root == NULL || km == root->station->km)
        return root;

    if (km < root->station->km)
        return searchStation(root->left, km);

    return searchStation(root->right, km);
}

StationNode* nextStation(StationNode *root) {
    if (root->right != NULL) {
        root = root->right;

        while (root->left != NULL)
            root = root->left;

        return root;
    }

    StationNode *y = root->parent;

    while (y != NULL && root == y->right) {
        root = y;
        y = y->parent;
    }
    return y;
}

StationNode* prevStation(StationNode *root) {
    if (root->left != NULL) {
        root = root->left;

        while (root->right != NULL)
            root = root->right;

        return root;
    }

    StationNode *y = root->parent;

    while (y != NULL && root == y->left) {
        root = y;
        y = y->parent;
    }
    return y;
}

void deallocateTree(StationNode *root) {
    if (root == NULL)
        return;

    deallocateTree(root->left);
    deallocateTree(root->right);

    if (root->station != NULL)
        free(root->station->cars);
    free(root->station);
    free(root);
}



/*
 * The shortest path is calculated using a greedy algorithm on an array of stations, from the START station to the END.
 */
    // the following function is used to build the full path of stations from the START to the END
Path *buildFullPath(StationNode *startNode, StationNode *endNode) {
    Path *path = malloc(sizeof(Path));
    path->pathSize = 0;
    path->sizeFactor = 1;
    path->stations = malloc(STD_PATH_SIZE * sizeof(Station));

    StationNode *current = startNode;

    while (current->station->km != endNode->station->km) {
        if (path->pathSize == path->sizeFactor * STD_PATH_SIZE) {
            path->sizeFactor += 1;
            path->stations = realloc(path->stations, path->sizeFactor * STD_PATH_SIZE * sizeof(Station));
        }

        path->stations[path->pathSize] = *current->station;
        path->pathSize++;

        if (startNode->station->km < endNode->station->km)
            current = nextStation(current);
        else
            current = prevStation(current);
    }

    if (path->pathSize == path->sizeFactor * STD_PATH_SIZE) {
        path->sizeFactor += 1;
        path->stations = realloc(path->stations, path->sizeFactor * STD_PATH_SIZE * sizeof(Station));
    }

    path->stations[path->pathSize] = *current->station;
    path->pathSize++;

    return path;
}

int32_t abs(int32_t n) {
    if (n < 0)
        return 0 - n;
    return n;
}

// the following function is used to calculate the shortest path when START.km < END.km
void shortestPathSmallToBig(Path *path) {
    int32_t current = 0;
    int32_t start = 0;
    int32_t end = path->pathSize - 1;

    while (current < end) {
        if (path->stations[current].km + maxAutonomy(&path->stations[current]) >= abs(path->stations[end].km)) {
            path->stations[current].km = path->stations[current].km * -1; // mark as used station
            end = current;
            current = start;
        } else if (current + 1 == end) {
            printf("nessun percorso\n");
            free(path->stations);
            free(path);
            return;
        } else {
            current++;
        }
    }

    for (int i = 0; i < path->pathSize - 1; i++) {
        if (path->stations[i].km <= 0) {
            path->stations[i].km = path->stations[i].km * -1; // restore the original value
            printf("%d ", path->stations[i].km);
        }
    }
    printf("%d\n", path->stations[path->pathSize-1].km);
    free(path->stations);
    free(path);
}

// the following function is used to calculate the shortest path when START.km > END.km
void shortestPathBigToSmall(Path *path) {
    int32_t distances[path->pathSize];
    int32_t previous[path->pathSize];
    int32_t newDist;

    distances[0] = 0; // first node is starting node, so 0 stations must be crossed to reach it
    previous[0] = -1;

    for (int32_t i = 1; i < path->pathSize; i++) { // setting the distances to all other nodes at infinite
        distances[i] = INT32_MAX;
        previous[i] = -1;
    }

    for (int32_t current = 0; current < path->pathSize; current++) {
        for (int32_t i = current + 1; i < path->pathSize && (path->stations[current].km - maxAutonomy(&path->stations[current]) <= path->stations[i].km); i++) {
            newDist = distances[current] + 1; // all neighbors are at distance 1

            if (newDist <= distances[i]) {
                distances[i] = newDist;
                previous[i] = current;
            }
        }
    }

    int32_t index = path->pathSize - 1;
    while (index >= 0) {
        if (index > 0 && previous[index] < 0) {
            printf("nessun percorso\n");
            free(path->stations);
            free(path);
            return;
        }

        path->stations[index].km *= -1; // mark as used
        index = previous[index];
    }

    for (int i = 0; i < path->pathSize - 1; i++) {
        if (path->stations[i].km <= 0) {
            path->stations[i].km = path->stations[i].km * -1; // restore the original value
            printf("%d ", path->stations[i].km);
        }
    }

    path->stations[path->pathSize-1].km *= -1;
    printf("%d\n", path->stations[path->pathSize-1].km);
    free(path->stations);
    free(path);
}



int main() {
    char currentCommand[20];
    int32_t distance, start, end, numCars, car;
    StationNode *root = NULL;
    Station *station = NULL;

    while (fscanf(stdin, "%s", currentCommand) != EOF) {

        // aggiungi-stazione
        if (currentCommand[0] == 'a' && currentCommand[9] == 's') {
            assert(fscanf(stdin, "%d", &distance));

            if (root == NULL) {
                station = malloc(sizeof(Station));
                station->km = distance;
                station->carsInStation = 0;
                station->cars = malloc(MAX_CARS_PER_STATION * sizeof(int32_t));

                assert(fscanf(stdin, "%d", &numCars));

                for(int i=0; i<numCars; i++) {
                    assert(fscanf(stdin, "%d", &car));
                    insertCar(station, car);
                }
                buildMaxHeap(station);

                root = malloc(sizeof(StationNode));
                root->station = station;
                root->parent = NULL;
                root->left = NULL;
                root->right = NULL;
                root->color = BLACK;
                printf("aggiunta\n");
            }
            else {
                if (searchStation(root, distance) == NULL) {
                    station = malloc(sizeof(Station));
                    station->km = distance;
                    station->carsInStation = 0;
                    station->cars = malloc(sizeof(int32_t) * MAX_CARS_PER_STATION); // nel test 39 da malloc() corrupted top size

                    assert(fscanf(stdin, "%d", &numCars));

                    for(int i=0; i<numCars; i++) {
                        assert(fscanf(stdin, "%d", &car));
                        insertCar(station, car);
                    }
                    buildMaxHeap(station);                    

                    insertStation(&root, station);
                    printf("aggiunta\n");
                }
                else {
                    printf("non aggiunta\n");
                }
            }
        }

        // demolisci-stazione
        else if (currentCommand[0] == 'd') {
            assert(fscanf(stdin, "%d", &distance));
            StationNode *searchResult = searchStation(root, distance);

            if (searchResult == NULL) 
                printf("non demolita\n");
            else {
                removeStation(&root, searchResult);
                printf("demolita\n");
            }
        }

        // aggiungi-auto
        else if (currentCommand[0] == 'a' && currentCommand[9] == 'a') {
            assert(fscanf(stdin, "%d", &distance));
            assert(fscanf(stdin, "%d", &car));
            StationNode *searchResult = searchStation(root, distance);

            if (searchResult == NULL)
                printf("non aggiunta\n");
            else {
                if (insertCar(searchResult->station, car)) {
                    buildMaxHeap(searchResult->station);
                    printf("aggiunta\n");
                }
                else
                    printf("non aggiunta\n");
            }
        }

        // rottama-auto
        else if (currentCommand[0] == 'r') {
            assert(fscanf(stdin, "%d", &distance));
            assert(fscanf(stdin, "%d", &car));
            StationNode *searchResult = searchStation(root, distance);

            if (searchResult == NULL)
                printf("non rottamata\n");
            else {
                if (removeCar(searchResult->station, car)) {
                    buildMaxHeap(searchResult->station);
                    printf("rottamata\n");
                }
                else 
                    printf("non rottamata\n");
            }
        }

        // pianifica-percorso
        else if (currentCommand[0] == 'p') {
            assert(fscanf(stdin, "%d", &start));
            assert(fscanf(stdin, "%d", &end));

            StationNode *startNode = searchStation(root, start);
            StationNode *endNode = searchStation(root, end);

            if (startNode == NULL || endNode == NULL)
                printf("nessun percorso\n");
            else if (start == end)
                printf("%d\n", start);
            else if (start < end) {
                shortestPathSmallToBig(buildFullPath(startNode, endNode));
            } else {
                shortestPathBigToSmall(buildFullPath(startNode, endNode));
            }
        }
    }

    deallocateTree(root);
}