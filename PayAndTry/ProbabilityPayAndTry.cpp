//
// Created by lj on 2020/9/18.
//

#include "ProbabilityPayAndTry.h"
void ProbabilityPayAndTry::init() {
    for(ui i = 0; i < n; ++i)
        if(is[i]){
            if(degree[i] == 1)  degree_ones.push_back(i);
            else if(degree[i] == 2) degree_twos.push_back(i);
        }
    initial_dominance_and_degree_two_remove();
    ui *ids = new ui[n];
    ui ids_n = 0;
    ui new_m = 0;

    for(ui i = 0;i < n;i ++) {
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        ids[ids_n ++] = i;
        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m ++] = edges[j];
            }
    }
    pstart[n] = new_m;

    if(ids_n > 0) lp_reduction(ids, ids_n);
    assert(degree_ones.empty()&&degree_twos.empty());
    for(ui i = 0;i < ids_n;i ++) if(is[ids[i]]&&degree[ids[i]] > 0) {
            if(degree[ids[i]] == 1) degree_ones.push_back(ids[i]);
            else if(degree[ids[i]] == 2) degree_twos.push_back(ids[i]);
        }

    remove_degree_one_two();

    delete[] ids;
    new_m = 0;
    for(ui i = 0;i < n;i ++) { // shrink the memory for storing edges
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m ++] = edges[j];
            }
    }
    pstart[n] = new_m;

    for(ui i = 0;i < n;i ++) {
        pend[i] = pstart[i+1];
    }
    //printf("max_d: %d, edges: %u\n", max_dd, new_m/2);

    dominate = new char[n];
    memset(dominate, 0, sizeof(char) * n);
    //the default value of reverseEdge is quite large. must be larger than m
//    diff = new ui[new_m];

    if(new_m <= m/2) tri = edges+new_m;
    else {
        tri = new ui[pstart[n]];
        delete_tri = true; // to avoid repeating free the same memory space
    }


    compute_triangle_counts();

    new_m = 0;
    for(ui i = 0;i < n;i ++) { // shrink the memory for storing edges
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m] = edges[j];
                tri[new_m] = tri[j];
                ++new_m;
            }
    }
    pstart[n] = new_m;

    for(ui i = 0;i < n;i ++) {
        pend[i] = pstart[i+1];
    }




    reverseEdge = new ui[new_m];
    memset(reverseEdge, 0xff, sizeof(ui) * new_m);
    //initialize the triangle count and reverseEdge
    for(ui i = 0; i < n; ++i)
        if(is[i] && degree[i] > 0)
            for(ui u = pstart[i]; u < pend[i]; ++u){
                ui x = edges[u];
                if(!is[x]) continue;
                for(ui t = pstart[x]; t < pend[x] && reverseEdge[u] > m; ++t ){
                    ui y = edges[t];
                    if(y == i){
                        reverseEdge[u] = t;
                        reverseEdge[t] = u;
                        break;
                    }
                }
                assert(reverseEdge[u] < m);
            }
    bin_head = new int[ids_n];
    bin_next = new int[n];
    memset(bin_head, -1, sizeof(int) * ids_n);
    for(ui i = 0;i < n;i ++) if(is[i]&&degree[i] > 0) {
            bin_next[i] = bin_head[degree[i]];
            bin_head[degree[i]] = i;

            if(degree[i] == 2) degree_twos.push_back(i);
            if(degree[i] > max_d) max_d = degree[i];
        }

    fixed = new char[n];
    memset(fixed, 0, sizeof(char) * n);
    if(new_m != 0){
        edgeDeleteIndex = new  ProbabilityDeleteIndex(n, new_m, max_d, edges, is, reverseEdge, pstart, pend, degree);
    }
    else edgeDeleteIndex = nullptr;
}


//int ProbabilityPayAndTry::pay() {
//    if(k == 0)  return -1;
//    auto tmp = edgeDeleteIndex->
//}
void ProbabilityPayAndTry::inexact_delete() {}

int ProbabilityPayAndTry::pay() {
    return 0;
}

void ProbabilityPayAndTry::last_delete() {}

void ProbabilityPayAndTry::appending_action() {}

void ProbabilityPayAndTry::debugCheck() {}

bool ProbabilityPayAndTry::canSafeReduction() {
    return false;
}

bool ProbabilityPayAndTry::canRiskReduction() {

    return true;
}

void ProbabilityPayAndTry::riskReduction() {}

bool ProbabilityPayAndTry::isDone() {
    return true;
}

void ProbabilityPayAndTry::localSearch() {}