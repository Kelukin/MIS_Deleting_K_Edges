from copy import Error
import networkx as nx
import sys
from networkx import exception
from networkx.algorithms import approximation
import heapq

def readFromFile(file):
    return int.from_bytes(file.read(4), byteorder=sys.byteorder)
def readGraph(filePath):
    G = nx.Graph()
    n = 0
    m = 0
    degree = []
    with open(filePath+'_degree.bin', 'rb') as f:
        x = int.from_bytes(f.read(4), byteorder=sys.byteorder)
        n = int.from_bytes(f.read(4), byteorder=sys.byteorder)
        m = readFromFile(f)
        for i in range(n):
            degree.append(readFromFile(f))
    with open(filePath + '_adj.bin', 'rb'):
        for i in range(n):
            for j in range(degree[i]):
                tmp = readFromFile(filePath)
                G.add_edge(i, tmp)
    return G

def readGivenSet(filePath):
    ret = []
    n = 0
    with open(filePath + '_given_set.bin', 'rb') as f:
        n = readFromFile(f)
        for i in range(n):
            x = readFromFile(f)
            ret.append(x)
    return ret


def get_is(G, gs_set, k):
    cnt = 0
    for x in gs_set:
        for y in gs_set:
            if G.has_edge(x, y) == True:
                cnt += 1
                G.remove_edge(x, y)
    if cnt > k:
        raise Exception('The the number of K is too small!')

    for x in gs_set:
        neighbors = G.neighbors(x)
        for y in neighbors:
            if G.has_node(y):
                G.remove_node(y)
    return approximation.maximum_independent_set(G), k - cnt

def greedyExpansion(G, is, k):
    ret = is[:]
    h = []
    is = set(is)
    degree = [0 for i in range(G.order())]
    for x in ret:
        for y in G.neighbors(x):
            degree[y] += 1
             
    return ret

if __name__ == "__main__":
    argv = sys.argv
    file_path = argv[1]
    k = int(argv[2])
    G = readGraph(file_path)
    gs_set = readGivenSet(file_path)
    tmpG = G.copy()
    try:
        is, k = get_is(G, gs_set, k)
        greedyExpansion(tmpG, is, k)
        print("The result size is %d\n", len(is))
    except Exception as error:
        print(error)