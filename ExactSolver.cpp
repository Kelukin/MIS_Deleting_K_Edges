//
// Created by Kelukin on 2020/6/1.
//

#include "ExactSolver.h"

//int extractTree(int _n, int edge_cnt){
//    if()
//}
int ExactSolver::get_max_degree_vertex() {
    while(degreeHead[max_d] == NULL && max_d != 0) --max_d;
    if(degreeHead[max_d] == NULL) return -1;
    else return degreeHead[max_d]->no;
}
int ExactSolver::extractTree(ui residual_n, int k, int sz) {
    if(k < 0)   return -1;
    if(residual_n == 0) return sz;

    int u = get_max_degree_vertex();
    assert(u != -1 && is[u]);
    rmVertex(u);
    inAnswer[u] = false;
    int not_in = extractTree(residual_n -1, k, sz);

    inAnswer[u] = true;
    for(ui i = pstart[u]; i < pstart[u + 1]; ++i){
        ui y = edges[i];
        ++weight[y];
    }

    int in = extractTree(residual_n - 1, k - weight[u], sz+1);
    inAnswer[u] = false;
    for(ui i = pstart[u]; i < pstart[u + 1]; ++i){
        ui y = edges[i];
        --weight[y];
    }
    putVertex(u);

    return (not_in > in)? not_in : in;
}

void ExactSolver::init(int &k) {
    for(ui i = 0; i < gs_length; ++i){
        ui x = given_set[i];

        rmVertex(x);
        k -= weight[x];
        inAnswer[i] = true;
        for(ui j = pstart[x]; j < pstart[x + 1]; ++j){
            ui y = edges[j];
            ++weight[y];
        }
    }
}

void ExactSolver::mvDegreeList(ui u, ui old_d, ui new_d) {
    if(new_d > max_d)  max_d = new_d;
    if(old_d != -1){
        if(degreeHead[old_d] == &pool[u])
            degreeHead[old_d] = pool[u].nxt;
        if(pool[u].bf != NULL)  pool[u].bf->nxt = pool[u].nxt;
        if(pool[u].nxt != NULL) pool[u].nxt->bf = pool[u].bf;
    }
    if(new_d != -1){
        pool[u].nxt = degreeHead[new_d];
        degreeHead[new_d]->bf = pool + u;
        pool[u].bf = NULL;
        degreeHead[new_d] = pool + u;
    }else{
        pool[u].bf = pool[u].nxt = NULL;
    }
}

void ExactSolver::rmVertex(ui u) {
    is[u] = 0;
    for(ui i  = pstart[u]; i < pstart[u + 1]; ++i){
        ui y = edges[i];
        if(is[y])  mvDegreeList(y, degree[y], degree[y] - 1);
        --degree[y];
    }
    mvDegreeList(u, degree[u], -1);
}

void ExactSolver::putVertex(ui u) {
    is[u] = 1;
    for(ui i = pstart[u]; i < pstart[u + 1]; ++i){
        ui y = edges[i];
        if(is[y])   mvDegreeList(y, degree[y], degree[y] + 1);
        ++degree[y];
    }
    mvDegreeList(u, -1, degree[u]);
}

int ExactSolver::solve(int k){
    init(k);
    return extractTree(n - gs_length, k, gs_length);
}