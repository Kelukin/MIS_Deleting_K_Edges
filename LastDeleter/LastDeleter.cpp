//
// Created by Kelukin on 2020/4/8.
//

#include "LastDeleter.h"
#include "../Utility.h"
#include <unordered_set>
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


LastDeleter* LastDeleter::lastDeleter = nullptr;

int LastDeleter::smart_delete_edges(char *is, ui *pstart, ui *edges, ui k, ui n, ui *given_set, ui gs_length) {
    std::unordered_set<int> hash;
    int res = 0;
    NodeList *pool = new NodeList[n];
    NodeList **inDegreeHead = new NodeList* [n];
    NodeList **outDegreeHead = new NodeList* [n];
    for(ui i = 0; i < n; ++i){
        inDegreeHead[i] = NULL;
        outDegreeHead[i] = NULL;
        pool[i].no = i;
    }

    for(ui i = 0; i < gs_length; ++i)
        is[given_set[i]] = 2;

    int lb_out = n, ub_in = 0;

    for(ui i = 0; i < n; ++i)
        if(is[i] != 2){
            int degreeCnt = 0;
            for(ui j = pstart[i]; j < pstart[i + 1]; ++j)
                if(is[edges[j]] != 0) ++degreeCnt;
            NodeList **degreeHead;
            if(is[i] == 0){
                assert(degreeCnt != 0);
                lb_out = std::min(lb_out, degreeCnt);
                degreeHead = outDegreeHead;
            }else{
                ub_in = std::max(ub_in, degreeCnt);
                degreeHead = inDegreeHead;
            }

            putNodeList(pool + i, degreeHead, degreeCnt);
        }
    //finish the procession
    int tmp_cnt = 0;
    while(ub_in > lb_out || lb_out <=k){
        while(outDegreeHead[lb_out] == NULL && lb_out < n) ++lb_out;
        while(inDegreeHead[ub_in] == NULL && ub_in > 0) ub_in--;
        if(lb_out == n) break;


        if(lb_out == 1 && (++tmp_cnt)%100 == 0){
            NodeList* node = inDegreeHead[1];
            while(node != NULL){
                NodeList* tmpNxt = node->nxt;
                int no = node->no;
                int num = 0;
                hash.clear();
                for(ui i = pstart[no]; i < pstart[no + 1]; ++i){
                    int x = edges[i];
                    if(is[x] == 0 && pool[x].degree == 1){
                        hash.insert(x);
                        ++num;
                    }
                }
                if(num >= 2){
                    for(ui i = pstart[no]; i < pstart[no + 1]; ++i){
                        int x = edges[i];
                        if(is[x] == 0 && pool[x].degree == 1){
                            int tmp_num = num;
                            for(ui j = pstart[x]; j < pstart[x + 1]; ++j)
                                if(hash.find(edges[j]) != hash.end())   --tmp_num;

                            if(tmp_num != 1){
                                put_out_S(no, is, pstart, edges, pool,
                                        lb_out, inDegreeHead, outDegreeHead);
                                put_in_S(x, is, pstart, edges, pool,
                                        ub_in, inDegreeHead, outDegreeHead);
                                ++k;
                                assert(lb_out == 0);
                                while(outDegreeHead[0] != NULL){
                                    NodeList* node = outDegreeHead[0];

                                    put_in_S(node->no, is, pstart, edges, pool
                                            , ub_in, inDegreeHead, outDegreeHead);

                                }
                                break;
                            }
                        }
                    }
                }
                node = tmpNxt;
            }
//            continue;
            node = inDegreeHead[0];
            while(node != NULL){
                NodeList* tmpNxt = node->nxt;
                int no = node->no;
                int num = 0;
                hash.clear();
                for(ui i = pstart[no]; i < pstart[no + 1]; ++i){
                    int x = edges[i];
                    if(is[x] == 0 && pool[x].degree == 1){
                        hash.insert(x);
                        ++num;
                    }
                }
                if(num >= 2){
                    for(ui i = pstart[no]; i < pstart[no + 1]; ++i){
                        int x = edges[i];
                        if(is[x] == 0 && pool[x].degree == 1){
                            int tmp_num = num;
                            for(ui j = pstart[x]; j < pstart[x + 1]; ++j)
                                if(hash.find(edges[j]) != hash.end())   --tmp_num;

                            if(tmp_num != 1){
                                put_out_S(no, is, pstart, edges, pool,
                                          lb_out, inDegreeHead, outDegreeHead);
                                put_in_S(x, is, pstart, edges, pool,
                                         ub_in, inDegreeHead, outDegreeHead);
                                assert(lb_out == 0);
                                while(outDegreeHead[0] != NULL){
                                    NodeList* node = outDegreeHead[0];

                                    put_in_S(node->no, is, pstart, edges, pool
                                            , ub_in, inDegreeHead, outDegreeHead);

                                }
                                break;
                            }
                        }
                    }
                }
                node = tmpNxt;
            }
            continue;
        }

        if(lb_out == 0){
            while(outDegreeHead[0] != NULL){
                NodeList* node = outDegreeHead[0];

                put_in_S(node->no, is, pstart, edges, pool
                        , ub_in, inDegreeHead, outDegreeHead);

            }
            continue;
        }

        if(ub_in > lb_out){
            NodeList* a = inDegreeHead[ub_in];
//            NodeList* b = outDegreeHead[lb_out];
//            k = k +

            int nodeNo = a->no;
            k += a->degree;
            put_out_S(nodeNo, is, pstart, edges, pool,
                    lb_out, inDegreeHead, outDegreeHead);

            NodeList* b = outDegreeHead[lb_out];
            k-= lb_out;
            nodeNo = b->no;
            put_in_S(nodeNo, is, pstart, edges, pool,
                     ub_in, inDegreeHead, outDegreeHead);

        }else{
            if(lb_out > k) continue;
            NodeList* a = outDegreeHead[lb_out];
            k -= lb_out;
            int nodeNo = a->no;
            put_in_S(nodeNo, is, pstart, edges, pool,
                     ub_in, inDegreeHead, outDegreeHead);

            ++res;
        }
    }
    for(ui i = 0; i < gs_length; ++i)
        is[given_set[i]] = 1;

    delete[] pool;
    delete[] inDegreeHead;
    delete[] outDegreeHead;
    return res;
}