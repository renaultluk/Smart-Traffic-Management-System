const int NUM_NODES = 0;
const int NUM_EDGES = 0;

struct Edge;

struct Node {
    int index;
    double x;
    double y;
    Edge edges[3];
    int distance = __INT_MAX__;
    Node* prev;
    bool visited = false;
};

struct Edge {
    Node* start;
    Node* end;
    double weight;
};

const Node node_map = {};