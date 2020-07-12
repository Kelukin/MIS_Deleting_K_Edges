#include "Utility.h"
#include "Graph.h"
void print_usage(){
    printf("Usage: [1]exe [2]alg [3]graph-dir [4]deleteNumber \n ");
    printf("[5]indexType  [6]lastDeleteType [7]indexThreshold\n");
    printf("\talg: greedy, NearLinear, PayAndTry, exact\n");
    printf("\tindexType: 0-GreedyIndex, 1-ProfitPriority\n");
    printf("\tlastDeleteType: 0-basic, 1-smart");
}
extern int INDEX_TYPE;
extern int LAST_DELETE_TYPE;
extern int THRESHOLD;
int main(int argc, char *argv[]) {
    if(argc < 3){
        print_usage();
        return 0;
    }
    if(argc >= 5)   INDEX_TYPE = atoi(argv[4]);
    if(argc >= 6)   LAST_DELETE_TYPE = atoi(argv[5]);
    if(INDEX_TYPE == 1)   THRESHOLD = 10;
    if(argc >= 7)   THRESHOLD = atoi(argv[6]);
    std::vector<int> v;
    int k = (argc >= 4)? atoi(argv[3]) : 0; // set the number of edges to be deleted
    Graph *graph = new Graph(argv[2]); // read the graph
    graph->read_graph();// read the graph, including of the given set of vertices
    if(strcmp(argv[1], "greedy") == 0)  graph->greedy(k);
    else if(strcmp(argv[1], "n") == 0 || strcmp(argv[1], "NearLinear") == 0) graph->near_maximum_near_linear(k);
    else if(strcmp(argv[1], "p") == 0 ||strcmp(argv[1], "PayAndTry") == 0) graph->pay_and_try_dominate_max_degree_greedy_delete_edges(k);
    else if(strcmp(argv[1], "exact") == 0) graph->exact_solver(k);
    //    std::cout << "Hello, World!" << std::endl;
    return 0;
}