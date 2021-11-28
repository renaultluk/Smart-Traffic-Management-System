#include <iostream>
#include "graphStructs.h"

using namespace std;

const int OCCUPIED_WEIGHT = 10;
Node* path[NUM_NODES];
Node* queue[NUM_NODES];
char direction_queue[NUM_EDGES];

Node node_map[NUM_NODES];
Node* destinations[NUM_DEST] = {&node_map[0], &node_map[18], &node_map[19]};

Edge edge_map[NUM_EDGES] = {
    newEdge(0, &node_map[0], &node_map[1], 1),
    newEdge(1, &node_map[1], &node_map[4], 2),
    newEdge(2, &node_map[4], &node_map[10], 2),
    newEdge(3, &node_map[10], &node_map[15], 4),
    newEdge(4, &node_map[15], &node_map[19], 1),
    newEdge(5, &node_map[15], &node_map[16], 3),
    newEdge(6, &node_map[16], &node_map[17], 9),
    newEdge(7, &node_map[17], &node_map[18], 1),
    newEdge(8, &node_map[17], &node_map[14], 3),
    newEdge(9, &node_map[14], &node_map[9], 3),
    newEdge(10, &node_map[9], &node_map[3], 4),
    newEdge(11, &node_map[3], &node_map[2], 8),
    newEdge(12, &node_map[2], &node_map[1], 4),
    newEdge(13, &node_map[2], &node_map[5], 2),
    newEdge(14, &node_map[5], &node_map[4], 4),
    newEdge(15, &node_map[5], &node_map[7], 1),
    newEdge(16, &node_map[7], &node_map[11], 2),
    newEdge(17, &node_map[11], &node_map[10], 3),
    newEdge(18, &node_map[11], &node_map[12], 3),
    newEdge(19, &node_map[12], &node_map[16], 3),
    newEdge(20, &node_map[12], &node_map[13], 4),
    newEdge(21, &node_map[13], &node_map[14], 4),
    newEdge(22, &node_map[13], &node_map[8], 3),
    newEdge(23, &node_map[8], &node_map[9], 4),
    newEdge(24, &node_map[8], &node_map[6], 3),
    newEdge(25, &node_map[6], &node_map[3], 3),
    newEdge(26, &node_map[6], &node_map[7], 3)
};

void enqueue(Node* queue[], Node* newNode, int &queue_size) {
    queue[queue_size] = newNode;
    queue_size++;
}

void dequeue(Node* queue[], int &queue_size) {
    for (int i = 0; i < queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    queue_size--;
}

char dequeue(char queue[], int &queue_size) {
    char tmp = queue[0];
    for (int i = 0; i < queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    queue_size--;
    return tmp;
}

Node* outgoing(Node* node, Edge edge) {
    return edge.start == node ? edge.end : edge.start;
}

void solve(Node* start) {
    cout << "solve" << endl;

    for (int i = 0; i < NUM_NODES; i++) {   // ** fix: reset all nodes for each starting point
        node_map[i].distance = __INT_MAX__;
        node_map[i].prev = NULL;
        node_map[i].visited = false;
    }
    Node* queue[NUM_NODES] = {};
    int queue_size = 0;
    enqueue(queue, start, queue_size);
    start->visited = true;
    start->distance = 0;

    while (queue_size > 0) {
        Node* current = queue[0];
        cout << "current: " << current->index << endl;
        queue[0]->visited = true;

        for (int i = 0; i < current->degree; i++) { // ** fix: use current->degree
            Edge edge = edge_map[current->edges[i]];
            Node* nextNode = outgoing(current, edge);

            int tmpDistance = current->distance + edge.weight;
            if (tmpDistance < nextNode->distance) {
                cout << "nextNode: " << nextNode->index << endl;
                nextNode->distance = tmpDistance;
                nextNode->prev = current;
            }

            if (!nextNode->visited) {
                enqueue(queue, nextNode, queue_size);
            }
        }
        dequeue(queue, queue_size);
    } 
}

void reconstructPath(Node* target, Node* path[]) {
    cout << "reconstructPath" << endl;
    Node* tmpPath[NUM_NODES];
    tmpPath[0] = target;    // ** fix: add target as first element of tmpPath
    int i = 1;
    Node* current = target; // ** fix: use current iterator
    while (current->prev != nullptr) {
        cout << "prev: " << current->prev->index << endl;
        tmpPath[i] = current->prev;
        current = current->prev;
        i++;
    }
    for (int j = 0; j < i; j++) {   // ** fix: reverse for loop direction
        cout << "[" << j << "]: " << tmpPath[j]->index << endl;
        path[j] = tmpPath[i - j - 1];
    }
    for (int j = 0; j < i; j++) {
        cout << "[" << j << "]: " << path[j]->index << endl;
    }
}

void setDirectionQueue(char direction_queue[], Node* path[], Node* start) {
    cout << "setDirectionQueue" << endl;
    
    for (int i = 0; i < NUM_NODES; i++) {   // ** fix: reset all directions for each starting point
        direction_queue[i] = '\0';
    }
    int path_length = *(&path + 1) - path; // ** fix: use pointer arithmetic for path_length
    cout << "path_length: " << path_length << endl;
    int direction_queue_size = 0;
    for (int i = 0; i < path_length-2; i++) {
        Node* prevNode = path[i];
        Node* currentNode = path[i+1];
        Node* nextNode = path[i+2];
        
        cout << "currentNode: " << currentNode->index << " degree: " << currentNode->degree << endl;
        for (int i = 0; i < currentNode->degree; i++) {
            cout << "edge: " << currentNode->edges[i] << " ";
        }
        cout << endl;
        switch (currentNode->degree)
        {
            case 1:
                cout << "arrived" << endl;
                return;
                break;
            case 2:
                direction_queue[direction_queue_size] = 'l';
                direction_queue_size++;
                break;
            case 3:
                cout << "entered case 3" << endl;
                int currentIndex, nextIndex;
                Edge* prevEdge = connectEdge(prevNode, currentNode);
                Edge* nextEdge = connectEdge(currentNode, nextNode);
                cout << "prevEdge:" << prevEdge->index << " nextEdge: " << nextEdge->index << endl;
                nextEdge->weight += OCCUPIED_WEIGHT;
                cout << "weight added" << endl;
                for (int j = 0; j < 3; j++) {
                    if (prevEdge->index == edge_map[currentNode->edges[j]].index) {
                        currentIndex = j;
                    } else if (nextEdge->index == edge_map[currentNode->edges[j]].index) {
                        nextIndex = j;
                    }

                }
                cout << "edges identified" << endl;
                
                if (currentIndex == (nextIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'l';
                    direction_queue_size++;
                    break;
                } else if (nextIndex == (currentIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'r';
                    direction_queue_size++;
                    break;
                }
                cout << "direction not set" << endl;
        }
    }
}

void planPath(Node* start, Node* target, Node* path[], char direction_queue[]) {
    cout << "entered planPath" << endl;
    solve(start);
    cout << "solved" << endl;
    reconstructPath(target, path);
    cout << "reconstructed" << endl;
    setDirectionQueue(direction_queue, path, start);
    cout << "set direction queue" << endl;
}

void releaseEdge(Edge *edge) {
    edge->weight -= OCCUPIED_WEIGHT;
}

void runCarSim(Node* start, Node* target, Node* path[], char direction_queue[], int index) {
    cout << "Car " << index << ": ";
    planPath(start, target, path, direction_queue);
    cout << "Path of car" << index << ": ";
    int i = 0;
    while (path[i] != target) {
        cout << "[" << path[i]->index << "] -> ";
        i++;
    }
    cout << "[" << path[i]->index << "] -> ";
    cout << "[END]" << endl;
    cout << "Direction queue of car" << index << ": ";
    for (int i = 0; i < sizeof(direction_queue)/sizeof(*direction_queue); i++) {
        cout << direction_queue[i] << " ";
    }
    cout << endl;
}

int main() {
    cout << "program started" << endl;
    initMap();
    cout << "initMap completed" << endl;

    runCarSim(&node_map[0], &node_map[18], path, direction_queue, 1);
    runCarSim(&node_map[19], &node_map[18], path, direction_queue, 2);
    runCarSim(&node_map[18], &node_map[0], path, direction_queue, 3);

    return 0;
}
