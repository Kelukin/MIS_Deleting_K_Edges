//
// Created by Kelukin on 2020/5/15.
//

#include "ProfitDeleteIndex.h"
int ProfitDeleteIndex::getRankValue(DeleteBlock* blk, int value) {
    if(value <= 0) return -1; // we need not to rank this one
    value = degree[blk->y] / value;
    if(value > last_max) last_max = value;
    return value;
}

std::pair<int, std::pair<ui, ui> > ProfitDeleteIndex::getMinCostEdge() {
    while(last_max > 0){
        while(bin[last_max] == nullptr && last_max > 0) --last_max;
        if(last_max <= 0) break;
        while(bin[last_max] != nullptr){
            DeleteBlock* cur = bin[last_max];
            if( !is[cur->x] || !is[cur->y] || cur->try_flag){
                bin[last_max] = cur->nxt;
            }else break;
        }

        if(bin[last_max] != nullptr) break;
    }
    DeleteBlock* ret = bin[last_max];
    if(last_max <= threshold) return std::make_pair(1<<30, std::make_pair(0, 0));
    else return std::make_pair(ret->deleteCost, std::make_pair(ret->x, ret->y));
}