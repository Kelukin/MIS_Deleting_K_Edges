//
// Created by lj on 2020/9/18.
//

#include "BaseEdgeIndex.h"

void BaseEdgeIndex::swap(ui edge_a, ui edge_b) {
    DeleteBlock* block = edge2deleteBlock[edge_a];
    edge2deleteBlock[edge_a] = edge2deleteBlock[edge_b];
    edge2deleteBlock[edge_b] = block;
}

void BaseEdgeIndex::setReverseRelation(ui edgeNo_a, ui edgeNo_b) {
    DeleteBlock* aBlock = edge2deleteBlock[edgeNo_a];
    DeleteBlock* bBlock = edge2deleteBlock[edgeNo_b];
    aBlock->reverse = bBlock;
    bBlock->reverse = aBlock;
}

bool BaseEdgeIndex::hasDeletedEdge(ui u) {
    DeleteBlock* tmp = nodeList[u];
    while(tmp != nullptr){
        if(tmp->valid) return true;
        tmp = tmp->nxt;
    }
    return false;
}

int BaseEdgeIndex::deleteVertex(ui no) {
    // return the number of k to be restored.
    int cnt = 0;
    DeleteBlock* tmp = nodeList[no];
#ifndef NDEBUG
    if(tmp != nullptr){
        if(tmp->pre != NULL)
            printf("error Node: %u\n", no);
        assert(tmp->pre == NULL);
    }
#endif
    while(tmp != nullptr){
        // Try to drawback the K value
        if(tmp->valid)  ++cnt;


        DeleteBlock* tmp_r = tmp->reverse;
        if(tmp_r->pre) {
            tmp_r->pre->nxt = tmp_r->nxt;
        }
        if(tmp_r->nxt) {
            tmp_r->nxt->pre = tmp_r->pre;
        }
        if(nodeList[tmp_r->x] == tmp_r) {
            nodeList[tmp_r->x] = tmp_r->nxt;
        }

        tmp_r->pre = tmp_r->nxt = nullptr;

        //judge if tmp_r -> x no longer related to a deleted edge.

        tmp->valid = tmp->reverse->valid = false;
        tmp = tmp -> nxt;
    }
//    printf("%d\n", drawbackCnt);
    return cnt;
}

void BaseEdgeIndex::edgeRewire(ui edgeNo, ui u, ui old_v, ui new_v) {
    DeleteBlock *cur = edge2deleteBlock[edgeNo];
    if((cur->x == u && cur->y == new_v)
       || (cur->x == new_v && cur->y == u))
        return;
    assert(cur->x == old_v || cur -> y == old_v);
    if(cur->x == old_v) cur->x = new_v;
    else cur-> y = new_v;
}
void BaseEdgeIndex::deleteEdge(ui edge_no) {
    DeleteBlock* cur = edge2deleteBlock[edge_no];
    if(cur->try_flag)    return;
    cur->try_flag = true;
    if(!cur->valid){
        printf("error edge no: %d\n", edge_no);
    }
    assert(cur != nullptr && cur->valid);
    //maintain the list structure in bin firstly
    if(cur->pre != nullptr)   cur->pre->nxt = cur->nxt;
    if(cur->nxt != nullptr)   cur->nxt->pre = cur->pre;

    //maintain the list structure in nodeList
    DeleteBlock* tmp = nodeList[cur->x];
    if(tmp != nullptr) tmp->pre = cur;
    cur->nxt = tmp;
    cur->pre = nullptr;
    nodeList[cur->x] = cur;
}