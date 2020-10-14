//
// Created by Kelukin on 2020/7/12.
//

#ifndef MIS_DELETING_K_EDGES_BASICVERSION_H
#define MIS_DELETING_K_EDGES_BASICVERSION_H

#include "../Utility.h"
#include <list>
#include <time.h>
#include <cstdlib>

extern std::string LOG_PATH;
extern bool LOG_FLAG;
class BasicVersion {
protected:
#ifdef __LINUX__
    struct timeval start, end;
#endif
    ui n, m;
    ui *given_set;
    ui gs_length;
    ui *pstart;
    ui *edges;
    int total_k;
    char* currentSolution;
    std::list<int> history;
    int cSize;
    int c_k;
    char* optimumSolution;
    int oSize;
    NodeList *pool;
    NodeList **inQDegreeHead;
    NodeList **outQDegreeHead;
    unsigned int maxd;
    int lb_out, ub_in;
    FILE* fp;
public:
    BasicVersion(const BasicVersion &obj) = delete;
    BasicVersion(ui _n, ui _m, ui* _give_set
            , ui _gs_length, ui* _pstart, ui* _edges, ui _k, char* is): n(_n), m(_m)
            , given_set(_give_set), gs_length(_gs_length), pstart(_pstart),
            edges(_edges), total_k(_k){
        fp = NULL;
        srand(time(NULL));
        maxd = 0;
        lb_out = n;
        ub_in = 0;
        pool = new NodeList[n];
        for(ui i = 0; i < n; ++i)   maxd = std::max(maxd, pstart[i+1] - pstart[i]);
        ++maxd;

        inQDegreeHead = new NodeList*[maxd];
        outQDegreeHead = new NodeList*[maxd];
        for(ui i = 0; i < maxd; ++i){
            inQDegreeHead[i] = NULL;
            outQDegreeHead[i] = NULL;
        }
        for(ui i = 0; i < n; ++i)   pool[i].no = i;
        ui tmp_cnt = 0;
        for(ui i = 0; i < n; ++i)
            if(is[i]){
                for(ui j = pstart[i]; j < pstart[i + 1]; ++j)
                    if(is[edges[j]]) ++tmp_cnt;
            } else
                history.push_back(i);


        c_k = total_k - tmp_cnt / 2;


        for(ui i = 0; i < gs_length; ++i)
            is[given_set[i]] = 2;

        for(ui i = 0; i < n; ++i)
            // initialize the NodeList
            if(is[i] != 2){
                int degreeCnt = 0;
                for(ui j = pstart[i]; j < pstart[i + 1]; ++j)
                    if(is[edges[j]] != 0) ++degreeCnt;
                NodeList **degreeHead;
                if(is[i] == 0){
                    assert(degreeCnt != 0);
                    lb_out = std::min(lb_out, degreeCnt);
                    degreeHead = outQDegreeHead;
                }else{
                    ub_in = std::max(ub_in, degreeCnt);
                    degreeHead = inQDegreeHead;
                }

                putNodeList(pool + i, degreeHead, degreeCnt);
            }


        cSize = 0;
        currentSolution = new char[n];
        memcpy(currentSolution, is, sizeof(char) * n);

        for(ui i = 0; i < n; ++i)   if(is[i]) ++cSize;
        oSize = cSize;

        optimumSolution = new char[n];
        memcpy(optimumSolution, is, sizeof(char) * n);
    }
    virtual void perturbation(char *is, int &res_k, std::vector<int>& inArea, bool* in);
    virtual void localSearch(char *is, int& tmpSize, int &res_k, NodeList* pool
            , NodeList** inList, NodeList** outList, std::vector<int> &inArea);
    virtual bool accept(int tmpSize, int res_k);
    virtual char* getOptimum();
    void outputLog(std::vector<int>& inArea, std::vector<int>& outArea, int old_size, int new_size);

    void outputLogConflictEdges();
    int getOptimumSize(){
        return oSize;
    }
    virtual bool canStop();
    void iterate_local_search();
    int getVertexNum(){
        return n;
    }
    virtual ~BasicVersion(){
        if(fp != NULL)  fclose(fp);
        delete[] currentSolution;
        delete[] optimumSolution;
        delete[] pool;
        delete[] inQDegreeHead;
        delete[] outQDegreeHead;
    }
};


#endif //MIS_DELETING_K_EDGES_BASICVERSION_H
