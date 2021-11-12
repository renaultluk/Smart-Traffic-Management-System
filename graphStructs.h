const int NUM_NODES = 0;
const int NUM_EDGES = 0;

struct Edge;

struct Node {
    int index;
    double x;
    double y;
    int degree;
    Edge edges[3];
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

Edge connectEdge(Node node1, Node node2);

const Node node_map = {};
const Edge edge_map = {};