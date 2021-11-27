#include "graphStructs.h"

Node newBlankNode() {
    Node node;
    node.index = __INT_MAX__;
    node.x = __INT_MAX__;
    node.y = __INT_MAX__;
    node.prev = nullptr;
    node.visited = false;
    node.distance = __INT_MAX__;
    return node;
}

Node newNode(int index, int x, int y) {
    Node node;
    node.index = index;
    node.x = x;
    node.y = y;
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

Edge connectEdge(Node node1, Node node2) {
    int map_length = sizeof(edge_map)/sizeof(*edge_map);
    for (int i = 0; i < map_length; i++) {
        if (edge_map[i].start->index == node1.index && edge_map[i].end->index == node2.index) {
            return edge_map[i];
        }
    }
}