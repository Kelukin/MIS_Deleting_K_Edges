//
// Created by Kelukin on 2020/3/16.
//

#include "Graph.h"

Graph::Graph(const char *_dir) {
    dir = std::string(dir);
    n = m = 0;

    pstart = NULL;
    edges = NULL;
}

Graph::~Graph() {
    if(pstart != NULL){
        delete[] pstart;
        pstart = NULL;
    }

    if(edges != NULL){
        delete[] edges;
        edges = NULL;
    }
}

void Graph::read_graph() {
    FILE *f = open_file((dir + std::string("/b_degree.bin")).c_str(), "rb");

    int tt;
    fread(&tt, sizeof(int), 1, f);
    if(tt != (int) sizeof(int)){
        printf("sizeof int is different: edge.bin(%d), machine(%d)\n", tt, (int)sizeof(int));
        return;
    }

    fread(&n, sizeof(int), 1, f);
    fread(&m, sizeof(int), 1, f);

    ui *degree = new ui[n];
    fread(degree, sizeof(int), n, f);

    fclose(f);

    f = open_file((dir  + std::string("/b_adj.bin")).c_str(), "rb");
    if(pstart == NULL)  pstart = new ui[n+1];
    if(edges == NULL)   edges = new ui[m];

    ui *buf = new ui[n];

    pstart[0] = 0;
    for(ui i = 0; i < n; i++){
        if(degree[i] > 0) fread(buf, sizeof(int), degree[i], f);

        for(ui j = 0; j < degree[i]; j++){
            edges[pstart[i] + j] = buf[j];

            if(buf[j] == i) printf("Self-loop\n");
        }

        pstart[i + 1] = pstart[i] + degree[i];
    }
    fclose(f);

    f = open_file((dir + std::string("/b_given_set.bin")).c_str(), "rb");
    fread(&gs_length, sizeof(int), 1, f);
    given_set = new ui[gs_length];
    fread(given_set, sizeof(int), gs_length, f);


    delete[] buf;

    delete[] degree;
}

void Graph::check_is(const char *is, int count, int k) {
    int cnt = 0;

    for(ui i = 0; i < n; i++)   if(is[i])   cnt++;
    if(count != cnt)    printf("WA count in check_is! %d\n", cnt);

    cnt = 0;//conflict count

    bool maximal = true;
    for(ui i = 0; i < n; i++){
        if(is[i]){
            for(int j = pstart[i]; j < pstart[i + 1]; j++)
                if(is[edges[j]])    ++cnt;
        }else if(maximal){
            bool find = false;
            for(int j = pstart[i]; j < pstart[i + 1] && !find; j++)
                if(is[edges[j]])    find = true;
            if(!find)   maximal = false;
        }
    }
    if(!maximal)    printf("WA!!!!!!!!! Not Maximal!");
    assert((cnt & 1) == 0);
    cnt >>= 1;
    if(cnt > k) printf("WA!!!!!!! Delete more than k edges!!!");
}
void Graph::construct_degree_increase(ui *ids) {
    memset(ids, 0, sizeof(ui)*n);

    for(ui i = 0; i < n; i++){
        ui d = pstart[i + 1] - pstart[i];
        ++ids[d];
    }

    for(ui i = 1; i < n; i++)   ids[i] += ids[i-1];

    ui *order = new ui[n];
    for(ui i = 0; i < n; i++){
        ui d = pstart[i + 1] - pstart[i];
        order[i] = ids[d];
        --ids[d];
    }
    for(ui i = 0; i < n; i++)   ids[order[i] - 1] = i;

    delete[] order;
}

void Graph::greedy_delete_edges(char *is, ui k) {
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
        while(degreeHead[i] != NULL){
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

        }
    }

    delete[] degreeHead;
    delete[] pool;
}

void Graph::greedy(ui k) {
#ifdef __LINUX__
    struct timeval start, endl, end;
    gettimeofday(&start, NULL);
#endif

    char *is = new char[n];
    for(ui i = 0; i< n; i++)    is[i] = 1;

    int cnt = 0;

    for(ui i = 0; i < gs_length; i++)
        is[given_set[i]] = 2;

    for(ui i = 0; i< gs_length; i++){
        int x = given_set[i];
        for(ui j = pstart[i]; j < pstart[i + 1]; j++){
            if(is[edges[j]] == 2)   cnt++;
            else    is[edges[j]] = 0;
        }
    }
    int origin_k;
    cnt /= 2;
    if(cnt > k){
        delete[] is;
        printf("The deleting number should be greater than %d!", cnt);
        return;
    }else   k -= cnt;

    ui *ids = new ui[n];
    construct_degree_increase(ids);

    int res = 0;
    for(ui i = 0; i < n; i++)  if(is[ids[i]] == 1){
        ui u = ids[i];
        ++ res;
        for(ui j = pstart[u]; j < pstart[u + 1]; j ++)  is[edges[j]] = 0;
    }

    printf("Greedy MIS: %d\n", res);
    delete[] ids;
#ifdef __LINUX__
    gettimeofday(&endl, NULL);
    long long mtime1, seconds1, useconds1;
    seconds1 = end.tv_sec - start.tv_sec;
    usecond1 = end.tv_sec - start.tv_usec;
    mtime1 = seconds1 * 1000000 + useconds1;
#endif
    greedy_delete_edges(is, k);
#ifdef CHECK_BUG
    check_is(is, res, origin_k);
#endif CHECK_BUG

    delete[] is;

#ifdef __LINUX__
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconfs = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds * 1000000 + useconds;

    printf("Process time: %lld, Swap time: %lld, Total time: %lld\n", mtime1, mtime - mtime1, mtime);
#endif

}