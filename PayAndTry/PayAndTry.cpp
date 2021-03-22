//
// Created by Kelukin on 2020/4/7.
//

#include "PayAndTry.h"



void  PayAndTry::localSearch() {
    BasicVersion localSearch(n, m, given_set, gs_length, pstart_copy, edges_copy, ori_k, is);
    localSearch.iterate_local_search();

}

int PayAndTry::getMISSize() {
    int ret = 0;
    for(ui i = 0; i < n; ++i)
        if(is[i])  ++ret;
    return ret;
}

void PayAndTry::get_two_neighbors(ui u, ui &u1, ui &u2) {
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(is[edges[i]]) {
        if(u1 == n) {
            u1 = edges[i];
            std::swap(edges[pstart[u]], edges[i]);
        }
        else {
            assert(u2 == n);
            u2 = edges[i];
            std::swap(edges[pstart[u]+1], edges[i]);
            break;
        }
    }
}


bool PayAndTry::exist_edge(ui u1, ui u2) {
    if(pstart[u1 + 1]-pstart[u1] < pstart[u2+1]-pstart[u2]) {
        for(ui i = pstart[u1];i < pstart[u1+1];i ++) if(edges[i] == u2 ) return true;
        return false;
    }
    for(ui i = pstart[u2];i < pstart[u2+1];i ++) if(edges[i] == u1) return true;
    return false;
}

int PayAndTry::init_given_set() {
    ui cost = 0;
    for(ui i = 0 ; i < gs_length; ++i){
        int x = given_set[i];
        is[x] = 2;
    }

    for(ui i = 0; i < gs_length; ++i){
        int x = given_set[i];
        for(ui k = pstart[x]; k < pstart[x + 1]; ++k)
            if(is[edges[k]] == 2)   ++cost;
            else if(is[edges[k]] == 1){
                delete_vertex(edges[k], is, degree);
            }
    }
    cost >>= 1;

    for(ui i = 0; i < gs_length; ++i){
        int x = given_set[i];
        delete_vertex(x, is, degree);
    }

    return cost;
}

void PayAndTry::delete_vertex(ui v, char *is, int *degree) {
    is[v] = 0;
    for(ui k = pstart[v]; k < pstart[v + 1]; ++k)
        if(is[edges[k]]){
            int w = edges[k];
            --degree[w];
        }
}

