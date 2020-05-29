//
// Created by Kelukin on 2020/5/27.
//


#ifndef MIS_DELETING_K_EDGES_RISKPAYANDTRY_H
#define MIS_DELETING_K_EDGES_RISKPAYANDTRY_H

#include "NearLinearPayAndTry.h"
#include "../Widget/Notifier.h"
#include "../Utility.h"
class RiskPayAndTry : public NearLinearPayAndTry {
protected:
    void addDominated(ui u, ui v) override ;
    Notifier *notifier;
    std::vector<ui> riskDominated;
    int delete_vertex_dominate(ui u) override;
    void degree2Reduction();
    void clearRisk();
    char* inRisk;
    int reduct_cnt;
public:
    RiskPayAndTry(ui _n, ui _m, ui *_given_set, ui _gs_length
            , ui *_pstart, ui *_edges, ui _k):NearLinearPayAndTry(_n, _m, _given_set, _gs_length, _pstart, _edges, _k){
        notifier = new Notifier();
        reduct_cnt = 0;
        inRisk = new char[_n];
        memset(inRisk, 0, sizeof(char) * _n);
    }
    RiskPayAndTry(const RiskPayAndTry& riskPayAndTry) = delete;
    void init() override ;
    void riskReduction() override;
    bool canRiskReduction() override;
    void safe_reduction() override;
    bool canSafeReduction() override ;
    virtual ~RiskPayAndTry() override {
        delete notifier;
        delete[] inRisk;
    }
};


#endif //MIS_DELETING_K_EDGES_RISKPAYANDTRY_H
