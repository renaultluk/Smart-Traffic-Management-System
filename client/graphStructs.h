#ifndef GRAPHSTRUCTS_H
#define GRAPHSTRUCTS_H

#define NUM_NODES = 0;
#define NUM_EDGES = 0;
#define NUM_DEST = 0;

struct Node {
    int index;
    double x;
    double y;
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

Node newNode(int index, double x, double y);

Edge newEdge(int index, Node* start, Node* end, int weight);

Edge connectEdge(Node node1, Node node2);

Node node_map[] = {};
Node* destinations[] = {};
Edge edge_map[] = {};

#endif