//
// Created by lj on 2020/9/18.
//

#ifndef MIS_DELETING_K_EDGES_BASEEDGEINDEX_H
#define MIS_DELETING_K_EDGES_BASEEDGEINDEX_H
#include "../Utility.h"
struct DeleteBlock{
    //delete block for one edge
    // if it is in bin, then y_nxt, y_pre is always nullptr
    // if it is not in bin and nodeList, then all pointer is nullptr
    int x, y;
    int deleteCost;
    int pre_bin_no;
    DeleteBlock* reverse;
    bool valid;
    bool try_flag;
    // just record that whether one edge has been restored or not
    // is the valid value has been set to true, we cannot get another k from it
    DeleteBlock *nxt, *pre;
    explicit DeleteBlock(int _x = 0, int _y = 0): x(_x), y(_y), deleteCost(-1), pre_bin_no(0){
        nxt = pre = reverse =  nullptr;
        valid = true;
        try_flag = false;
    }
    DeleteBlock(const DeleteBlock &obj)= default;

};

class BaseEdgeIndex {
protected:
    char* is;
    DeleteBlock** edge2deleteBlock;
    DeleteBlock** nodeList;
    DeleteBlock* pools;
    int const *degree;
    int _max_d;
    ui up;
public:
    BaseEdgeIndex(ui n, ui m, int max_d, ui *edges, char *_is, ui *r_edges, ui* pstart, ui *pend, int const *_degree){
        degree = _degree;
        up = m;
        pools = new DeleteBlock[m];
        _max_d = max_d;
        is = _is;

        nodeList = new DeleteBlock*[n];
        for(ui i = 0; i < n; ++i)   nodeList[i] = nullptr;
        edge2deleteBlock = new DeleteBlock*[m];
        for(ui i = 0; i < m; ++i)   edge2deleteBlock[i] = nullptr;
        int cnt = 0;
        for(ui i = 0; i < n; ++i){
            if(!is[i])  continue;
            else{
                for(ui u = pstart[i]; u < pend[i]; ++u){
                    int x = edges[u];
//                    if(!is[x]) continue;
                    if(edge2deleteBlock[u]) continue;

                    pools[cnt].x = i; pools[cnt].y = x;
                    pools[cnt + 1].x = x; pools[cnt + 1].y = i;
                    pools[cnt].reverse = pools + cnt + 1;
                    pools[cnt + 1].reverse = pools + cnt;
                    edge2deleteBlock[u] = pools + cnt;
                    edge2deleteBlock[r_edges[u]] = pools + cnt + 1;
                    assert(r_edges[u] <up);
                    cnt += 2;
                }
            }
        }

    }
    virtual ~BaseEdgeIndex(){
        delete[] edge2deleteBlock;
        delete[] pools;
    }
    BaseEdgeIndex(const BaseEdgeIndex &ano) = delete;
    void swap(ui edge_a, ui edge_b);

    void setReverseRelation(ui edgeNo_a, ui edgeNo_b);
    bool hasDeletedEdge(ui u);
    int deleteVertex(ui no);
    void edgeRewire(ui edgeNo, ui u, ui old_v, ui new_v);
    void deleteEdge(ui edge_no);

};


#endif //MIS_DELETING_K_EDGES_BASEEDGEINDEX_H
