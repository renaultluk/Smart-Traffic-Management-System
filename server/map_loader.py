import csv

class Node:
    def __init__(self, index, x, y):
        self.index = index
        self.x = x
        self.y = y
        self.degree = 0
        self.edges = []

class Edge:
    def __init__(self, index, start, end, weight):
        self.index = index
        self.start = start
        self.end = end
        self.weight = weight
    
    def changeWeight(self, delta):
        self.weight += delta

def loadMap(node_map, edge_map, destinations):
    with open("node_map.csv", newline='') as nodeMap:
        node_index = 0
        nodeReader = csv.reader(nodeMap, delimiter=',')

        for row in nodeReader:
            curr_node = Node(node_index, int(row[0]), int(row[1]))
            nodeMap.append(curr_node)
            if (row[2] == '1'):
                destinations.append(curr_node)
            
            node_index += 1

    with open("edge_map.csv", newline='') as edgeMap:
        edge_index = 0
        with open("edge_map.csv", newline='') as edgeMap:
            edgeReader = csv.reader(edgeMap, delimiter=',')

            for row in edgeReader:
                edge_map.append(Edge(edge_index, int(row[0]), int(row[1]), int(row[2])))
                node1 = node_map[int(row[0])]
                node2 = node_map[int(row[1])]
                node1.degree += 1
                node2.degree += 1
                node1.edges.append(edge_index)
                node2.edges.append(edge_index)
