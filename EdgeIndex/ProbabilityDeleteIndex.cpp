//
// Created by Kelukin on 2020/9/15.
//

#include "ProbabilityDeleteIndex.h"
void ProbabilityDeleteIndex::calProbability() {
    FILE *fp = fopen(LOG_PATH, "r");
    int x, y;
    while(fscanf(fp, "%d%d", &x, &y) != EOF){
        unsigned long long hash = edge2Ull(x, y);
        auto it = edgeCnt.find(hash);
        if(it == edgeCnt.end())
            edgeCnt[hash] = 1;
        else it->second = it->second + 1;
    }

    for(auto it = edgeCnt.begin(); it != edgeCnt.end(); ++it){
        qu.push(std::make_pair(it->first, it->second));
    }

}

std::pair<ui, ui> ProbabilityDeleteIndex::recommendEdge() {
    if(qu.empty())  return std::make_pair(-1, -1);
    else{
        auto p = qu.top();
        auto hashValue = p.first;

        //decode

        ui x = hashValue >> 32;
        ui y = (hashValue << 32) >> 32;
        qu.pop();
        return std::make_pair(x, y);
    }
}