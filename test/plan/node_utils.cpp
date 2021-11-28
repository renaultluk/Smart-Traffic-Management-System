#include <iostream>
#include "graphStructs.h"

using namespace std;

Node newBlankNode() {
    Node node;
    node.index = __INT_MAX__;
    node.prev = nullptr;
    node.visited = false;
    node.distance = __INT_MAX__;
    return node;
}

Node newNode(int index) {
    Node node;
    node.index = index;
    node.prev = nullptr;
    node.visited = false;
    node.distance = __INT_MAX__;
    return node;
}

Edge newEdge(int index, Node* start, Node* end, int weight) {
    Edge edge;
    edge.index = index;
    edge.start = start;
    edge.end = end;
    edge.weight = weight;
    return edge;
}

Edge* connectEdge(Node* node1, Node* node2) {   // ** fix: check both orders
    int map_length = sizeof(edge_map)/sizeof(*edge_map);
    for (int i = 0; i < map_length; i++) {
        if ((edge_map[i].start->index == node1->index && edge_map[i].end->index == node2->index) ||
        (edge_map[i].start->index == node2->index && edge_map[i].end->index == node1->index)) {
            return &edge_map[i];
        }
    }
}

void initMap() {
    for (int i = 0; i < NUM_NODES; i++) {
        node_map[i] = newNode(i);
    }
    cout << "node_map init" << endl;

    for (int i = 0; i < NUM_EDGES; i++) {
        edge_map[i].start->edges[edge_map[i].start->degree] = edge_map[i].index;
        edge_map[i].start->degree++;
        edge_map[i].end->edges[edge_map[i].end->degree] = edge_map[i].index;
        edge_map[i].end->degree++;
    }
    cout << "edge_map init" << endl;
}