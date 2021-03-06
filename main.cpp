#include "Utility.h"
#include "Graph.h"
void print_usage(){
    printf("Usage: [1]exe [2]alg [3]graph-dir [4]deleteNumber \n ");
    printf("[5]TrainFlag [6]ratioSwitch [7]timeThreshold\n");
    printf("[8]CostFunctionOpen");
    printf("\talg: greedy, NearLinear, PayAndRecycle, exact\n");
    printf("\t ratio Switch: 0-absolute value, 1-percentage\n");
//    printf("\tlastDeleteType: 0-basic, 1-smart");
}
extern int ratioSwitch;
extern int LAST_DELETE_TYPE;
extern int TIME_THRESHOLD;
extern bool OPEN_COST_FUNCTION;
extern std::string LOG_PATH;
extern bool Train_Flag; // in the train mode, we assumed the query set to be set
extern bool LOG_FLAG;
extern int EDGE_NUMBER_CONSTRAINT;
int main(int argc, char *argv[]) {
    if(argc < 3){
        print_usage();
        return 0;
    }
    if(argc >= 6)   ratioSwitch = atoi(argv[5]);
    if(argc >= 7)   TIME_THRESHOLD = atoi(argv[6]);
    if(argc >= 5){
        int tmp = atoi(argv[4]);
        if(tmp == 1){
            Train_Flag = true;
            LOG_FLAG = true;
        }
    }
    if(argc >= 8) OPEN_COST_FUNCTION = true;
//    if(argc >= 6)   LAST_DELETE_TYPE = atoi(argv[5]);
    //if(ratioSwitch == 1)   THRESHOLD = 10;
    std::vector<int> v;
    LOG_PATH = std::string(argv[2]) + "default.log";



    int k = (argc >= 4)? atoi(argv[3]) : 0; // set the number of edges to be deleted
    EDGE_NUMBER_CONSTRAINT = k;

//    if(Train_Flag && QUERY_ID != -1 )
//        LOG_PATH = std::string(argv[2]) + "_" + std::to_string(QUERY_ID) + "_" + std::to_string(k) + ".log";
    Graph *graph = new Graph(argv[2]); // read the graph
    graph->read_graph();// read the graph, including of the given set of vertices

    if(ratioSwitch == 1){
        k = double(k) / 1000.0 * graph->get_edge_number() / 2;
    }
    if(strcmp(argv[1], "greedy") == 0)  graph->greedy(k);
    else if(strcmp(argv[1], "n") == 0 || strcmp(argv[1], "NearLinear") == 0) graph->near_maximum_near_linear(k);
    else if(strcmp(argv[1], "p") == 0 || strcmp(argv[1], "PayAndRecycle") == 0) graph->pay_and_try_dominate_max_degree_greedy_delete_edges(k);
    else if(strcmp(argv[1], "exact") == 0) graph->exact_solver(k);
    //    std::cout << "Hello, World!" << std::endl;
    return 0;
}
