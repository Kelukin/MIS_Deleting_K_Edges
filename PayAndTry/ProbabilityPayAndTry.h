//
// Created by lj on 2020/9/18.
//

#ifndef MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
#define MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
#include "NearLinearPayAndTry.h"
#include "../EdgeIndex/ProbabilityDeleteIndex.h"
class ProbabilityPayAndTry : public NearLinearPayAndTry {
protected:
    EdgeDeleteIndex *edgeDeleteIndex;
    ui *r_edges;
    ui *pend;
public:
    void init();
    void inexact_delete();
    int pay();
    void last_delete();
    void appending_action();
    void debugCheck();
    bool canSafeReduction();
    bool canRiskReduction();
    void riskReduction();
    bool isDone();
    void localSearch();
    ProbabilityPayAndTry(ui _n, ui _m, ui *_given_set, ui _gs_length,
                         ui *_pstart, ui *_edges, ui _k):
                         NearLinearPayAndTry(_n, _m, _given_set, _gs_length, _pstart, _edges, _k){
        // I might should put the initialization of edgeDeleteIndex in the `init' method
    }
};


#endif //MIS_DELETING_K_EDGES_PROBABILITYPAYANDTRY_H
