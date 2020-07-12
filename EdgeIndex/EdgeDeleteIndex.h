//
// Created by Kelukin on 2020/4/13.
//

#ifndef MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
#define MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
#include "../Utility.h"
#include "../Widget/Notifier.h"
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
class EdgeDeleteIndex {
protected:
    int* diff;
    char* is;
    DeleteBlock** edge2deleteBlock;
    DeleteBlock** nodeList;
    DeleteBlock* pools;
    DeleteBlock** bin;
    int _max_d;
    int drawbackCnt;
    int const *degree;
    ui up;
    Notifier* notifier;
    virtual int getRankValue(DeleteBlock* blk, int value);
public:
    EdgeDeleteIndex(ui n, ui m, int max_d, ui *edges, char *_is, ui *r_edges, ui *pstart, ui *pend, int const *_degree){
        notifier = nullptr;
        drawbackCnt = 0;
        degree = _degree;
        up = m;
        pools = new DeleteBlock[m];
        _max_d = max_d;
        diff = new int[m];
        is = _is;
        memset(diff, 0xff, sizeof(int) * m);

        nodeList = new DeleteBlock*[n];
        bin = new DeleteBlock*[max_d];
        for(ui i = 0; i < max_d; ++i)   bin[i] = nullptr;
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
        assert(cnt <= m);
    }
    virtual ~EdgeDeleteIndex(){
        delete[] diff;
        delete[] edge2deleteBlock;
        delete[] bin;
        delete[] pools;
    }
    EdgeDeleteIndex(const EdgeDeleteIndex &obj) = delete;
    void setDiffValue(ui edgeNo, int value);
    void swap(ui edge_a, ui edge_b);
    int deleteVertex(ui no);
    void setReverseRelation(ui edgeNo_a, ui edgeNo_b);
    virtual  std::pair<int, std::pair<ui, ui> > getMinCostEdge();
    void descDiffValue(ui edgeNo);
    void incDiffValue(ui edgeNo);
    void edgeRewire(ui edgeNo, ui u, ui old_v, ui new_v);
    int getDiffValue(int no);
    void deleteEdge(ui edge_no);
    bool hasDeletedEdge(ui u);
    void setNotifier(Notifier *_notifier);
};


#endif //MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
