//
// Created by Kelukin on 2020/3/16.
//
#include "Utility.h"
int INDEX_TYPE = 0;
int LAST_DELETE_TYPE = 1;
int THRESHOLD = -1;
int TIME_THRESHOLD = 100;
bool LOCALSEARCH = true;
FILE *open_file(const char *filename, const char *mode){
    FILE *f = fopen(filename, mode);
    if(f == NULL){
        printf("Cannot open file %s!\n", filename);
        exit(1);
    }

    return f;
}
void putNodeList(NodeList* node, NodeList** list, int targetNo){
    if(list[targetNo] != NULL)  list[targetNo]->bf = node;
    node->bf = NULL;
    node->nxt = list[targetNo];
    node->degree = targetNo;
    list[targetNo] = node;
}

void rmNodeList(NodeList* node, NodeList** list, int targetNo){
    if(list[targetNo] == node) list[targetNo] = list[targetNo]->nxt;
    if(node->bf != NULL) node->bf->nxt = node->nxt;
    if(node->nxt != NULL) node->nxt->bf = node->bf;
    node->bf = node->nxt = NULL;
}

void put_out_S(int nodeNo, char *is, ui *pstart, ui *edges, NodeList* pool,
               int &lb_out, NodeList** inDegreeHead, NodeList** outDegreeHead){
    rmNodeList(pool + nodeNo, inDegreeHead, pool[nodeNo].degree);
    putNodeList(pool + nodeNo, outDegreeHead, pool[nodeNo].degree);
    for(ui i = pstart[nodeNo]; i < pstart[nodeNo + 1]; ++i){
        int x = edges[i];
        if(is[x] == 1){
            int no = pool[x].degree;
            rmNodeList(pool + x, inDegreeHead, no);
            putNodeList(pool + x, inDegreeHead, no - 1);
        }else if(is[x] == 0){
            int no = pool[x].degree;
            rmNodeList(pool + x, outDegreeHead, no);
            putNodeList(pool + x, outDegreeHead, no - 1);
            if(no - 1 < lb_out) lb_out = no - 1;
        }
    }
    is[nodeNo] = 0;
}
void put_in_S(int nodeNo, char *is, ui *pstart, ui *edges, NodeList* pool,
              int &ub_in, NodeList** inDegreeHead, NodeList** outDegreeHead){
    is[nodeNo] = 1;

    rmNodeList(pool + nodeNo, outDegreeHead, pool[nodeNo].degree);
    putNodeList(pool + nodeNo, inDegreeHead, pool[nodeNo].degree);
    for(ui i = pstart[nodeNo]; i < pstart[nodeNo + 1]; ++i){
        int x = edges[i];
        if(is[x] == 1){
            int no = pool[x].degree;
            rmNodeList(pool + x, inDegreeHead, no);
            putNodeList(pool + x, inDegreeHead, no + 1);
            if(no + 1 > ub_in) ub_in = no + 1;
        }else if(is[x] == 0){
            int no = pool[x].degree;
            rmNodeList(pool + x, outDegreeHead, no);
            putNodeList(pool + x, outDegreeHead, no + 1);
        }
    }
}