//
// Created by Kelukin on 2020/4/7.
//

#ifndef MIS_DELETING_K_EDGES_PAYANDTRY_H
#define MIS_DELETING_K_EDGES_PAYANDTRY_H

#include "../Utility.h"
#include "../LastDeleter/LastDeleter.h"
#include "../EdgeIndex/EdgeDeleteIndex.h"
#include "../EdgeIndex/ProfitDeleteIndex.h"
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
    virtual void safe_reduction() = 0;
    virtual void appending_action() = 0;
    virtual void debugCheck() = 0;
//    virtual void recycleK() = 0;
    virtual bool canSafeReduction() = 0;
    virtual bool canRiskReduction();
    virtual void riskReduction();
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


#endif //MIS_DELETING_K_EDGES_PAYANDTRY_H
