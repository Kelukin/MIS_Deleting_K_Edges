//
// Created by lj on 2020/9/18.
//

#ifndef MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
#define MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
#include "NearLinearPayAndTry.h"
#include "../EdgeIndex/ProbabilityDeleteIndex.h"
class ProbabilityPayAndTry : public NearLinearPayAndTry {
protected:
    ProbabilityDeleteIndex *edgeDeleteIndex;
    ui *r_edges;
//    ui *pend;
    void deleteEdge(int edgeNo, int u, int v);
    ui edge_rewire(ui u, const ui *pend, ui v, ui w);
    void shrink(ui u, ui &end);
    void addDominated(ui u, ui v = -1);
    void update_triangle(ui u1, ui u2);
    int delete_vertex_dominate(ui u);
    int compute_triangle_counts();
public:
    void init();
//    void inexact_delete();
    int pay();
    void safe_reduction();
    ProbabilityPayAndTry(ui _n, ui _m, ui *_given_set, ui _gs_length,
                         ui *_pstart, ui *_edges, ui _k):
                         NearLinearPayAndTry(_n, _m, _given_set, _gs_length, _pstart, _edges, _k){
        edgeDeleteIndex = NULL;
        // I might should put the initialization of edgeDeleteIndex in the `init' method
    }
};


#endif //MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
