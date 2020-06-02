//
// Created by Kelukin on 2020/6/1.
//

#ifndef MIS_DELETING_K_EDGES_EXACTSOLVER_H
#define MIS_DELETING_K_EDGES_EXACTSOLVER_H

#include "Utility.h"


class ExactSolver {
private:
    ui n,m;
    ui *given_set;
    ui gs_length;
    ui *pstart;
    ui *edges;
    ui max_d;
    ui *degree;
    char *is; // whether it is in the discussion scope
    std::vector<bool> inAnswer;
    NodeList *pool;
    NodeList** degreeHead;
    int extractTree(ui residual_n, int k, int sz);
    int *weight;
    void init(int &k); // initialize the status of owning given sets
    void rmVertex(ui u); // remove the vertex from the degree pool
    void putVertex(ui u); // put the vertex into the degree pool
    void mvDegreeList(ui u, ui old_d, ui new_d);
    int get_max_degree_vertex();
public:
    ExactSolver(ui _n, ui _m, ui* _gs, ui _gs_length, ui* _pstart, ui* _edges):
    n(_n), m(_m), given_set(_gs), gs_length(_gs_length), pstart(_pstart), edges(_edges){
        is = new char[_n];
        inAnswer = std::vector<bool> (_n, false);
        weight = new int[_n];
        degree = new ui[_n];
        memset(weight, 0, sizeof(int) * _n);
        memset(is, 1, sizeof(char) * _n);
        max_d = 0;
        NodeList *pool = new NodeList[n];
        for(ui i = 0; i < _n; ++i){
            pool[i].no = i;
            degree[i] = pstart[i + 1]  - pstart[i];
            max_d = (max_d > degree[i]) ? max_d : degree[i];
        }

        degreeHead = new NodeList*[++max_d];
        for(ui i = 0; i < max_d; ++i)   degreeHead[i] = NULL;

        for(ui i = 0; i < max_d; ++i){
            ui d = pstart[i + 1] - pstart[i];
            if(degreeHead[d] != NULL){
                pool[i].nxt = degreeHead[d];
                degreeHead[d]->bf = &pool[i];
            }
            else pool[i].nxt = NULL;
            pool[i].bf = NULL;
            degreeHead[d] = &pool[i];
        }

    }
    ExactSolver(ExactSolver &another) = delete;
    int solve(int k);
    ~ExactSolver(){
        delete[] is;
        delete[] pool;
        delete[] degreeHead;
        delete[] weight;
        delete[] degree;
    }
};


#endif //MIS_DELETING_K_EDGES_EXACTSOLVER_H
