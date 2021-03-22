//
// Created by Kelukin on 2020/9/15.
//

#include "ProbabilityDeleteIndex.h"
#include "set"
extern  long long BUILT_INDEX_TIME;
void ProbabilityDeleteIndex::calProbability() {
    // corresponding to the older version, which does not contain
    // the information about the containing vertices
    // It does not call for the information about the query set
    std::unordered_map<unsigned long long, int> edgeCnt;
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

void ProbabilityDeleteIndex::calProbability(ui *gs_set, ui gs_len) {
    std::unordered_map<unsigned long long, double> edgeCnt;
#ifdef __LINUX__
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    std::set<ui> inGS;
    for(int i = 0; i < gs_len; ++i) inGS.insert(gs_set[i]);
    FILE *fp = fopen(LOG_PATH.c_str(), "r");
    int set_size = 0;
    int x, y;
    while(fscanf(fp, "%d", &set_size) != EOF){
        int in_cnt(0); // it is about the weight of this local optima
        int* tmp_set = new int[set_size];
        for(int i = 0; i < set_size; ++i){
            int tmp;
            fscanf(fp, "%d", &tmp);
            tmp_set[i] =tmp;
            if(inGS.find(tmp) != inGS.end())    ++in_cnt;
        }

        if(in_cnt){
            for(int i = 0; i < set_size; ++i)   ++vertexWeight[tmp_set[i]];
        }

        delete[] tmp_set;
        // 使用Jaccord Similarity 本身并不合理
        // 因为希望的是，set_size越大，包含的点愈多则愈加合理
        // 当前机制下会惩罚过大的set_size
        double edge_score = double(in_cnt) / gs_len * (double(set_size) / vertex_num);

        while(fscanf(fp, "%d%d", &x, &y) != EOF){
            if(x == -1 && y == -1)  break;
            if(in_cnt == 0)  continue;

            unsigned long long hash = edge2Ull(x, y);
            auto it = edgeCnt.find(hash);
            if(it == edgeCnt.end())
                edgeCnt[hash] = edge_score;
            else it->second = it->second + edge_score;
        }

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