//
// Created by lj on 2020/9/15.
//

#ifndef MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
#define MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H

#include "EdgeDeleteIndex.h"
#include "../Utility.h"
#include <unordered_map>
#include <queue>
extern char LOG_PATH[];
typedef std::pair<unsigned long long, int> edge_cnt;
struct EdgeCnt{
    unsigned long long edgeHash;
    int cnt;
    EdgeCnt(unsigned long long _hash, int _cnt): edgeHash(_hash), cnt(_cnt){}
    bool operator <(const EdgeCnt &ano) const {
        return cnt < ano.cnt;
    }
};
class ProbabilityDeleteIndex : public EdgeDeleteIndex {
protected:
    unsigned long long edge2Ull(ui x, ui y){
        return ((unsigned long long)x << 32) | y;
    }
    std::unordered_map<unsigned long long, int> edgeCnt;
    void calProbability();
    std::priority_queue<EdgeCnt> qu;
public:
    ProbabilityDeleteIndex(ui n, ui m, int max_d, ui* edges, char* _is, ui* r_edges, ui *pstart, ui *pend, int const *_degree):
            EdgeDeleteIndex(n, m, max_d, edges, _is, r_edges, pstart, pend, _degree){
        calProbability();
    }
    std::pair<ui, ui> recommendEdge();
};


#endif //MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
