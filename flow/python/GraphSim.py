#
# @file GraphSim.py
# @author Mingjie Liu
# @date 10/02/2019
# @brief The class for calculating graph similarity score based on graph spectral analysis
#
import networkx as nx
import matplotlib.pyplot as plt
import scipy.stats as stat
import sys

class GraphSim(object):
    def __init__(self, graph=nx.Graph()):
        self.graph = graph
        self.alpha_min = 2
        self.alpha_max = 3
        self.min_size = 5

    def defSub(self, subgraphA, subgraphB):
        self.subA = subgraphA
        self.subB = subgraphB

    def dist(self, centerType):
        if centerType == 'jordan':
            self.cenA, self.radA = self.jordanCenter(self.subA)
            self.cenB, self.radB = self.jordanCenter(self.subB)
        elif centerType == 'eigen':
            self.cenA, self.radA = self.eigenCenter(self.subA)
            self.cenB, self.radB = self.eigenCenter(self.subB)
        else:
            self.cenA, self.radA = self.pagerankCenter(self.subA)
            self.cenB, self.radB = self.pagerankCenter(self.subB)
        dist = nx.shortest_path_length(self.graph, source=self.cenA, target=self.cenB) * 0.5
        # Choosing distance based center distance and subgraph radius
        dist = max(dist, self.alpha_min*self.radA, self.alpha_min*self.radB, self.min_size) # At least alpha_min
        dist = min(dist, self.alpha_max*self.radA, self.alpha_max*self.radB) # At most alpha_max
        # Ensure feasibility
        dist = min(dist, nx.eccentricity(self.graph, v=self.cenA), nx.eccentricity(self.graph, v=self.cenB))
        return dist

    def extractSub(self, centerType):
        dist = self.dist(centerType)
        self.extA = self.BFSSub(self.cenA, dist)
        self.extB = self.BFSSub(self.cenB, dist)

    def specSimScore(self, subgraphA, subgraphB, centerType=None):
        self.defSub(subgraphA, subgraphB)
        self.extractSub(centerType)
        specA = nx.laplacian_spectrum(self.extA, weight=None)
        specB = nx.laplacian_spectrum(self.extB, weight=None)
        D, p_val = stat.ks_2samp(specA, specB)
        return p_val

    def BFSSub(self, cen, dist):
        assert dist <= nx.eccentricity(self.graph, v=cen), "ERROR! Distance indicated is too large!"
        visited = set()
        current = {cen}
        for i in range(dist):
            toVisit = set()
            visited = visited.union(current)
            for v in current:
                for u in self.graph[v]:
                    if u not in visited:
                        toVisit.add(u)
            current = toVisit
        return self.graph.subgraph(visited)

    @staticmethod
    def jordanCenter(graph):
        if len(graph.nodes) == 1:
            return graph.nodes[0], 0
        if not nx.is_connected(graph):
            conComp = max(nx.connected_components(graph), key=len)
            graph = graph.subraph(conComp)
        center = nx.center(graph)[0]
        radius = nx.radius(graph)
        return center, radius

    @staticmethod
    def eigenCenter(graph):
        if len(graph.nodes) == 1:
            return graph.nodes[0], 0
        simMatrix = nx.eigenvector_centrality_numpy(graph)
        maxSim = 0
        for node in graph.nodes:
            if maxSim < simMatrix[node]:
                center = node
                maxSim = simMatrix[node]
        radius = nx.eccentricity(graph, v=center)
        return center, radius

    @staticmethod
    def pagerankCenter(graph):
        if len(graph.nodes) == 1:
            return graph.nodes[0], 0
        simMatrix = nx.pagerank(graph)
        sumDist = dict()
        minDist = float("inf")
        centerList = list()
        center = None
        for node in graph.nodes:
            if hasattr(simMatrix[node], 'values'):
                sumDist[node] = sum(list(simMatrix[node].values()))
            else:
                sumDist[node] = simMatrix[node]
            if minDist > sumDist[node]:
                centerList = [node]
                minDist = sumDist[node]
            elif minDist == sumDist[node]:
                centerList.append(node)
        center = centerList[0]
        radius = nx.eccentricity(graph, v=center)
        return center, radius

class testGraph(object):
    def __init__(self):
        self.g = nx.Graph()

    @staticmethod
    def plot(graph):
        labels = dict()
        for x in graph.nodes():
            labels[x] = x
        nx.draw(graph, labels=labels)
        plt.show()

    def fulCon(self, nodes):
        x = len(nodes)
        for i in range(x):
            for j in range(i+1,x):
                self.g.add_edge(nodes[i], nodes[j])

    def graph1(self):
        self.g.add_nodes_from(range(10))
        self.fulCon([0,1,2,3])
        self.fulCon([0,4,5,6])
        self.fulCon([0,7,8,9])

    def graph2(self):
        self.g.add_nodes_from(range(13))
        self.fulCon([0,1,2,3,4])
        self.fulCon([0,5,6,7,8])
        self.g.add_edge(2,9)
        self.g.add_edge(5,10)
        self.g.add_edge(11,10)
        self.g.add_edge(12,10)

    def graph3(self):
        self.g.add_nodes_from(range(27))
        self.fulCon(range(10))
        self.fulCon(range(10, 20))
        self.fulCon([20,21,22])
        self.fulCon([23,24,25])
        self.g.add_edge(0,22)
        self.g.add_edge(20,26)
        self.g.add_edge(23,26)
        self.g.add_edge(25,10)

def main(argv):
    g = testGraph()
    g.graph3()
    graphSim = GraphSim(g.g)
    """
    print GraphSim.jordanCenter(g.g)
    print GraphSim.eigenCenter(g.g)
    print GraphSim.pagerankCenter(g.g)
    print "Original Graph"
    """
    g.plot(graphSim.graph)
    #print "Jordan Center Rad 3"
    g.plot(graphSim.BFSSub(graphSim.jordanCenter(graphSim.graph)[0], 3))
    #print "Eigen Center Rad 2"
    g.plot(graphSim.BFSSub(graphSim.eigenCenter(graphSim.graph)[0], 2))

if __name__ == '__main__':
    main(sys.argv[1:])
