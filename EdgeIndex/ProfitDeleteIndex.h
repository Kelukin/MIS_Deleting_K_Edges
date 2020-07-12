//
// Created by Kelukin on 2020/5/15.
//
#include "EdgeDeleteIndex.h"
#ifndef MIS_DELETING_K_EDGES_PROFITDELETEINDEX_H
#define MIS_DELETING_K_EDGES_PROFITDELETEINDEX_H

extern int THRESHOLD;
class ProfitDeleteIndex :public EdgeDeleteIndex{
protected:
    int last_max;
    int getRankValue(DeleteBlock* blk, int value) override ;
    int threshold;
public:
    ProfitDeleteIndex(ui n, ui m, int max_d, ui *edges, char *_is, ui *r_edges, ui *pstart, ui *pend, int const *_degree):
    EdgeDeleteIndex(n, m, max_d, edges, _is, r_edges, pstart, pend, _degree), last_max(-1), threshold(THRESHOLD){}
    ProfitDeleteIndex(const ProfitDeleteIndex &another) = delete;
    std::pair<int, std::pair<ui, ui> > getMinCostEdge() override;
};


#endif //MIS_DELETING_K_EDGES_PROFITDELETEINDEX_H
