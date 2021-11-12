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