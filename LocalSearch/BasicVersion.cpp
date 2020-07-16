//
// Created by Kelukin on 2020/7/12.
//

#include "BasicVersion.h"
#include "../Graph.h"
extern int TIME_THRESHOLD;
void BasicVersion::pertubation(char *is, int &res_k, std::vector<int>& inArea, bool* in) {
    bool canFinish = false;
    inArea.clear();
    auto it = history.begin();
    while(it != history.end()){
        auto tmpIt = it;
        ++tmpIt;
        if(in[*it]){
            history.erase(it);
        }else{
            if(rand() & 1){
                inArea.push_back(*it);
                in[*it] = true;
                history.erase(it);
                canFinish = true;
            }
            if(canFinish){
                if(rand() & 1)  break;
            }
        }
        it = tmpIt;
    }
}
bool BasicVersion::canStop() {
#ifdef __LINUX__
    gettimeofday(&end, NULL);
    double mtime = end.tv_sec - start.tv_sec +
            (end.tv_usec - start.tv_usec) / 1000000.0;
    if(mtime >= TIME_THRESHOLD)
        return  true;
#endif
    return false;
}
void BasicVersion::iterate_local_search() {
#ifndef NDEBUG
    check_is(currentSolution, c_k, n, total_k, pstart, edges);
#endif
    std::vector<int> inArea;
    char *tmpIs = new char[n];
    bool* in = new bool[n];
    NodeList* tmpPool = new NodeList[n];
    NodeList** inQDegreeHead_tmp = new NodeList*[maxd];
    NodeList** outQDegreeHead_tmp = new NodeList*[maxd];
#ifdef __LINUX__
    gettimeofday(&start, NULL);
#endif
    memcpy(tmpIs, currentSolution, sizeof(char) * n);
    for(ui i = 0; i < n; ++i)
        in[i] = currentSolution[i] != 0;
    while(!canStop()){
        memcpy(outQDegreeHead_tmp, outQDegreeHead, sizeof(NodeList*) * maxd);
        memcpy(inQDegreeHead_tmp, inQDegreeHead, sizeof(NodeList*) * maxd);
        memcpy(tmpPool, pool, sizeof(NodeList) * n);
        memcpy(tmpIs, currentSolution, sizeof(char) * n);
        int tmpSize = cSize;
        int tmp_k =  c_k;
        pertubation(tmpIs, tmp_k, inArea, in);
        if(tmp_k >= 0 ) localsearch(tmpIs, tmpSize, tmp_k, pool, inQDegreeHead_tmp, outQDegreeHead_tmp, inArea);
        if(accept(tmpSize, tmp_k)){

            std::swap(tmpIs, currentSolution);
            cSize = tmpSize;
            c_k = tmp_k;
            std::swap(inQDegreeHead_tmp, inQDegreeHead);
            std::swap(outQDegreeHead_tmp, outQDegreeHead);
            for(ui i = 0; i < n; ++i)
                if(in[i] != currentSolution[i]){
                    if(in[i]){
                        history.push_back(i);
                    }
                    in[i] = currentSolution[i] > 0;
                }

            if(tmpSize > oSize){
                oSize = tmpSize;
                memcpy(optimumSolution, currentSolution, sizeof(char) * n);
#ifdef __LINUX__
                gettimeofday(&end, NULL);
                double duration = end.tv_sec - start.tv_sec
                                    + (end.tv_usec - start.tv_usec) / 1000000.0;
                printf("get |Q| = %d, at %f s \n",
                        oSize, duration);
#endif
            }
#ifndef NDEBUG
            check_is(currentSolution, c_k, n, total_k, pstart, edges);
#endif
        }else{
            memcpy(pool, tmpPool, sizeof(NodeList) * n);
            for(auto &x:inArea){
                history.push_back(x);
                in[x] = false;
            }
        }
    }
    delete[] in;
    delete[] tmpIs;
    delete[] tmpPool;
    delete[] inQDegreeHead_tmp;
    delete[] outQDegreeHead_tmp;
}

void BasicVersion::localsearch(char *is, int& tmpSize, int &res_k, NodeList* pool
        , NodeList** inList, NodeList** outList, std::vector<int> &inArea) {
    // the ub_in and lb_out call for backing up before entering this function
    // we can assume that the outList and inList have been well maintained.

    int lb_out = 0;
    int ub_in = maxd - 1;

    for(auto &x : inArea){
        rmNodeList(pool + x, outList, pool[x].degree);
        ++tmpSize;
        is[x] = 3;
        for(ui i = pstart[x]; i < pstart[x + 1]; ++i){
            ui y = edges[i];
            if(is[y] == 1){
                --tmpSize;
                put_out_S(y, is, pstart, edges, pool, lb_out, inList, outList);
//                res_k += pool[y].degree;
                for(ui j = pstart[y]; j < pstart[y + 1]; ++j){
                    ui z = edges[j];
                    if(is[z] == 1 || is[z] == 2){
//                        printf("%d %d\n", y, z);
                        ++res_k;
                    }
                }
            }else if(is[y] == 2 || is[y] == 3) --res_k;
            assert(is[y] != 1);
        }
    }
    for(auto &x:inArea){
        is[x] = 2;
        for(ui i= pstart[x]; i < pstart[x + 1]; ++i){
            ui y = edges[i];
            if(is[y] < 2){
                if(is[y]){
                    rmNodeList(pool + y, inList, pool[y].degree);
                    putNodeList(pool + y, inList, pool[y].degree + 1);
                }else{
                    rmNodeList(pool + y, outList, pool[y].degree);
                    putNodeList(pool + y, outList, pool[y].degree + 1);
                }
            }
        }
    }
#ifndef NDEBUG
    check_is(is, res_k, n, total_k, pstart, edges);
#endif
    while(ub_in > lb_out || lb_out <=res_k){
        while(outList[lb_out] == NULL && lb_out < n) ++lb_out;
        while(inList[ub_in] == NULL && ub_in > 0) ub_in--;
        if(lb_out == n) break;

        // we may add the module of (1,2)swap

        if(lb_out == 0){
            while(outList[0] != NULL){
                NodeList* node = outList[0];
                put_in_S(node->no, is, pstart, edges, pool
                        , ub_in, inList, outList);
                ++tmpSize;

            }
            continue;
        }

        if(ub_in > lb_out){
            NodeList* a = inList[ub_in];

            int nodeNo = a->no;
            res_k += a->degree;
            put_out_S(nodeNo, is, pstart, edges, pool,
                      lb_out, inList, outList);

            NodeList* b = outList[lb_out];
            res_k-= lb_out;
            nodeNo = b->no;
            put_in_S(nodeNo, is, pstart, edges, pool,
                     ub_in, inList, outList);

        }else{
            if(lb_out > res_k) continue;
            NodeList* a = outList[lb_out];
            res_k -= lb_out;
            int nodeNo = a->no;
            put_in_S(nodeNo, is, pstart, edges, pool,
                     ub_in, inList, outList);

            ++tmpSize;
        }
    }

    for(auto &x : inArea){
        int cnt = 0;
        for(ui i = pstart[x]; i < pstart[x + 1]; ++i)
            if(is[edges[i]])    ++cnt;

        putNodeList(pool + x, inList, cnt);
        is[x] = 1;
    }
}

bool BasicVersion::accept(int tmpSize, int res_k) {
    static int rejectCnt = 0;
    if(res_k < 0) return false;
    if(tmpSize > cSize){
        rejectCnt = 0;
        return true;
    }
    if(rejectCnt > cSize){
        long long base = 1 + 1LL *(cSize - tmpSize) * (oSize - tmpSize);
        if( (rand() % base) == 1) return  true;
    }
    ++rejectCnt;
    return false;
}

char* BasicVersion::getOptimum() {
    return optimumSolution;
}