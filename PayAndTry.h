//
// Created by Kelukin on 2020/4/7.
//

#ifndef MIS_DELETING_K_EDGES_PAYANDTRY_H
#define MIS_DELETING_K_EDGES_PAYANDTRY_H

#include "Utility.h"
#include "LastDeleter.h"
#include "EdgeDeleteIndex.h"
#include "ProfitDeleteIndex.h"
class PayAndTry {
protected:
    ui n,m;
    ui *given_set;
    ui gs_length;
    ui *pstart;
    ui *edges;
    ui k;
    char *is;
    int *degree;
    int init_given_set();
    void delete_vertex(ui v, char* is, int *degree);
    void get_two_neighbors(ui u, ui &u1, ui &u2);
    virtual bool exist_edge(ui u1, ui u2);
public:
    virtual void init() = 0;
    virtual void inexact_delete() = 0;
    virtual int pay() = 0;
    virtual void last_delete() = 0;
    virtual void reduction() = 0;
    virtual void appending_action() = 0;
    virtual void debugCheck() = 0;
//    virtual void recycleK() = 0;
    virtual bool canReduction() = 0;
    virtual bool isDone() = 0; // it means all vertices' status have been certain.
    PayAndTry(ui _n, ui _m, ui *_given_set, ui _gs_length, ui *_pstart, ui *_edges, ui _k):
            n(_n), m(_m), given_set(_given_set), gs_length(_gs_length), pstart(_pstart),
            edges(_edges), k(_k)
            {
                is = new char[n];
                for(ui i = 0; i < n; ++i)   is[i] = 1;
                degree = new int[n];
                for(ui i = 0; i < n; ++i) degree[i] = pstart[i + 1] - pstart[i];

                k -= init_given_set();
            }
//    PayAndTry(const PayAndTry &obj):n(obj.n), m(obj.m), given_set(obj.given_set) ,
//    gs_length(obj.gs_length), pstart(obj.pstart), edges(obj.edges), k(obj.k){
//    }
    PayAndTry(const PayAndTry &obj) = delete;
    virtual  bool check_result(ui mis_size, ui ori_k) = 0;
    virtual ~PayAndTry(){
        delete[] is;
        delete[] degree;
    }
    int getMISSize();
};

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
    std::vector<ui> dominated;
    std::vector<ui> degree_ones, degree_twos;
    std::vector<std::pair<ui, ui> > S;
    void initial_dominance_and_degree_two_remove();
    int remove_degree_one_two();
    int delete_vertex(ui v, char *is, int *degree
            , std::vector<ui> &degree_ones, std::vector<ui> &degree_twos);
    int delete_vertex_dominate(ui u);
    int compute_triangle_counts();
    void update_triangle(ui u1, ui u2);
    int lp_reduction(const ui *ids, ui ids_n);
    void shrink(ui u, ui &end);
    void update_max_d();
    bool dominated_check(ui u);
    void edge_rewire(ui u, ui u1, ui u2);
    void clear_S();
    ui edge_rewire(ui u, const ui *pend, ui v, ui w);
    bool exist_edge(ui u1, ui u2) override ;
    ui get_other_neighbor(ui u, ui u1);
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
    void reduction() override ;
    bool canReduction() override ;
    bool isDone() override ;
    bool check_result(ui mis_size, ui ori_k);
    void appending_action() override ;
    void deleteEdge(int edgeNo, int u, int v);
    void debugCheck();
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

#endif //MIS_DELETING_K_EDGES_PAYANDTRY_H
