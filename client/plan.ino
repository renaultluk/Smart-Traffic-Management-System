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

Node outgoing(Node node, Edge edge) {
    return edge.start == &node ? *edge.end : *edge.start;
}

void solve(Node start) {
    Node* queue = new Node[NUM_NODES];
    int queue_size = 0;
    enqueue(queue, start, queue_size);
    start.visited = true;
    start.distance = 0;

    while (queue_size > 0) {
        Node current = dequeue(queue, queue_size);

        for (int i = 0; i < 3; i++) {
            Edge edge = current.edges[i];
            Node nextNode = outgoing(current, edge);

            int tmpDistance = current.distance + edge.weight;
            if (tmpDistance < nextNode.distance) {
                nextNode.distance = tmpDistance;
                nextNode.prev = &current;
            }

            if (!nextNode.visited) {
                enqueue(queue, nextNode, queue_size);
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

void planPath(Node start, Node target, Node path[]) {
    solve(start);
    reconstructPath(target, path);
}
