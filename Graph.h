//
// Created by Kelukin on 2020/3/16.
//

#ifndef MIS_DELETING_K_EDGES_GRAPH_H
#define MIS_DELETING_K_EDGES_GRAPH_H

#include "Utility.h"

struct Edge {
    int id, duplicate;
    int next;
};
struct NodeList{
    NodeList* nxt;
    NodeList* bf;
    int no;
    int degree;
};
class Graph {
private:
    std::string dir;
    ui n, m;
    ui *given_set;
    ui gs_length;
    ui *pstart;
    ui *edges;
    void read_graph();
    void construct_degree_increase(ui *ids);
    void greedy_delete_edges(char *is, ui k);
public:
    Graph(const char *_dir);
    ~Graph();
    void check_is(const char *is, int count, int k = 0);
    void greedy(ui k = 0);
};

#endif //MIS_DELETING_K_EDGES_GRAPH_H
