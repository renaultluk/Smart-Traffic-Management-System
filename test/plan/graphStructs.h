#ifndef GRAPHSTRUCTS_H
#define GRAPHSTRUCTS_H

#define NUM_NODES   20  // ** fix: use NUM_NODES
#define NUM_EDGES   27  // ** fix: use NUM_EDGES
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

Edge* connectEdge(Node* node1, Node* node2);

void initMap();

extern Node node_map[NUM_NODES];
extern Node* destinations[NUM_DEST];
extern Edge edge_map[NUM_EDGES];

#endif