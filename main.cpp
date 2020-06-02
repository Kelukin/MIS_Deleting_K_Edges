#include "Utility.h"
#include "Graph.h"
void print_usage(){
    printf("Usage: [1]exe [2]alg [3]graph-dir [4]deleteNumber\n");
    printf("\talg: greedy, NearLinear, PayAndTry, exact\n");
}
int main(int argc, char *argv[]) {
    if(argc < 3){
        print_usage();
        return 0;
    }
    std::vector<int> v;
    int k = (argc == 4)? atoi(argv[3]) : 0; // set the number of edges to be deleted
    Graph *graph = new Graph(argv[2]); // read the graph
    graph->read_graph();// read the graph, including of the given set of vertices
    if(strcmp(argv[1], "greedy") == 0)  graph->greedy(k);
    else if(strcmp(argv[1], "NearLinear") == 0) graph->near_maximum_near_linear(k);
    else if(strcmp(argv[1], "PayAndTry") == 0) graph->pay_and_try_dominate_max_degree_greedy_delete_edges(k);
    else if(strcmp(argv[1], "exact") == 0) graph->exact_solver(k);
    //    std::cout << "Hello, World!" << std::endl;
    return 0;
}