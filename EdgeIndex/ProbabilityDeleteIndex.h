//
// Created by lj on 2020/9/15.
//

#ifndef MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
#define MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H

#include "BaseEdgeIndex.h"
#include "../Utility.h"
#include <unordered_map>
#include <queue>
#include <set>
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
private:
    void processOneFile(FILE* fp,
                        std::unordered_map<unsigned long long, double> &edgeCnt,
                        std::set<ui> &inGS,
                        ui gs_len,
                        double &sample_sum,
                        int edgeWeight = 1);
protected:
    static unsigned long long edge2Ull(ui x, ui y){
        return ((unsigned long long)x << 32) | y;
    }
    std::priority_queue<EdgeCnt> qu;
    double* vertexWeight;
    ui tot_k;
public:
    ProbabilityDeleteIndex(ui n, ui m, int max_d, ui* edges, char* _is, ui* r_edges, ui *pstart, ui *pend, int const *_degree, ui _k):
            BaseEdgeIndex(n, m, max_d, edges, _is, r_edges, pstart, pend, _degree){
        tot_k = _k;
        vertexWeight = new double[n];
        memset(vertexWeight, 0, sizeof(int) * n);
    }
    ~ProbabilityDeleteIndex(){
        delete[] vertexWeight;
    }
    void calProbability();
    void calProbability(ui* gs_set, ui gs_len);
    std::pair<ui, ui> recommendEdge();
    double getVertexWeight(ui v);
    bool empty();
};


#endif //MIS_DELETING_K_EDGES_PROBABILITYDELETEINDEX_H
