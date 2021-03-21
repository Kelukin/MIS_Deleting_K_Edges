//
// Created by Kelukin on 2020/3/16.
//

#ifndef MIS_DELETING_K_EDGES_UTILITY_H
#define MIS_DELETING_K_EDGES_UTILITY_H

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cstdio>
#include <string>
#include <cassert>
#include <queue>
#include <iostream>
#include <unordered_set>
#include <bitset>
typedef unsigned int ui;
struct NodeList{
    NodeList* nxt;
    NodeList* bf;
    int no;
    int degree;
};
void putNodeList(NodeList* node, NodeList** list, int targetNo);
void rmNodeList(NodeList* node, NodeList** list, int targetNo);
void put_out_S(int nodeNo, char *is, ui *pstart, ui *edges, NodeList* pool,
               int &lb_out, NodeList** inDegreeHead, NodeList** outDegreeHead);

void put_in_S(int nodeNo, char *is, ui *pstart, ui *edges, NodeList* pool,
              int &ub_in, NodeList** inDegreeHead, NodeList** outDegreeHead);
std::vector<double> getLogProbability(int vertexNum);
#define __LINUX__
#ifdef __LINUX__
#include <sys/time.h>
#endif
//#define CHECK_BUG
#define NDEBUG
FILE *open_file(const char *file_name, const char *mode);

#endif //MIS_DELETING_K_EDGES_UTILITY_H
