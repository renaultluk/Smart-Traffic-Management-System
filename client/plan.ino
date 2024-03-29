#include "../graphStructs.h"

void enqueue(Node queue[], Node newNode, int &queue_size) {
    queue[queue_size] = newNode;
    queue_size++;
}

Node dequeue(Node queue[], int &queue_size) {
    Node tmp = queue[0];
    for (int i = 0; i < queue_size - 1; i++) {
        queue[i] = queue[i + 1];
    }
    queue_size--;
    return tmp;
}

Node* outgoing(Node* node, Edge edge) {
    return edge.start == node ? edge.end : edge.start;
}

void solve(Node &start) {
    Node* queue = new Node[NUM_NODES];
    int queue_size = 0;
    enqueue(queue, start, queue_size);
    start.visited = true;
    start.distance = 0;

    while (queue_size > 0) {
        Node* current = &dequeue(queue, queue_size);
        (*current).visited = true;

        for (int i = 0; i < 3; i++) {
            Edge edge = (*current).edges[i];
            Node* nextNode = outgoing(current, edge);

            int tmpDistance = (*current).distance + edge.weight;
            if (tmpDistance < (*nextNode).distance) {
                (*nextNode).distance = tmpDistance;
                (*nextNode).prev = current;
            }

            if (!(*nextNode).visited) {
                enqueue(queue, (*nextNode), queue_size);
            }
        }
    } 
}

void reconstructPath(Node target, Node path[]) {
    Node tmpPath[NUM_NODES];
    int i = 0;
    while (target.prev != nullptr) {
        tmpPath[i] = *target.prev;
        target = *target.prev;
        i++;
    }
    for (int j = i - 1; j >= 0; j--) {
        path[j] = tmpPath[i - j - 1];
    }
}

void setDirectionQueue(char direction_queue[], Node path[], Node start) {
    int path_length = sizeof(path)/sizeof(*path);
    int direction_queue_size = 0;
    for (int i = 0; i < path_length-2; i++) {
        Node prevNode = path[i];
        Node currentNode = path[i+1];
        Node nextNode = path[i+2];
        
        switch (currentNode.degree)
        {
            case 1:
                return;
                break;
            case 2:
                direction_queue[direction_queue_size] = 'l';
                direction_queue_size++;
                break;
            case 3:
                int currentIndex, nextIndex;
                Edge prevEdge = connectEdge(prevNode, currentNode);
                Edge nextEdge = connectEdge(currentNode, nextNode);
                for (int j = 0; j < 3; j++) {
                    if (prevEdge.index == currentNode.edges[j].index) {
                        currentIndex = j;
                    } else if (nextEdge.index == currentNode.edges[j].index) {
                        nextIndex = j;
                    }

                }
                
                if (currentIndex == (nextIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'l';
                    direction_queue_size++;
                    break;
                } else if (nextIndex == (currentIndex+1)%3) {
                    direction_queue[direction_queue_size] = 'r';
                    direction_queue_size++;
                    break;
                }
        }
    }
}

void planPath(Node start, Node target, Node path[], char direction_queue[]) {
    solve(start);
    reconstructPath(target, path);
    setDirectionQueue(direction_queue, path, start);
}
