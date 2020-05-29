//
// Created by Kelukin on 2020/4/13.
//

#include "EdgeDeleteIndex.h"

void EdgeDeleteIndex::setNotifier(Notifier *_notifier) {
    this->notifier = _notifier;
}

void EdgeDeleteIndex::setReverseRelation(ui edgeNo_a, ui edgeNo_b) {
    DeleteBlock* aBlock = edge2deleteBlock[edgeNo_a];
    DeleteBlock* bBlock = edge2deleteBlock[edgeNo_b];
    aBlock->reverse = bBlock;
    bBlock->reverse = aBlock;
}


void EdgeDeleteIndex::swap(ui edge_a, ui edge_b) {
    int t = diff[edge_a];
    diff[edge_a] = diff[edge_b];
    diff[edge_b] = t;

    DeleteBlock* block = edge2deleteBlock[edge_a];
    edge2deleteBlock[edge_a] = edge2deleteBlock[edge_b];
    edge2deleteBlock[edge_b] = block;
}
bool EdgeDeleteIndex::hasDeletedEdge(ui u) {
    DeleteBlock* tmp = nodeList[u];
    while(tmp != nullptr){
        if(tmp->valid) return true;
        tmp = tmp->nxt;
    }
    return false;
}

int EdgeDeleteIndex::deleteVertex(ui no) {
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
        if(notifier && tmp_r -> valid && !nodeList[tmp_r->x])
            notifier->addItem(tmp_r->x);

        tmp->valid = tmp->reverse->valid = false;
        tmp = tmp -> nxt;
    }
    drawbackCnt += cnt;
//    printf("%d\n", drawbackCnt);
    return cnt;
}

int EdgeDeleteIndex::getRankValue(DeleteBlock* blk, int value) {
    return value;
}

void EdgeDeleteIndex::deleteEdge(ui edge_no) {
    DeleteBlock* cur = edge2deleteBlock[edge_no];
    if(cur->try_flag)    return;
    cur->try_flag = true;
    if(!cur->valid){
        printf("error edge no: %d\n", edge_no);
    }
    assert(cur != nullptr && cur->valid);
    //maintain the list structure in bin firstly
    if(cur->pre != nullptr)   cur->pre->nxt = cur->nxt;
    else{
        //if it is in some bin's head?
//        bin[diff[edge_no]] = cur->nxt;
        bin[cur->pre_bin_no] = cur->nxt;
    }
    if(cur->nxt != nullptr)   cur->nxt->pre = cur->pre;

    //maintain the list structure in nodeList
    DeleteBlock* tmp = nodeList[cur->x];
    if(tmp != nullptr) tmp->pre = cur;
    cur->nxt = tmp;
    cur->pre = nullptr;
    nodeList[cur->x] = cur;
}

void EdgeDeleteIndex::setDiffValue(ui edgeNo, int value) {
    if(diff[edgeNo] == value)   return;

    DeleteBlock* cur = edge2deleteBlock[edgeNo];
//    if(cur == pools + 116942){
//        std::cout<<" value"<<value<<std::endl;
//        std::cout<<"x : "<<cur->x<<" "<<" y : "<<cur->y<<std::endl;
//    }
    if(!cur->valid || cur->try_flag){
        return;
    }
    if(diff[edgeNo] >= 0){
        // then it has been in a bin
        if(cur->pre != nullptr)   cur->pre->nxt = cur->nxt;
        if(cur->nxt != nullptr)   cur->nxt->pre = cur->pre;
//        assert(bin[diff[edgeNo]] != nullptr);
        if(cur == bin[cur->pre_bin_no])    bin[cur->pre_bin_no] = cur->nxt;

        cur->nxt = cur->pre = nullptr;
    }
    cur->deleteCost = value;
    diff[edgeNo] = value;
    value = getRankValue(cur, value);
    cur->pre_bin_no = value;
    if(value >= 0){
        DeleteBlock* tmp = bin[value];
        if(tmp != nullptr)  tmp->pre = cur;
        cur->pre = nullptr;
        cur->nxt = tmp;
        bin[value] = cur;
    }
}

std::pair<int, std::pair<ui, ui> > EdgeDeleteIndex::getMinCostEdge() {
    int cnt = 0;
    while(bin[cnt] != nullptr){
        DeleteBlock *cur = bin[cnt];
        if(!is[cur->x] || !is[cur->y] || cur->try_flag) {
            bin[cnt] = cur->nxt;
        } else break;
    }
#ifndef NDEBUG
    if(bin[0] != nullptr){
        DeleteBlock *cur = bin[cnt];
        printf("%d %d\n", cur->x, cur->y);
        printf("%c %c\n", is[cur->x], is[cur->y]);
    }
#endif
    assert(bin[0] == nullptr);
    while(true){
        while(bin[cnt] == nullptr && cnt < _max_d) ++cnt;
        if(cnt == _max_d) return std::make_pair(1<<30, std::make_pair(0, 0));
        while(bin[cnt] != nullptr){
            DeleteBlock *cur = bin[cnt];
            if(!is[cur->x] || !is[cur->y] || cur->try_flag){
                bin[cnt] = cur->nxt;
            }
            else break;
        }
        if(bin[cnt] != nullptr) break;
    }
    return std::make_pair(cnt, std::make_pair(bin[cnt]->x, bin[cnt]->y));
}

void EdgeDeleteIndex::descDiffValue(ui edgeNo) {
    this->setDiffValue(edgeNo, diff[edgeNo] - 1);
}

void EdgeDeleteIndex::incDiffValue(ui edgeNo) {
    this->setDiffValue(edgeNo, diff[edgeNo] + 1);
}
void EdgeDeleteIndex::edgeRewire(ui edgeNo, ui u, ui old_v, ui new_v) {
    DeleteBlock *cur = edge2deleteBlock[edgeNo];
    if((cur->x == u && cur->y == new_v)
        || (cur->x == new_v && cur->y == u))
        return;
    assert(cur->x == old_v || cur -> y == old_v);
    if(cur->x == old_v) cur->x = new_v;
    else cur-> y = new_v;
}

int EdgeDeleteIndex::getDiffValue(int no) {
    if( no > up) return -100;
    else return diff[no];
}