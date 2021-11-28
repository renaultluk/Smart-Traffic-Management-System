#ifndef GRAPHSTRUCTS_H
#define GRAPHSTRUCTS_H

#define NUM_NODES   20
#define NUM_EDGES   27
#define NUM_DEST    3

struct Node {
    int index;
    int degree;
    int edges[3];
    int distance = __INT_MAX__;
    Node* prev;
    bool visited = false;
};

struct Edge {
    int index;
    Node* start;
    Node* end;
    double weight;
};

Node newBlankNode();

Node newNode(int index);

Edge newEdge(int index, Node* start, Node* end, int weight);

Edge connectEdge(Node node1, Node node2);

void initMap();

Node node_map[NUM_NODES] = {};
Node* destinations[NUM_DEST] = {};
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
    newEdge(25, &node_map[6], &node_map[7], 3)
};

#endif