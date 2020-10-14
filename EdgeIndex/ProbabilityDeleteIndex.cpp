//
// Created by Kelukin on 2020/9/15.
//

#include "ProbabilityDeleteIndex.h"
extern  long long BUILT_INDEX_TIME;
void ProbabilityDeleteIndex::calProbability() {
#ifdef __LINUX__
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    FILE *fp = fopen(LOG_PATH.c_str(), "r");
    int x, y;
    while(fscanf(fp, "%d%d", &x, &y) != EOF){
        unsigned long long hash = edge2Ull(x, y);
        auto it = edgeCnt.find(hash);
        if(it == edgeCnt.end())
            edgeCnt[hash] = 1;
        else it->second = it->second + 1;
    }

    for(auto it = edgeCnt.begin(); it != edgeCnt.end(); ++it){
        qu.push(EdgeCnt(it->first, it->second));
    }

    fclose(fp);
#ifdef __LINUX__
    gettimeofday(&end, NULL);
    long long  seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    BUILT_INDEX_TIME = seconds * 1000000 + useconds;
#endif

    printf("The log read for estimating probability has been finished!\n");
}

std::pair<ui, ui> ProbabilityDeleteIndex::recommendEdge() {
    auto p = qu.top();
    auto hashValue = p.edgeHash;

    //decode

    ui x = hashValue >> 32;
    ui y = (hashValue << 32) >> 32;
    qu.pop();
    return std::make_pair(x, y);
}

bool ProbabilityDeleteIndex::empty() {
    return qu.empty();
}