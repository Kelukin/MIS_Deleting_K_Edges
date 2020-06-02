//
// Created by Kelukin on 2020/3/16.
//

#ifndef MIS_DELETING_K_EDGES_GRAPH_H
#define MIS_DELETING_K_EDGES_GRAPH_H

#include "Utility.h"
#include "PayAndTry/PayAndTry.h"
#include "PayAndTry/NearLinearPayAndTry.h"
#include "PayAndTry/RiskPayAndTry.h"
#include "ExactSolver.h"
struct Edge {
    int id, duplicate;
    int next;
};
class Graph {
private:
    std::string dir;
    ui n, m;
    ui *given_set;
    ui gs_length;
    ui *pstart;
    ui *edges;
    void construct_degree_increase(ui *ids);
    int greedy_delete_edges(char *is, ui k);
    int initial_dominance_and_degree_two_remove(std::vector<ui> &degree_ones, std::vector<ui> &degree_twos, char *is, int *degree, char *adj, std::vector< std::pair<ui,ui> > &S) ;
    int delete_vertex(ui v, char *is, int *degree, std::vector<ui> &degree_ones, std::vector<ui> &degree_twos);
    int delete_vertex(ui u, ui *pend, char *is, std::vector<ui> &degree_twos, ui *tri, char *adj, int *degree, char *dominate, std::vector<ui> &dominated);
    void delete_vertex(ui v, char *is, int *degree);

    int remove_degree_one_two(std::vector<ui> &degree_ones, std::vector<ui> &degree_twos, char *is, int *degree, std::vector<std::pair<ui,ui> > &S);
    void get_two_neighbors(ui u, char *is, ui &u1, ui &u2);
    int lp_reduction(ui *ids, ui ids_n, char *is, int *degree) ;
    ui get_other_neighbor(ui u, char *is, ui u1);
    int exist_edge(ui u, ui v, const ui *pend);
    int exist_edge(ui u1, ui u2);
    ui edge_rewire(ui u, const ui *pend, ui v, ui w);
    void edge_rewire(ui u, ui u1, ui u2);
    int compute_triangle_counts(ui *tri, ui *pend, char *adj, char *is, int *degree, char *dominate, std::vector<ui> &dominated) ;
    void shrink(ui u, ui &end, const char *is, ui *tri) ;
    int dominated_check(ui u, ui *pend, char *is, ui *tri, int *degree) ;
    void update_triangle(ui u1, ui u2, ui *pend, char *is, char *adj, ui *tri, int *degree, char *dominate, std::vector<ui> &dominated) ;
    int pay_and_try_framework(PayAndTry* payAndTry);
    int init_given_set(char *is, int* degree);
    void recover_given_set(char *is);
public:
    Graph(const char *_dir);
    ~Graph();
    void check_is(const char *is, int count, int k = 0);
    void greedy(ui k = 0);
    void read_graph();
    void pay_and_try_dominate_max_degree_greedy_delete_edges(ui k = 0);
    void near_maximum_near_linear(ui k = 0);
    void exact_solver(ui k = 0);
};

#endif //MIS_DELETING_K_EDGES_GRAPH_H
