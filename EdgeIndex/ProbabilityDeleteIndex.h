//
// Created by lj on 2020/9/15.
//

#ifndef MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
#define MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H

#include "BaseEdgeIndex.h"
#include "../Utility.h"
#include <unordered_map>
#include <queue>
extern std::string LOG_PATH;
typedef std::pair<unsigned long long, double> edge_cnt;
struct EdgeCnt{
    unsigned long long edgeHash;
    double cnt;
    EdgeCnt(unsigned long long _hash, double _cnt): edgeHash(_hash), cnt(_cnt){}
    bool operator <(const EdgeCnt &ano) const {
        return cnt < ano.cnt;
    }
};
class ProbabilityDeleteIndex : public BaseEdgeIndex {
protected:
    static unsigned long long edge2Ull(ui x, ui y){
        return ((unsigned long long)x << 32) | y;
    }
    std::priority_queue<EdgeCnt> qu;
public:
    ProbabilityDeleteIndex(ui n, ui m, int max_d, ui* edges, char* _is, ui* r_edges, ui *pstart, ui *pend, int const *_degree):
            BaseEdgeIndex(n, m, max_d, edges, _is, r_edges, pstart, pend, _degree){
    }
    void calProbability();
    void calProbability(ui* gs_set, ui gs_len);
    std::pair<ui, ui> recommendEdge();
    bool empty();
};


#endif //MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
