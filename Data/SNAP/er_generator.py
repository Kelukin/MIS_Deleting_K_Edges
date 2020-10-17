from networkx.generators.random_graphs import fast_gnp_random_graph as generator
import sys
def output_ER_graph(outPath, graph):
    degreeFile = open(outPath + "_degree.bin", "wb")
    adjFile = open(outPath + "_adj.bin", "wb")
    # realFile = open(outPath + "_real.txt", "w")
    n = graph.number_of_nodes()
    m = graph.number_of_edges() * 2
    tmp = int(4)
    degreeFile.write(tmp.to_bytes(4, sys.byteorder))
    degreeFile.write(n.to_bytes(4, sys.byteorder))
    degreeFile.write(m.to_bytes(4, sys.byteorder))
    for x in range(n):
        degreeFile.write(graph.degree(x).to_bytes(4, sys.byteorder))
        for i in graph.neighbors(x):
            adjFile.write(i.to_bytes(4, sys.byteorder))
    adjFile.close()
    degreeFile.close()
if __name__ == "__main__":
    argv = sys.argv
    filepath = argv[1]
    n = int(argv[2])
    avg_degree = int(argv[3])
    graph = generator(n, float(avg_degree) / float(n - 1))
    output_ER_graph(filepath, graph)
