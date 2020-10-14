#include "Utility.h"
#include "Graph.h"
void print_usage(){
    printf("Usage: [1]exe [2]alg [3]graph-dir [4]deleteNumber \n ");
//    printf("[5]indexType  [6]lastDeleteType [7]indexThreshold\n");
    printf("[5]TrainFlag [6]indexType [7]timeThreshold\n");
    printf("[8]querysetNumber [9]autoLogChooseFlag");
    printf("\talg: greedy, NearLinear, PayAndTry, exact\n");
    printf("\tindexType: 0-GreedyIndex, 1-ProfitPriority, 2-Edge Estimation\n");
//    printf("\tlastDeleteType: 0-basic, 1-smart");
}
extern int INDEX_TYPE;
extern int LAST_DELETE_TYPE;
extern int TIME_THRESHOLD;
extern std::string LOG_PATH;
extern int QUERY_ID;
extern bool Train_Flag; // in the train mode, we assumed the query set to be set
extern bool LOG_FLAG;
extern bool AUTO_LOG;
extern int EDGE_NUMBER_CONSTRAINT;
int main(int argc, char *argv[]) {
    if(argc < 3){
        print_usage();
        return 0;
    }
    if(argc >= 6)   INDEX_TYPE = atoi(argv[5]);
    if(argc >= 7)   TIME_THRESHOLD = atoi(argv[6]);
    if(argc >= 5){
        int tmp = atoi(argv[4]);
        if(tmp == 1){
            Train_Flag = true;
            LOG_FLAG = true;
        }
    }
//    if(argc >= 6)   LAST_DELETE_TYPE = atoi(argv[5]);
    if(INDEX_TYPE == 1)   THRESHOLD = 10;
    if(argc >= 8)   QUERY_ID = atoi(argv[7]);
    if(argc >= 9)   AUTO_LOG = atoi(argv[8]);
    std::vector<int> v;
    LOG_PATH = std::string(argv[2]) + "default.log";



    int k = (argc >= 4)? atoi(argv[3]) : 0; // set the number of edges to be deleted
    EDGE_NUMBER_CONSTRAINT = k;

    if(Train_Flag && QUERY_ID != -1 )
        LOG_PATH = std::string(argv[2]) + "_" + std::to_string(QUERY_ID) + "_" + std::to_string(k) + ".log";
    Graph *graph = new Graph(argv[2]); // read the graph
    graph->read_graph();// read the graph, including of the given set of vertices
    if(strcmp(argv[1], "greedy") == 0)  graph->greedy(k);
    else if(strcmp(argv[1], "n") == 0 || strcmp(argv[1], "NearLinear") == 0) graph->near_maximum_near_linear(k);
    else if(strcmp(argv[1], "p") == 0 ||strcmp(argv[1], "PayAndTry") == 0) graph->pay_and_try_dominate_max_degree_greedy_delete_edges(k);
    else if(strcmp(argv[1], "exact") == 0) graph->exact_solver(k);
    //    std::cout << "Hello, World!" << std::endl;
    return 0;
}