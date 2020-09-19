//
// Created by Kelukin on 2020/4/13.
//

#ifndef MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
#define MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
#include "../Utility.h"
#include "../Widget/Notifier.h"
#include "BaseEdgeIndex.h"
class EdgeDeleteIndex : public BaseEdgeIndex {
protected:
    int* diff;
    DeleteBlock** bin;
    int drawbackCnt;
    Notifier* notifier;
    virtual int getRankValue(DeleteBlock* blk, int value);
public:
    EdgeDeleteIndex(ui n, ui m, int max_d,
                    ui *edges, char *_is, ui *r_edges,
                    ui *pstart, ui *pend, int const *_degree): BaseEdgeIndex(n, m, max_d, edges,
                                                                             _is, r_edges, pstart,
                                                                             pend, _degree)
    {
        notifier = nullptr;
        drawbackCnt = 0;
        diff = new int[m];
        memset(diff, 0xff, sizeof(int) * m);

        bin = new DeleteBlock*[max_d];
        for(ui i = 0; i < max_d; ++i)   bin[i] = nullptr;
    }
    virtual ~EdgeDeleteIndex(){
        delete[] diff;
        delete[] bin;
    }
    EdgeDeleteIndex(const EdgeDeleteIndex &obj) = delete;
    void setDiffValue(ui edgeNo, int value);
    int deleteVertex(ui no);
    virtual  std::pair<int, std::pair<ui, ui> > getMinCostEdge();
    void descDiffValue(ui edgeNo);
    void incDiffValue(ui edgeNo);
    int getDiffValue(int no);
    void swap(ui edge_a, ui edge_b);
    void deleteEdge(ui edge_no);
    void setNotifier(Notifier *_notifier);
};


#endif //MIS_DELETING_K_EDGES_EDGEDELETEINDEX_H
