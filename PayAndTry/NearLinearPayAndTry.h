//
// Created by Kelukin on 2020/5/27.
//
#include "PayAndTry.h"

#ifndef MIS_DELETING_K_EDGES_NEARLINEARPAYANDTRY_H
#define MIS_DELETING_K_EDGES_NEARLINEARPAYANDTRY_H

class NearLinearPayAndTry : public PayAndTry{
protected:
    ui ori_k;
    char *adj;
    char *pay_adj;
    char *dominate;
    ui *pend;
    bool delete_tri;
    ui *tri;
    int max_d;
    int *bin_head;
    int *bin_next;
    ui *reverseEdge;
    char *fixed;
    bool *edgeValid;
    EdgeDeleteIndex *edgeDeleteIndex;
    ui *tmp_edges;
    ui *tmp_pstart;
    std::deque<ui> dominated;
    std::vector<ui> degree_ones, degree_twos;
    std::vector<std::pair<ui, ui> > S;
    void initial_dominance_and_degree_two_remove();
    int remove_degree_one_two();
    int delete_vertex(ui v, char *is, int *degree
            , std::vector<ui> &degree_ones, std::vector<ui> &degree_twos);
    virtual int delete_vertex_dominate(ui u);
    virtual int compute_triangle_counts();
    virtual void update_triangle(ui u1, ui u2);
    int lp_reduction(const ui *ids, ui ids_n);
    virtual void shrink(ui u, ui &end);
    virtual void update_max_d();
    bool dominated_check(ui u);
    void edge_rewire(ui u, ui u1, ui u2);
    void clear_S();
    virtual ui edge_rewire(ui u, const ui *pend, ui v, ui w);
    bool exist_edge(ui u1, ui u2) override ;
    ui get_other_neighbor(ui u, ui u1);
    virtual void deleteEdge(int edgeNo, int u, int v);

    virtual void addDominated(ui u, ui v = -1);//u dominate v and u should be reducted

public:
    NearLinearPayAndTry(ui _n, ui _m, ui *_given_set, ui _gs_length
            , ui *_pstart, ui *_edges, ui _k)
            :PayAndTry(_n, _m, _given_set, _gs_length, _pstart, _edges, _k){
        S.clear();
        edgeDeleteIndex = nullptr;
        ori_k = _k;
        adj = new char[n];
        pay_adj = new char[n];
        memset(pay_adj, 0, sizeof(char) * n);
        memset(adj, 0, sizeof(char) * n);
        dominate = new char[n];
        memset(dominate, 0, sizeof(char) * n);
        pend = new ui[n];
        delete_tri = false;

//        edgeValid = nullptr;
        edgeValid = new bool [m];
        for(ui i = 0; i < m; ++i)   edgeValid[i] = true;

        tmp_edges = new ui[m];
        tmp_pstart = new ui[n + 1];

        for(ui i = 0; i < m; ++i)   tmp_edges[i] = edges[i];
        for(ui i = 0; i <= n; ++i)  tmp_pstart[i] = pstart[i];

        max_d = 0;
        tri = nullptr;
        bin_next = nullptr;
        bin_head = nullptr;
        fixed = nullptr;
        reverseEdge = nullptr;
    }
    void init() override;
    void inexact_delete() override ;
    int pay() override ;
    void last_delete() override;
    void localSearch() override;
    void safe_reduction() override ;
    bool canSafeReduction() override ;
    bool isDone() override ;
    void appending_action() override ;
    bool check_result(ui mis_size, ui ori_k);
    void debugCheck() override ;
    bool canRiskReduction() override;
    void riskReduction() override ;
    ~NearLinearPayAndTry() override{
        delete[] adj;
        delete[] dominate;
        delete[] pend;
        delete[] pay_adj;
        if(delete_tri)  delete[] tri;
        if(bin_head)    delete[] bin_head;
        if(bin_next)    delete[] bin_next;
        if(fixed) delete[] fixed;
        delete[] tmp_edges;
        delete[] tmp_pstart;
        if(reverseEdge) delete[] reverseEdge;
        if(edgeValid)   delete[] edgeValid;
        if(edgeDeleteIndex) delete edgeDeleteIndex;
    }
};

#endif //MIS_DELETING_K_EDGES_NEARLINEARPAYANDTRY_H
