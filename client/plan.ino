#include "graphStructs.h"
#include "client.h"
#include <ArduinoJson.h>

const int OCCUPIED_WEIGHT = 10;

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
    Node* queue[NUM_NODES] = {};
    int queue_size = 0;
    enqueue(queue, start, queue_size);
    start->visited = true;
    start->distance = 0;

    while (queue_size > 0) {
        Node* current = queue[0];
        queue[0]->visited = true;

        for (int i = 0; i < current->degree; i++) {
            Edge edge = edge_map[current->edges[i]];
            Node* nextNode = outgoing(current, edge);

            int tmpDistance = current->distance + edge.weight;
            if (tmpDistance < nextNode->distance) {
                nextNode->distance = tmpDistance;
                nextNode->prev = current;
            }

            if (!nextNode->visited) {
                enqueue(queue, nextNode, queue_size);
                nextNode = nullptr;
            }
        }
        dequeue(queue, queue_size);
        current = nullptr;
    }
}

int reconstructPath(Node* target, Node* path[]) {
    Node* tmpPath[NUM_NODES];
    tmpPath[0] = target;
    int i = 1;
    Node* current = target;
    while (current->prev != nullptr) {
        addToJSON(weightsJson, connectEdge(current, current->prev)->index, OCCUPIED_WEIGHT);      // JSON pending
        tmpPath[i] = current->prev;
        current = current->prev;
        i++;
    }
    for (int j = 0; j < i; j++) {
        path[j] = tmpPath[i - j - 1];
        Serial.print(path[j]->index);
        Serial.print("->");
    }
    return i;
}

void setDirectionQueue(char direction_queue[], Node* path[], Node* start, int path_length) {
    Serial.print("path_length: ");
    Serial.println(path_length);
    int direction_queue_size = 0;
    for (int i = 0; i < path_length-2; i++) {
        Serial.print("Entering node ");
        Serial.println(i);
        Node* prevNode = path[i];
        Node* currentNode = path[i+1];
        Node* nextNode = path[i+2];
        
        switch (currentNode->degree)
        {
            case 1:
                return;
                break;
            case 2:
                direction_queue[direction_queue_size] = 'l';
                direction_queue_size++;
                break;
            case 3:
                Serial.println("Entered switch");
                int currentIndex, nextIndex;
                Edge* prevEdge = connectEdge(prevNode, currentNode);
                Edge* nextEdge = connectEdge(currentNode, nextNode);
                for (int j = 0; j < 3; j++) {
                    if (prevEdge->index == edge_map[currentNode->edges[j]].index) {
                        currentIndex = j;
                    } else if (nextEdge->index == edge_map[currentNode->edges[j]].index) {
                        nextIndex = j;
                    }

                }
                
                if (currentIndex == (nextIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'l';
                    Serial.println("l");
                    direction_queue_size++;
                    break;
                } else if (nextIndex == (currentIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'r';
                    Serial.println("r");
                    direction_queue_size++;
                    break;
                }
        }
        Serial.print("Direction set for node ");
        Serial.println(i);
    }
    for (int i = 0; i < direction_queue_size; i++) {
      Serial.print(direction_queue[i]);
      Serial.print("->");
    }
    Serial.println();
}

void planPath(Node* start, Node* target, Node* path[], char direction_queue[]) {
    solve(start);
    int path_length = reconstructPath(target, path);
    setDirectionQueue(direction_queue, path, start, path_length);
    Serial.println("planPath finished");
}

void releaseEdge(Edge *edge) {
    addToJSON(weightsJson, edge->index, -1*OCCUPIED_WEIGHT);
}

//void replanPath(Node upcomingNode, Node target, Node path[], char direction_queue[], int path_length) {
//    planPath(upcomingNode, target, path, direction_queue);
//    for (int i = 0; i < path_length-1; i++) {
//        releaseEdge(&connectEdge(path[i], path[(i+1)]));
//        direction_queue[i] = ' ';
//    }
//}
