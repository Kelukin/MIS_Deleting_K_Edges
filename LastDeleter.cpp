//
// Created by Kelukin on 2020/4/8.
//

#include "LastDeleter.h"
LastDeleter* LastDeleter::lastDeleter = nullptr;
int LastDeleter::greedy_delete_edges(char *is, ui *pstart, ui *edges, ui k, ui n) {
    int res = 0;
    NodeList *pool = new NodeList[n];
    NodeList **degreeHead = new NodeList*[n];
    for(ui i = 0; i < n; i++){
        degreeHead[i] = NULL;
        pool[i].no = i;
    }

    for(ui i = 0; i < n; i++)
        if(is[i] == 0){
            int degreeCnt = 0;
            for(ui j = pstart[i]; j < pstart[i + 1]; j++)
                if(is[edges[j]] != 0)  ++degreeCnt;
            assert(degreeCnt != 0);
            if(degreeHead[degreeCnt] != NULL)   degreeHead[degreeCnt]->bf = pool + i;
            pool[i].bf = NULL;
            pool[i].nxt = degreeHead[degreeCnt];
            pool[i].degree = degreeCnt;
            degreeHead[degreeCnt] =  pool + i;
        }

    for(ui i = 0; i < n && k >= i; i++){
        while(degreeHead[i] != NULL && k >= i){
            NodeList *curr = degreeHead[i];
            if(curr->nxt)   curr->nxt->bf = NULL;
            degreeHead[i] = curr->nxt;

            int no = curr->no;
            int edgeCnt = 0;
            for(ui j = pstart[no]; j < pstart[no + 1]; j++)
                if(is[edges[j]] == 0){
                    ++edgeCnt;
                    int anotherNode = edges[j];
                    int d = pool[anotherNode].degree++;

                    if(degreeHead[d] == pool + anotherNode){
                        if(degreeHead[d]->nxt)  degreeHead[d]->nxt->bf = NULL;
                        degreeHead[d] = pool[anotherNode].nxt;
                    }else{
                        pool[anotherNode].bf->nxt = pool[anotherNode].nxt;
                        if(pool[anotherNode].nxt)   pool[anotherNode].nxt->bf = pool[anotherNode].bf;
                    }

                    if(degreeHead[d + 1] != NULL) degreeHead[d + 1]->bf = pool + anotherNode;
                    pool[anotherNode].nxt = degreeHead[d + 1];
                    pool[anotherNode].bf = NULL;
                    degreeHead[d + 1] = pool + anotherNode;

                }
            k -= i;
            is[no] = 1;
            ++res;
        }
    }

    delete[] degreeHead;
    delete[] pool;
    return res;
}