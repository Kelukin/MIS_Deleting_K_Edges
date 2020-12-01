from copy import Error
from heapq import heappop, heappush
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
    with open(filePath + '_adj.bin', 'rb') as f:
        for i in range(n):
            for j in range(degree[i]):
                tmp = readFromFile(f)
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
        neighbors = list(G.neighbors(x))
        for y in neighbors:
            if G.has_node(y):
                G.remove_node(y)
    return approximation.maximum_independent_set(G), k - cnt

def greedyExpansion(G, ss, k):
    h = []
    ss = set(ss)
    degree = [0 for i in range(G.order())]
    for x in ss:
        for y in G.neighbors(x):
            degree[y] += 1
    
    for i in range(G.order()):
        if i not in ss:
            heappush(h, (degree[i], i))

    while h != []:
        tmp = heappop(h)
        if tmp[0] > k:
            break
        elif degree[tmp[1]] != tmp[0]:
            continue
        
        x = tmp[1]
        if x in ss:
            continue
        
        ss.add(x)
        k -= tmp[0]
        for y in G.neighbors(x):
            if y in ss:
                continue
            degree[y] += 1
            heappush(h, (degree[y], y))
        
    return list(ss)

if __name__ == "__main__":
    argv = sys.argv
    file_path = argv[1]
    k = int(argv[2])
    G = readGraph(file_path)
    gs_set = readGivenSet(file_path)
    tmpG = G.copy()
    try:
        s, k = get_is(G, gs_set, k)
        s = greedyExpansion(tmpG, s, k)
        print("The result size is %d\n", len(s))
    except Exception as error:
        print(error)