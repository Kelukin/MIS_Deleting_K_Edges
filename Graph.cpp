//
// Created by Kelukin on 2020/3/16.
//

#include "Graph.h"
#include "LocalSearch/BasicVersion.h"
Graph::Graph(const char *_dir) {
    dir = std::string(_dir);
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
void Graph::delete_vertex(ui v, char *is, int *degree) {
    is[v] = 0;
    for(ui k = pstart[v]; k < pstart[v + 1]; ++k)
        if(is[edges[k]]){
            int w = edges[k];
            --degree[w];
        }
}
int Graph::delete_vertex(ui v, char *is, int *degree, std::vector<ui> &degree_ones, std::vector<ui> &degree_twos) {
    is[v] = 0;
    int res = 0;
    for(ui k = pstart[v];k < pstart[v+1];k ++) if(is[edges[k]]) {
            int w = edges[k];
            -- degree[w];
            if(degree[w] == 0) ++ res;
            else if(degree[w] == 1) degree_ones.push_back(w);
            else if(degree[w] == 2) degree_twos.push_back(w);
        }
    return res;
}

int Graph::delete_vertex(ui u, ui *pend, char *is, std::vector<ui> &degree_twos, ui *tri, char *adj, int *degree, char *dominate, std::vector<ui> &dominated) {
    is[u] = 0;
    int res = 0;
    shrink(u, pend[u], is, tri);
    for(ui i = pstart[u];i < pend[u];i ++) {
        ui v = edges[i];
        adj[v] = 1;
        -- degree[v];

        if(degree[v] == 0) ++ res;
        else if(degree[v] == 2) degree_twos.push_back(v);
    }

    for(ui i = pstart[u];i < pend[u];i ++) {
        ui v = edges[i];

        for(ui j = pstart[v];j < pend[v];j ++) if(is[edges[j]]) {
                ui w = edges[j];

#ifndef NDEBUG
                if(adj[w]&&tri[j] == 0) printf("WA delete_vertex!\n");
#endif
                if(adj[w]) -- tri[j];

                if(tri[j]+1 == degree[v]&&!dominate[w]) {
                    dominate[w] = 1;
                    dominated.push_back(w);
                }
            }
    }

    for(ui i = pstart[u];i < pend[u];i ++) adj[edges[i]] = 0;

    return res;
}

int Graph::init_given_set(char *is, int *degree) {
    // set the status of vertices in given set and their neighborhood's  to 0
    // and return the edges need to be deleted from the graph
    // in addition, it will judge whether the `money` afford the cost
    ui cost = 0;
    for(ui i = 0 ; i < gs_length; ++i){
        int x = given_set[i];
        is[x] = 2;
    }

    for(ui i = 0; i < gs_length; ++i){
        int x = given_set[i];
        for(ui k = pstart[x]; k < pstart[x + 1]; ++k)
            if(is[edges[k]] == 2)   ++cost;
            else if(is[edges[k]] == 1){
                delete_vertex(edges[k], is, degree);
            }
    }
    cost >>= 1;

    for(ui i = 0; i < gs_length; ++i){
        int x = given_set[i];
        delete_vertex(x, is, degree);
    }

    return cost;
}
extern bool Train_Flag;
extern int QUERY_ID;
void Graph::read_graph() {
    std::cout<< dir << std::endl;
    FILE *f = open_file((dir + std::string("_degree.bin")).c_str(), "rb");

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

    f = open_file((dir  + std::string("_adj.bin")).c_str(), "rb");
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
    if(Train_Flag){
        gs_length = 0;
        printf("Open the train mode.\n");
    }
    else{
        std::string id_str = (QUERY_ID == -1)? "" : std::to_string(QUERY_ID);
        f = open_file((dir + std::string("_given_set") + id_str
                + std::string(".bin")).c_str(), "rb");
        if(f != NULL){
            fread(&gs_length, sizeof(int), 1, f);
            given_set = new ui[gs_length];
            fread(given_set, sizeof(int), gs_length, f);
        } else{
            gs_length = 0;
        }
    }

    delete[] buf;

    delete[] degree;
}
void Graph::get_two_neighbors(ui u, char *is, ui &u1, ui &u2) {
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(is[edges[i]]) {
        if(u1 == n) {
            u1 = edges[i];
            std::swap(edges[pstart[u]], edges[i]);
        }
        else {
            assert(u2 == n);
            u2 = edges[i];
            std::swap(edges[pstart[u]+1], edges[i]);
            break;
        }
    }
}
ui Graph::get_other_neighbor(ui u, char *is, ui u1) {
    ui idx = 0;
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(is[edges[i]]) {
            std::swap(edges[pstart[u]+idx], edges[i]);
            if((++ idx) == 2) break;
        }
    assert(edges[pstart[u]] == u1||edges[pstart[u]+1] == u1);
    if(edges[pstart[u]] == u1) return edges[pstart[u]+1];
    return edges[pstart[u]];
}

ui Graph::edge_rewire(ui u, const ui *pend, ui v, ui w) {
#ifndef NDEBUG
    for(ui i = pstart[u];i < pend[u];i ++) if(edges[i] == w) printf("WA preexist edge!\n");
#endif

    for(ui i = pstart[u];i < pend[u];i ++) if(edges[i] == v) {
            edges[i] = w;
            return i;
        }
    printf("WA in edge_rewire!\n");
    return 0;
}

int Graph::exist_edge(ui u, ui v, const ui *pend) {
    if(pend[u]-pstart[u] < pend[v]-pstart[v]) {
        for(ui i = pstart[u];i < pend[u];i ++) {
            if(edges[i] == v) return 1;
        }
        return 0;
    }
    for(ui i = pstart[v];i < pend[v];i ++) {
        if(edges[i] == u) return 1;
    }
    return 0;
}

int Graph::exist_edge(ui u1, ui u2) {
    if(pstart[u1+1]-pstart[u1] < pstart[u2+1]-pstart[u2]) {
        for(ui i = pstart[u1];i < pstart[u1+1];i ++) if(edges[i] == u2) return 1;
        return 0;
    }
    for(ui i = pstart[u2];i < pstart[u2+1];i ++) if(edges[i] == u1) return 1;
    return 0;
}

void Graph::edge_rewire(ui u, ui u1, ui u2) {
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(edges[i] == u1) {
        edges[i] = u2;
        break;
    }
}

int Graph::remove_degree_one_two(std::vector<ui> &degree_ones, std::vector<ui> &degree_twos, char *is, int *degree,
                                 std::vector<std::pair<ui, ui> > &S) {
    int res = 0;

    while(!degree_ones.empty()||!degree_twos.empty()) {
        while(!degree_ones.empty()) {
            ui u = degree_ones.back();
            degree_ones.pop_back();
            if(!is[u]||degree[u] != 1) continue;

            for(int j = pstart[u];j < pstart[u+1];j ++) if(is[edges[j]]) {
                    res += delete_vertex(edges[j], is, degree, degree_ones, degree_twos);
                }
        }

        while(!degree_twos.empty()&&degree_ones.empty()) {
            ui u = degree_twos.back();
            degree_twos.pop_back();
            if(!is[u]||degree[u] != 2) continue;

            ui u1 = n, u2 = n;
            get_two_neighbors(u, is, u1, u2);
            assert(u1 != n&&u2 != n);

            ui pre = u, cnt = 1;
            while(u1 != u&&degree[u1] == 2) {
                ++ cnt;
                ui tmp = get_other_neighbor(u1, is, pre);
                pre = u1;
                u1 = tmp;
            }
            if(u1 == u) {// then it is a circle
                res += delete_vertex(u, is, degree, degree_ones, degree_twos);
                assert(!degree_ones.empty());
                continue;
            }

            pre = u;
            while(degree[u2] == 2) {
                ++ cnt;
                ui tmp = get_other_neighbor(u2, is, pre);
                pre = u2;
                u2 = tmp;
            }
            if(u1 == u2) {//one closed circle, with one vertex having the degree larger than three
                res += delete_vertex(u1, is, degree, degree_ones, degree_twos);
                assert(!degree_ones.empty());
                continue;
            }

            if(cnt%2 == 1) {
                if(exist_edge(u1, u2)) {
                    res += delete_vertex(u1, is, degree, degree_ones, degree_twos);
                    res += delete_vertex(u2, is, degree, degree_ones, degree_twos);
                    assert(!degree_ones.empty());
                }
                else if(cnt > 1) {
                    u = get_other_neighbor(pre, is, u2);
                    edge_rewire(pre, u, u1);

                    u2 = pre;
                    while(u != u1) {
                        is[u] = 0;
                        ui tmp = get_other_neighbor(u, is, pre);
                        S.push_back(std::make_pair(u, tmp));
                        pre = u;
                        u = tmp;
                    }

                    edge_rewire(u1, pre, u2);
                }
            }
            else {
                ui v2 = pre, v1 = pre;
                pre = u2;
                while(v1 != u1) {
                    is[v1] = 0;
                    ui tmp = get_other_neighbor(v1, is, pre);
                    S.push_back(std::make_pair(v1,tmp));
                    pre = v1;
                    v1 = tmp;
                }
                v1 = pre;
                if(exist_edge(u1, u2)) {
                    if((-- degree[u1]) == 2) degree_twos.push_back(u1);
                    if((-- degree[u2]) == 2) degree_twos.push_back(u2);

                    assert(degree[u1] > 1&&degree[u2] > 1);
                }
                else {
                    edge_rewire(u1, v1, u2);
                    edge_rewire(u2, v2, u1);
                }
            }
        }
    }

    return res;
}

int Graph::initial_dominance_and_degree_two_remove(std::vector<ui> &degree_ones, std::vector<ui> &degree_twos,
                                                    char *is, int *degree, char *adj,
                                                    std::vector<std::pair<ui, ui> > &S) {
    int res = 0;
    res += remove_degree_one_two(degree_ones, degree_twos, is, degree, S);

    // sort vertices in degree increasing order
    ui *ids = new ui[n];
    memset(ids, 0, sizeof(ui)*n);
    for(ui i = 0;i < n;i ++) ++ ids[degree[i]];
    for(ui i = 1;i < n;i ++) ids[i] += ids[i-1];

    ui *order = new ui[n];
    for(ui i = 0;i < n;i ++) {
        order[i] = ids[degree[i]];
        -- ids[degree[i]];
    }
    for(ui i = 0;i < n;i ++) ids[order[i]-1] = i;

    // compute dominance for vertices in degree decreasing order
    for(int i = n-1;i >= 0;i --) {
        int u = ids[i];
        if(!is[u]||degree[u] <= 0) continue;

        for(ui j = pstart[u];j < pstart[u+1];j ++) if(is[edges[j]]) adj[edges[j]] = 1;
        adj[u] = 1;// note the neighbor edges

        int dominate = 0;
        for(ui j = pstart[u];j < pstart[u+1];j ++) if(is[edges[j]]&&degree[edges[j]] <= degree[u]) {
                int tri_cnt = 0;
                ui v = edges[j];
                for(ui k = pstart[v];k < pstart[v+1];k ++) if(is[edges[k]]) {
                        if(!adj[edges[k]]) break;//then it is impossible to dominate
                        ++ tri_cnt;
                    }
                if(tri_cnt == degree[v]) {
                    dominate = 1; // find one neighbor vertex of `u` dominates it.
                    break;
                }
            }

        for(ui j = pstart[u];j < pstart[u+1];j ++) adj[edges[j]] = 0;
        adj[u] = 0;

        if(dominate) {
            res += delete_vertex(u, is, degree, degree_ones, degree_twos);
            res += remove_degree_one_two(degree_ones, degree_twos, is, degree, S);
        }
    }

    delete[] ids;
    delete[] order;

    return res;
}
void check_is(const char *is, int count, ui n, int k, ui *pstart, ui *edges){
    int cnt = 0;

//    for(ui i = 0; i < n; i++)   if(is[i])   cnt++;
//    if(count != cnt)    printf("WA count in check_is! %d\n", cnt);

    cnt = 0;//conflict count

    bool maximal = true;
    for(ui i = 0; i < n; i++){
        if(is[i]){
            for(ui j = pstart[i]; j < pstart[i + 1]; j++)
                if(is[edges[j]] > 0)    ++cnt;
        }else if(maximal){
            bool find = false;
            for(ui j = pstart[i]; j < pstart[i + 1] && !find; j++)
                if(is[edges[j]])    find = true;
            if(!find)   maximal = false;
        }
    }
    if(!maximal)    printf("WA!!!!!!!!! Not Maximal!");
    assert((cnt & 1) == 0);
    cnt >>= 1;
    if(cnt > k) printf("WA!!!!!!! Delete more than k edges!!! %d", cnt);
//    printf("residual k: %d, assumed %d\n", k - cnt, count);
    assert(count == k - cnt);
    assert(cnt <=k);
}


void Graph::check_is(const char *is, int count, int k) {
    int cnt = 0;

    for(ui i = 0; i < n; i++)   if(is[i])   cnt++;
    if(count != cnt)    printf("WA count in check_is! %d\n", cnt);

    cnt = 0;//conflict count

    bool maximal = true;
    for(ui i = 0; i < n; i++){
        if(is[i]){
            for(ui j = pstart[i]; j < pstart[i + 1]; j++)
                if(is[edges[j]])    ++cnt;
        }else if(maximal){
            bool find = false;
            for(ui j = pstart[i]; j < pstart[i + 1] && !find; j++)
                if(is[edges[j]])    find = true;
            if(!find)   maximal = false;
        }
    }
    if(!maximal)    printf("WA!!!!!!!!! Not Maximal!");
    assert((cnt & 1) == 0);
    cnt >>= 1;
    if(cnt > k) printf("WA!!!!!!! Delete more than k edges!!!");
    assert(cnt <=k);
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

int Graph::greedy_delete_edges(char *is, ui k) {
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

int Graph::lp_reduction(ui *ids, ui ids_n, char *is, int *degree) {
    //printf("begin lp_reduction\n");
    int *new_id = new int[n];
    memset(new_id, -1, sizeof(int)*n);
    for(ui i = 0;i < ids_n;i ++) new_id[ids[i]] = i;
    for(ui i = 0;i < ids_n;i ++) {
        ui u = ids[i];
        for(ui j = pstart[u];j < pstart[u+1];j ++) edges[j] = new_id[edges[j]];
    }
    delete[] new_id;
    new_id = NULL;

    ui *new_pstart = new ui[ids_n+1];
    for(ui i = 0;i < ids_n;i ++) new_pstart[i] = pstart[ids[i]];
    new_pstart[ids_n] = pstart[ids[ids_n-1]+1];

    int res = 0;
    int *x = new int[ids_n];
    int *y = new int[ids_n];
    memset(x, -1, sizeof(int)*ids_n);
    memset(y, -1, sizeof(int)*ids_n);

    char *used = new char[2*ids_n];
    int *level = new int[2*ids_n];
    ui *que = new ui[2*ids_n];
    ui *iter = new ui[ids_n];

    while(true) {
        memset(used, 0, sizeof(char)*2*ids_n);
        ui que_n = 0;
        for(ui u = 0;u < ids_n;u ++) if(x[u] == -1){
                level[u] = 0;
                used[u] = 1;
                que[que_n++] = u;
            }
        int find = 0;
        for(ui i = 0;i < que_n;i ++) {
            ui u = que[i];
            iter[u] = new_pstart[u+1];
            for(ui j = new_pstart[u];j < new_pstart[u+1];j ++) if(!used[ids_n+edges[j]]) {
                    used[ids_n+edges[j]] = 1;
                    int v = y[edges[j]];
                    if(v == -1) find = 1;
                    else {
                        assert(!used[v]);
                        used[v] = 1;
                        level[v] = level[u]+1;
                        que[que_n++] = v;
                    }
                }
        }
        if(!find) break;

        for(ui i = 0;i < ids_n;i ++) if(x[i] == -1) {
                que_n = 0;
                que[que_n++] = i;
                while(que_n > 0) {
                    ui u = que[que_n-1];
                    if(iter[u] <= new_pstart[u]) {
                        -- que_n;
                        if(que_n > 0) -- iter[que[que_n-1]];
                    }
                    else {
                        ui v = edges[iter[u]-1];
                        if(y[v] == -1) {
                            for(ui j = 0;j < que_n;j ++) {
                                u = que[j];
                                v = edges[iter[u]-1];
                                x[u] = v; y[v] = u;
                            }
                            que_n = 0;
                        }
                        else if(level[y[v]] > level[u]) que[que_n++] = y[v];
                        else -- iter[u];
                    }
                }
            }
    }

    for(ui u = 0;u < ids_n;u ++) if(!used[u]&&used[ids_n+u]) {
            is[ids[u]] = 0;
            for(ui j = new_pstart[u];j < new_pstart[u+1];j ++) if(is[ids[edges[j]]]) {
                    if( (-- degree[ids[edges[j]]]) == 0) ++ res;
                }
        }

#ifndef NDEBUG
    for(ui i = 0;i < ids_n;i ++) {
        ui u = ids[i];
        if(used[i]&&!used[ids_n+i]&&(!is[u]||degree[u] > 0)) printf("WA\n");
    }

    for(ui i = 0;i < ids_n;i ++) if((used[i]&&used[i+ids_n])||(!used[i]&&!used[i+ids_n])) {
            if(x[i] == -1||y[i] == -1) printf("WA half-integral solution! x[i]: %d, y[i]: %d\n", x[i], y[i]);
        }
#endif

    memset(used, 0, sizeof(char)*2*ids_n);
    for(ui i = 0;i < ids_n;i ++) iter[i] = new_pstart[i+1];

    ui level_n = 0;
    for(ui u = 0;u < ids_n;u ++) if(!used[u]&&is[ids[u]]&&degree[ids[u]] > 0) {
            ui que_n = 0;
            que[que_n ++] = u;
            used[u] = 1;
            while(que_n > 0) {
                u = que[que_n-1];
                if(u < ids_n) {
                    assert(x[u] != -1&&is[ids[x[u]]]&&degree[ids[x[u]]] > 0);
                    //assert(x[u] != -1); assert(is[ids[x[u]]]); assert(degree[ids[x[u]]] > 0);
                    if(!used[x[u]+ids_n]) {
                        used[x[u]+ids_n] = 1;
                        que[que_n ++] = x[u]+ids_n;
                    }
                    else {
                        level[level_n++] = u;
                        -- que_n;
                    }
                }
                else {
                    u -= ids_n;
                    ui v = -1;
                    while(iter[u] > new_pstart[u]) {
                        v = edges[iter[u]-1];
                        if(!used[v]&&is[ids[v]]) {
                            assert(degree[ids[v]] > 0);
                            break;
                        }
                        -- iter[u];
                    }
                    if(iter[u] <= new_pstart[u]) -- que_n;
                    else {
                        used[v] = 1;
                        que[que_n++] = v;
                    }
                }
            }
        }

    memset(used, 0, sizeof(char)*2*ids_n);
    char *in = new char[ids_n];
    memset(in, 0, sizeof(char)*ids_n);

    for(int i = level_n - 1;i >= 0;i --) {
        ui u = level[i];
        if(used[u]||!is[ids[u]]||degree[ids[u]] == 0) continue;

        ui que_n = 0;
        que[que_n ++] = u;
        used[u] = 1;
        int ok = 1;
        for(ui j = 0;j < que_n;j ++) {
            u = que[j];
            if(ok&&used[u < ids_n? (u+ids_n) : (u-ids_n)]) ok = 0;
            if(u < ids_n) {
                for(ui k = new_pstart[u];k < new_pstart[u+1];k ++) {
                    ui v = edges[k];
                    if(!is[ids[v]]||degree[ids[v]] <= 0) continue;

                    v += ids_n;
                    if(!used[v]) {
                        in[v-ids_n] = 1;
                        used[v] = 1;
                        que[que_n++] = v;
                    }
                    else if(!in[v-ids_n]) ok = 0;
                }
            }
            else {
                u -= ids_n;
                assert(y[u] != -1&&is[ids[y[u]]]&&degree[ids[y[u]]] > 0);
                if(!used[y[u]]) {
                    used[y[u]] = 1;
                    que[que_n++] = y[u];
                }
            }
        }

        for(ui j = 0;j < que_n;j ++) if(que[j] >= ids_n) in[que[j]-ids_n] = 0;

        if(ok) {
            for(ui j = 0;j < que_n;j ++) {
                u = que[j];
                if(u >= ids_n) {
                    u -= ids_n;
                    is[ids[u]] = 0;
                    for(ui k = new_pstart[u];k < new_pstart[u+1];k ++) if(is[ids[edges[k]]]) {
                            if( (-- degree[ids[edges[k]]]) == 0) ++ res;
                        }
                }
            }
        }
    }

    delete[] new_pstart;
    delete[] iter;
    delete[] level;
    delete[] que;
    delete[] used;
    delete[] x;
    delete[] y;
    delete[] in;

    for(ui i = 0;i < ids_n;i ++) {
        ui u = ids[i];
        for(ui j = pstart[u];j < pstart[u+1];j ++) edges[j] = ids[edges[j]];
    }
    //printf("finish lp reduction!\n");

    return res;
}


int Graph::dominated_check(ui u, ui *pend, char *is, ui *tri, int *degree) { // check whether this vertex could be reduced
    for(ui i = pstart[u];i < pend[u];i ++) if(is[edges[i]]&&tri[i]+1 == degree[edges[i]]) return 1;
    return 0;
}


void Graph::greedy(ui k) {
#ifdef __LINUX__
    struct timeval start, endl, end;
    gettimeofday(&start, NULL);
#endif

    char *is = new char[n];
    for(ui i = 0; i< n; i++)    is[i] = 1;

    ui cnt = 0;

    for(ui i = 0; i < gs_length; i++){
        is[given_set[i]] = 2;
    }

    for(ui i = 0; i< gs_length; i++){
        int x = given_set[i];
        for(ui j = pstart[x]; j < pstart[x + 1]; j++){
            if(is[edges[j]] == 2)   cnt++;
            else    is[edges[j]] = 0;
        }
    }
    ui origin_k = k;
    cnt /= 2;
    if(cnt > k){
        delete[] is;
        printf("The deleting number should be greater than %d!", cnt);
        return;
    }else   k -= cnt;

    ui *ids = new ui[n];
    construct_degree_increase(ids);

    for(ui i = 0; i < n; i++)  if(is[ids[i]] == 1){
        ui u = ids[i];
        for(ui j = pstart[u]; j < pstart[u + 1]; j ++)  is[edges[j]] = 0;
    }
    delete[] ids;
#ifdef __LINUX__
    gettimeofday(&end, NULL);
    long long mtime1, seconds1, useconds1;
    seconds1 = end.tv_sec - start.tv_sec;
    useconds1 = end.tv_usec - start.tv_usec;
    mtime1 = seconds1 * 1000000 + useconds1;
#endif
    greedy_delete_edges(is, k);
    int res = 0;
    for(ui i = 0; i < n; i++)
        if(is[i]>0)   ++res;
    printf("Greedy MIS: %d\n", res);



#ifdef __LINUX__
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds * 1000000 + useconds;

    printf("Process time: %lld, Swap time: %lld, Total time: %lld\n", mtime1, mtime - mtime1, mtime);
#endif
    if(LOCALSEARCH){
        BasicVersion localSearch(n, m, given_set, gs_length, pstart, edges, origin_k, is);
        localSearch.iterate_local_search();
#ifdef CHECK_BUG
//        printf("Begin Check!");
        check_is(localSearch.getOptimum(),
                localSearch.getOptimumSize(), origin_k);
#endif
    }
    delete[] is;
}

int Graph::pay_and_try_framework(PayAndTry* payAndTry) {

    payAndTry->init();
    do{
//        payAndTry->debugCheck();
        // Firstly, use reduction rules to shrink the graphs size
        // Second to recycle the paid k
        // Record the vertices has been affected in last roll, and check its out edge's state, whether is has been
        // delete during the new reduction rules.
        while(payAndTry->canSafeReduction())
            payAndTry->safe_reduction();//appear bug after one time of reduction
        payAndTry->debugCheck();
        // If we still have the rest bullet for deleting edges
        if(payAndTry->isDone()) break;
        if(payAndTry->pay() != -1){
            continue;
            // just try to delete some edges
        }else{
            // Else, we have to try inexact strategy
            // And see the next roll activate the reductions in next iteration
            if(payAndTry->canRiskReduction())
                payAndTry->riskReduction();
            else payAndTry->inexact_delete();
        };

    }while(true);
    payAndTry->appending_action();
    //And greedy delete unrelated vertices
    payAndTry->last_delete();

    return payAndTry->getMISSize();
}

void Graph::shrink(ui u, ui &end, const char *is, ui *tri) {
    ui i = pstart[u];
    while(true) {  //  确保在前的都是尚未被删除的节点
        while(i < end&&is[edges[i]]) ++ i;
        while(i < end&&!is[edges[end-1]]) -- end;

        if(i >= end) break;
        std::swap(edges[i], edges[end-1]);
        std::swap(tri[i], tri[end-1]);//边的计数也随之交换
    }
}

int Graph::compute_triangle_counts(ui *tri, ui *pend, char *adj, char *is, int *degree, char *dominate,
                                   std::vector<ui> &dominated) {
    std::vector<ui> vs;
    for(ui i = 0;i < n;i ++) if(is[i]&&degree[i] > 0) vs.push_back(i);

    ui *ids = new ui[vs.size()];
    memset(ids, 0, sizeof(ui)*vs.size());
    for(ui i = 0;i < vs.size();i ++) ++ ids[degree[vs[i]]];
    for(ui i = 1;i < vs.size();i ++) ids[i] += ids[i-1];

    int *order = new int[n];
    memset(order, -1, sizeof(int)*n);
    for(ui i = 0;i < vs.size();i ++) order[vs[i]] = (-- ids[degree[vs[i]]]);

    for(ui i = 0;i < vs.size();i ++) ids[order[vs[i]]] = vs[i];//descending in vertex's degree

    int res = 0;
    for(int i = (int)vs.size()-1;i >= 0;i --) {
        ui u = ids[i];
        shrink(u, pend[u], is, tri);

        for(ui j = pstart[u];j < pend[u];j ++) adj[edges[j]] = 1;

        if(!dominate[u]) {
            adj[u] = 1;
            for(ui j = pstart[u];j < pend[u];j ++) if(degree[edges[j]] <= degree[u]) {
                    int tri_cnt = 0;
                    ui v = edges[j];
                    for(ui k = pstart[v];k < pend[v];k ++) if(is[edges[k]]) {
                            if(!adj[edges[k]]) break;
                            ++ tri_cnt;
                        }
                    if(tri_cnt == degree[v]) {
                        dominate[u] = 1;
                        //maybe we should add 'adj[u] = 0;' here;
                        break; // make adj[u] != 0
                    }
                }
            adj[u] = 0;
        }

        if(dominate[u]) {
            is[u] = 0;
            for(ui j = pstart[u];j < pend[u];j ++) if((-- degree[edges[j]]) == 0) ++ res;
            for(ui j = pstart[u];j < pend[u];j ++) if(order[edges[j]] > i) {// modify the vertex whose degree is large than current value
                    ui v = edges[j];
                    for(ui k = pstart[v];k < pend[v];k ++) if(is[edges[k]]) {
                            ui w = edges[k];
                            if(adj[w]) {
                                assert(tri[k] > 0);
                                -- tri[k];
                            }
                            if(!dominate[v]&&tri[k]+1 == degree[w]) {
                                dominate[v] = 1;
                                dominated.push_back(v);// waiting for further process/ Having not been deleted
                            }
                            if(!dominate[w]&&tri[k]+1 == degree[v]) {
                                dominate[w] = 1;
                                if(order[w] > i) dominated.push_back(w);
                            }
#ifndef NDEBUG
                            if(order[w] == i) printf("%d %d %d %d\n", w, order[w], u, i);
#endif
                            assert(degree[v] > 1||dominate[w]);
                            assert(degree[w] > 1||dominate[v]);
                        }
                }
        }
        else {
#ifndef NDEBUG
            for(ui j = pstart[u];j < pend[u];j ++) if(degree[edges[j]] == 1) printf("WA dominate!\n");
#endif
            for(ui j = pstart[u];j < pend[u];j ++) {
                ui v = edges[j];
                tri[j] = 0;
                for(ui k = pstart[v];k < pend[v];k ++) if(adj[edges[k]]) ++ tri[j];
                assert(tri[j]+1 != degree[v]);
                if(!dominate[v]&&tri[j]+1 == degree[u]) {
                    dominate[v] = 1;
                    if(order[v] > i) dominated.push_back(v);
                }
            }
        }

        for(ui j = pstart[u];j < pend[u];j ++) adj[edges[j]] = 0;
    }

    delete[] order;
    delete[] ids;

    return res;
}

void Graph::update_triangle(ui u1, ui u2, ui *pend, char *is, char *adj, ui *tri, int *degree, char *dominate, std::vector<ui> &dominated) {
    int cnt = 0;
    shrink(u1, pend[u1], is, tri);
    shrink(u2, pend[u2], is, tri);

    for(ui i = pstart[u1];i < pend[u1];i ++) adj[edges[i]] = 1;

    for(ui i = pstart[u2];i < pend[u2];i ++) if(adj[edges[i]]) {
            ui v = edges[i];
            tri[i] ++;
            ++ cnt;
            if(tri[i]+1 == degree[u2]&&!dominate[v]) {
                dominate[v] = 1;
                dominated.push_back(v);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u2]) {
                dominate[u2] = 1;
                dominated.push_back(u2);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u2) {
                    tri[j] ++;
                    break;
                }
        }

    for(ui i = pstart[u1];i < pend[u1];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u2) tri[i] = cnt;
    }

    if(cnt+1 == degree[u1]&&!dominate[u2]) {
        dominate[u2] = 1;
        dominated.push_back(u2);
    }
    if(cnt+1 == degree[u2]&&!dominate[u1]) {
        dominate[u1] = 1;
        dominated.push_back(u1);
    }

    for(ui i = pstart[u2];i < pend[u2];i ++) adj[edges[i]] = 1;

    for(ui i = pstart[u1];i < pend[u1];i ++) if(adj[edges[i]]) {
            ui v = edges[i];
            tri[i] ++;
            if(tri[i]+1 == degree[u1]&&!dominate[v]) {
                dominate[v] = 1;
                dominated.push_back(v);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u1]) {
                dominate[u1] = 1;
                dominated.push_back(u1);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u1) {
                    tri[j] ++;
                    break;
                }
        }

    for(ui i = pstart[u2];i < pend[u2];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u1) tri[i] = cnt;
    }
}

void Graph::near_maximum_near_linear(ui k) {
    //copy and used for last greedy deleting edges
    typedef std::pair<int, double> id_probability;
    auto cmp = [](id_probability a, id_probability b) {return a.second < b.second; };
    std::priority_queue<id_probability , std::vector<id_probability>, decltype(cmp) > pro_qu(cmp);
    if(LOG_USE){
        std::vector<double> ret = getLogProbability(n);

        for(int i = 0; i < n; ++i)
            pro_qu.push(std::make_pair(i, ret[i]));
    }

    ui *tmp_edges = new ui[m];
    for(ui i = 0;i < m;i ++) tmp_edges[i] = edges[i];
    ui *tmp_pstart = new ui[n+1];
    for(ui i = 0;i <= n;i ++) tmp_pstart[i] = pstart[i];
//#ifndef NDEBUG
    ui origin_k = k;
//#endif

#ifdef __LINUX__
    struct timeval start, end1, end;
	gettimeofday(&start, NULL);
#endif

    char *is = new char[n];//is 数组来表示当前节点是否仍可被使用
    for(ui i = 0;i < n;i ++) is[i] = 1;
    char *adj = new char[n];
    memset(adj, 0, sizeof(char)*n);

    int res = 0;
    std::vector<ui> degree_ones, degree_twos; //使用vector以暂时存储中间的节点
    int *degree = new int[n];
    for(ui i = 0;i < n;i ++) {
        degree[i] = pstart[i+1]-pstart[i];
    }

    std::vector<std::pair<ui,ui> > S;
    std::vector<std::pair<std::pair<ui,ui>, ui> > modified_edges;

    //initialize the given set
    {
        for(ui i = 0; i < gs_length; i++)
            is[given_set[i]] = 2;
        int tmpCnt = 0;
        for(ui i = 0; i < gs_length; i++){
            int x = given_set[i];
            for(ui j = pstart[x]; j < pstart[x + 1]; ++j){
                int y = edges[j];
                if(is[y] == 2)  ++tmpCnt;
                else if(is[y] == 1) delete_vertex(y, is, degree);
            }
        }
        assert((tmpCnt & 1) == 0);
        tmpCnt >>= 1;
        if(tmpCnt > k){
            delete[] is;
            delete[] adj;
            delete[] degree;
            printf("The deleting number should be greater than %d!", tmpCnt);
            return;
        }else   k -= tmpCnt;

        for(ui i = 0; i < gs_length; ++i){
            int x = given_set[i];
            delete_vertex(x, is, degree);
        }
    }

    for(ui i = 0; i < n; ++i)
        if(is[i]){
            if(degree[i] == 0) ++ res;
            else if(degree[i] == 1) degree_ones.push_back(i);
            else if(degree[i] == 2) degree_twos.push_back(i);
        }

    res += initial_dominance_and_degree_two_remove(degree_ones, degree_twos, is, degree, adj, S);

#ifdef __LINUX__
    struct timeval end_t1;
	gettimeofday(&end_t1, NULL);
#endif

#ifndef NDEBUG
    int new_n = 0;
    for(ui i = 0;i < n;i ++) {
        //if(is[i]&&degree[i] == 1) printf("WA degree one exist!\n");
        if(is[i]&&degree[i] > 0) ++ new_n;
    }
    printf("initial dominance and degree two remove: %d -> %d\n", n, new_n);
#endif

    ui *ids = new ui[n];
    ui ids_n = 0;

    ui new_m = 0;
    for(ui i = 0;i < n;i ++) {
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        ids[ids_n ++] = i;
        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m ++] = edges[j];
            }
    }
    pstart[n] = new_m;

    if(ids_n > 0) res += lp_reduction(ids, ids_n, is, degree);

#ifdef __LINUX__
    struct timeval end_t2;
	gettimeofday(&end_t2, NULL);
#endif

#ifndef NDEBUG
    int new_n2 = 0;
    for(ui i = 0;i < n;i ++) {
        if(is[i]&&degree[i] > 0) ++ new_n2;
    }
    printf("lp reduction: %d -> %d\n", new_n, new_n2);
#endif

    assert(degree_ones.empty()&&degree_twos.empty());
    for(ui i = 0;i < ids_n;i ++) if(is[ids[i]]&&degree[ids[i]] > 0) {
            if(degree[ids[i]] == 1) degree_ones.push_back(ids[i]);
            else if(degree[ids[i]] == 2) degree_twos.push_back(ids[i]);
        }

    res += remove_degree_one_two(degree_ones, degree_twos, is, degree, S);

    delete[] ids; ids = NULL;

    new_m = 0;
    for(ui i = 0;i < n;i ++) { // shrink the memory for storing edges
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m ++] = edges[j];
            }
    }
    pstart[n] = new_m;

    ui *pend = new ui[n];
    int max_dd = 0; // maximum degree
    for(ui i = 0;i < n;i ++) {
        pend[i] = pstart[i+1];
        if(pend[i] - pstart[i] > max_dd) max_dd = pend[i] - pstart[i];
    }
    //printf("max_d: %d, edges: %u\n", max_dd, new_m/2);

    int delete_tri = 0;
    ui *tri = NULL;
    if(new_m <= m/2) tri = edges+new_m;
    else {
        tri = new ui[pstart[n]];
        delete_tri = 1; // to avoid repeating free the same memory space
    }
    char *dominate = new char[n];
    memset(dominate, 0, sizeof(char)*n);
    std::vector<ui> dominated;

    res += compute_triangle_counts(tri, pend, adj, is, degree, dominate, dominated);

#ifdef __LINUX__
    struct timeval end_t3;
	gettimeofday(&end_t3, NULL);

	long long mtime11, seconds11, useconds11;
	seconds11 = end_t1.tv_sec - start.tv_sec;
	useconds11 = end_t1.tv_usec - start.tv_usec;
	mtime11 = seconds11*1000000 + useconds11;
	//printf("time1: %lld,", mtime11);
	seconds11 = end_t2.tv_sec - end_t1.tv_sec;
	useconds11 = end_t2.tv_usec - end_t1.tv_usec;
	mtime11 = seconds11*1000000 + useconds11;
	//printf(" time2: %lld,", mtime11);
	seconds11 = end_t3.tv_sec - end_t2.tv_sec;
	useconds11 = end_t3.tv_usec - end_t2.tv_usec;
	mtime11 = seconds11*1000000 + useconds11;
	//printf(" time3: %lld\n", mtime11);
#endif

#ifndef NDEBUG
    for(ui i = 0;i < n;i ++) if(is[i]&&!dominate[i]) {
            for(ui j = pstart[i];j < pstart[i+1];j ++) if(is[edges[j]]&&degree[edges[j]] == 1) {
                    printf("WA degree one not dominate!\n");
                }
        }
#endif

    int *bin_head = new int[ids_n];
    int *bin_next = new int[n];
    memset(bin_head, -1, sizeof(int)*ids_n);  // 根据度数建立了一张链表

    assert(degree_twos.empty());
    int max_d = 0;
    for(ui i = 0;i < n;i ++) if(is[i]&&degree[i] > 0) {
            bin_next[i] = bin_head[degree[i]];
            bin_head[degree[i]] = i;

            if(degree[i] == 2) degree_twos.push_back(i);
            if(degree[i] > max_d) max_d = degree[i];
        }

    char *fixed = new char[n];
    memset(fixed, 0, sizeof(char)*n);

    int kernal_size = 0, inexact = 0, first_time = 1, S_size = (int)S.size();
    int kernal_edges = 0;
    while(!dominated.empty()||!degree_twos.empty()||max_d >= 3) {
        while(!dominated.empty()||!degree_twos.empty()) { // to line 2088
            while(!dominated.empty()) {
                ui u = dominated.back();
                dominated.pop_back();
                if(!is[u]||degree[u] == 0) continue;

                if(!dominated_check(u, pend, is, tri, degree)) dominate[u] = 0;
                else res += delete_vertex(u, pend, is, degree_twos, tri, adj, degree, dominate, dominated);
            }

            while(!degree_twos.empty()&&dominated.empty()) {
                ui u = degree_twos.back();
                degree_twos.pop_back();
                if(!is[u]||degree[u] != 2) continue;

                shrink(u, pend[u], is, tri);//pstart并未被改变
                assert(pend[u] == pstart[u] + 2);
                ui u1 = edges[pstart[u]], u2 = edges[pstart[u]+1];

                ui pre = u, cnt = 1;
                while(u1 != u&&degree[u1] == 2) { // Degree 2 的复杂讨论
                    ++ cnt;
                    shrink(u1, pend[u1], is, tri);
                    assert(pend[u1] == pstart[u1] + 2);
                    int tmp = u1;
                    if(edges[pstart[u1]] != pre) u1 = edges[pstart[u1]];
                    else u1 = edges[pstart[u1]+1];
                    pre = tmp;
                }

#ifndef NDEBUG
                if(u1 != u&&degree[u1] <= 2) {
                    printf("%d u1(%d) degree_u1(%d) WAxx!\n", pre, u1, degree[u1]);
                    printf("%d:", u1);
                    for(ui k = pstart[u1];k < pend[u1];k ++) if(edges[k] != n&&is[edges[k]]) {
                            printf(" %d(tri:%d,dominate:%d)", edges[k], tri[k], dominate[edges[k]]);
                        }
                    printf("\n");
                }
#endif
                if(u1 == u) {
                    res += delete_vertex(u, pend, is, degree_twos, tri, adj, degree, dominate, dominated);
                    assert(!dominated.empty());
                    continue;
                }

                pre = u;
                while(degree[u2] == 2) {
                    ++ cnt;
                    shrink(u2, pend[u2], is, tri);
                    assert(pend[u2] == pstart[u2] + 2);
                    int tmp = u2;
                    if(edges[pstart[u2]] != pre) u2 = edges[pstart[u2]];
                    else u2 = edges[pstart[u2]+1];
                    pre = tmp;
                }
                if(u1 == u2) {
                    res += delete_vertex(u1, pend, is, degree_twos, tri, adj, degree, dominate, dominated);
                    assert(!dominated.empty());
                    continue;
                }

                if(cnt%2 == 1) { //without folding
                    if(exist_edge(u1,u2, pend)) {
                        res += delete_vertex(u1, pend, is, degree_twos, tri, adj, degree, dominate, dominated);
                        res += delete_vertex(u2, pend, is, degree_twos, tri, adj, degree, dominate, dominated);
                        assert(!dominated.empty());
                    }
                    else if(cnt > 1) {
                        ui idx = pstart[pre];
                        if(edges[idx] == u2) ++ idx;
                        assert(degree[pre] == 2&&tri[idx] == 0);
                        u = edges[idx];
                        edges[idx] = u1;
                        if(!first_time) modified_edges.push_back(std::make_pair(std::make_pair(pre,u), u1));

                        u2 = pre;
                        while(u != u1) {
                            is[u] = 0;
                            int tmp = u;
                            if(edges[pstart[u]] == pre) u = edges[pstart[u]+1];
                            else u = edges[pstart[u]];
                            pre = tmp;
                            S.push_back(std::make_pair(pre,u));
                        }
                        idx = edge_rewire(u1, pend, pre, u2);
                        assert(tri[idx] == 0);
                        if(!first_time) modified_edges.push_back(std::make_pair(std::make_pair(u1,pre),u2));
                    }
                }
                else {
                    ui v2 = pre, v1 = pre;
                    pre = u2;
                    while(v1 != u1) {
                        is[v1] = 0;
                        int tmp = v1;
                        if(edges[pstart[v1]] == pre) v1 = edges[pstart[v1]+1];
                        else v1 = edges[pstart[v1]];
                        pre = tmp;
                        S.push_back(std::make_pair(pre,v1));
                    }
                    v1 = pre;
                    if(exist_edge(u1, u2, pend)) {
                        if((-- degree[u1]) == 2) degree_twos.push_back(u1);
                        if((-- degree[u2]) == 2) degree_twos.push_back(u2);

                        assert(degree[u1] > 1&&degree[u2] > 1);

                        for(ui k = pstart[u1];k < pend[u1];k ++) {
                            if(is[edges[k]]&&!dominate[edges[k]]&&tri[k]+1 == degree[u1]) {
                                dominate[edges[k]] = 1;
                                dominated.push_back(edges[k]);
                            }
                        }
                        for(ui k = pstart[u2];k < pend[u2];k ++) {
                            if(is[edges[k]]&&!dominate[edges[k]]&&tri[k]+1 == degree[u2]) {
                                dominate[edges[k]] = 1;
                                dominated.push_back(edges[k]);
                            }
                        }
                    }
                    else {
                        ui idx = edge_rewire(u1, pend, v1, u2);
                        assert(tri[idx] == 0);
                        idx = edge_rewire(u2, pend, v2, u1);
                        assert(tri[idx] == 0);

                        if(!first_time) {
                            modified_edges.push_back(std::make_pair(std::make_pair(u1,v1),u2));
                            modified_edges.push_back(std::make_pair(std::make_pair(u2,v2),u1));
                        }

                        update_triangle(u1, u2, pend, is, adj, tri, degree, dominate, dominated);
                    }
                }
            }
        }

        if(first_time) {//for the first time, calculate the kernel size
            first_time = 0;
            S_size = (int)S.size();
            for(ui k = 0;k < n;k ++) {
                if(is[k]&&degree[k] > 0) {
                    ++ kernal_size;
                    for(ui j = pstart[k];j < pend[k];j ++) if(is[edges[j]]) ++ kernal_edges;
                }
                else fixed[k] = 1;
            }
        }

        while(dominated.empty()&&degree_twos.empty()) {//peeling
            if(LOG_USE){
                bool flag = true;
                while(true){
                    if(pro_qu.empty()){
                        flag = false;
                        break;
                    }
                    auto tmp = pro_qu.top();
                    pro_qu.pop();
                    if(!is[tmp.first])  continue;
                    int x = tmp.first;
                    for(ui i = pstart[x]; i < pstart[x + 1]; ++i)
                        if(is[edges[i]]){
                            res += delete_vertex(edges[i], pend, is, degree_twos, tri, adj, degree, dominate, dominated);
                        }
                    break;

                }
                if(flag)    continue;
            }

            while(max_d >= 3&&bin_head[max_d] == -1) -- max_d;
            if(max_d < 3) break;

            int v = -1;
            for(v = bin_head[max_d];v != -1;) {
                int tmp = bin_next[v];
                if(is[v]&&degree[v] > 0) {
                    if(degree[v] < max_d) {
                        bin_next[v] = bin_head[degree[v]];
                        bin_head[degree[v]] = v;
                    }
                    else {
                        S.push_back(std::make_pair(v,n)); ++ inexact;
                        res += delete_vertex(v, pend, is, degree_twos, tri, adj, degree, dominate, dominated);

                        bin_head[max_d] = tmp;
                        break;
                    }
                }
                v = tmp;
            }
            if(v == -1) bin_head[max_d] = -1;
        }
    }

    ui UB = 0;

    for(int i = S.size()-1;i >= 0;i --) {
        ui u1 = S[i].first, u2 = S[i].second;
        assert(is[u1] == 0);

        if(u2 != n) {
            if(!is[u2]) {
                is[u1] = 1;
#ifndef NDEBUG
                for(ui j = tmp_pstart[u1];j < tmp_pstart[u1+1];j ++) if(is[tmp_edges[j]]) {
                        printf("WA conflict1!\n");
                    }
#endif
                ++ res;
            }
            continue;
        }

        int ok = 1;
        for(ui j = pstart[u1];j < pstart[u1+1];j ++) if(is[edges[j]]) {
                ok = 0;
                break;
            }
        if(ok) { // adding the legal peeling vertex
            is[u1] = 1;
            ++ res;

#ifndef NDEBUG
            for(ui j = tmp_pstart[u1];j < tmp_pstart[u1+1];j ++) if(is[tmp_edges[j]]) {
                    printf("WA conflict2!\n");
                }
#endif
        }
        else ++ UB;
    }

    printf("Degree_two_dominate_lp MIS: %d (kernal (|V|,|E|): (%d,%d), inexact reduction: %d, UB: %d)\n"
            , res, kernal_size, kernal_edges, inexact, res+UB);

    delete[] bin_head;
    delete[] bin_next;
    delete[] degree;

    delete[] pend;
    if(delete_tri) delete[] tri;
    delete[] dominate;
    delete[] adj;

#ifdef __LINUX__
    gettimeofday(&end1, NULL);

	long long mtime1, seconds1, useconds1;
	seconds1 = end1.tv_sec - start.tv_sec;
	useconds1 = end1.tv_usec - start.tv_usec;
	mtime1 = seconds1*1000000 + useconds1;
#endif


    for(int i = (int)modified_edges.size()-1;i >= 0;i --) {
        ui u = modified_edges[i].first.first, u1 = modified_edges[i].first.second, u2 = modified_edges[i].second;
        for(ui j = pstart[u];j < pstart[u+1];j ++) if(edges[j] == u2) {
                edges[j] = u1;
                break;
            }
    }

    /*
    if(inexact) {
        res = ARW(is, fixed, mtime1, time_limit);
        for(int i = S_size-1;i >= 0;i --) {
            ui u1 = S[i].first, u2 = S[i].second;

            if(!is[u2]) is[u1] = 1;
            else is[u1] = 0;
        }
    }
    */

    std::swap(edges, tmp_edges);
    delete[] tmp_edges;
    std::swap(pstart, tmp_pstart);
    delete[] tmp_pstart;

    for(ui i = 0; i < gs_length; ++i)
        is[given_set[i]] = 1;


    res += gs_length;
    printf("The size of MIS before deleting %d edges is: %d\n", origin_k, res);
    res += greedy_delete_edges(is, k);
    printf("The size of MIS after deleting %d edges is: %d", origin_k, res);
#ifndef NDEBUG
//    compute_upperbound(is, fixed);
    check_is(is, res, origin_k);
#endif
#ifdef __LINUX__
    gettimeofday(&end, NULL);

    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds*1000000 + useconds;

    printf("Process time: %lld, Swap time: %lld, Total time: %lld\n", mtime1, mtime-mtime1, mtime);
#endif
    if(LOCALSEARCH){
        BasicVersion localSearch(n, m, given_set, gs_length, pstart, edges, origin_k, is);
        localSearch.iterate_local_search();
#ifdef CHECK_BUG
        //        printf("Begin Check!");
        check_is(localSearch.getOptimum(),
                localSearch.getOptimumSize(), origin_k);
#endif
    }
    delete[] is;
    delete[] fixed;

}
extern int INDEX_TYPE;
void Graph::pay_and_try_dominate_max_degree_greedy_delete_edges(ui k) {

    PayAndTry* payAndTry = (INDEX_TYPE < 2)?
            new NearLinearPayAndTry(n, m, given_set, gs_length,pstart, edges, k)
            :
            new ProbabilityPayAndTry(n, m, given_set, gs_length, pstart, edges, k);
//    PayAndTry* payAndTry = new RiskPayAndTry(n, m, given_set, gs_length
//            , pstart, edges, k);
#ifdef __LINUX__
    struct timeval start, endl, end;
    gettimeofday(&start, NULL);
#endif
//    payAndTry->init();
    int res = pay_and_try_framework(payAndTry);
    printf("The MIS size get by PayAndTry-dominate_max_degree_greedy after deleting %d edges is %d\n",
            k, res);
#ifdef __LINUX__
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds * 1000000 + useconds;

    printf("Process time: %lld\n",mtime);
#endif
    if(LOCALSEARCH)
        payAndTry->localSearch();
    res = payAndTry->getMISSize();
    assert(payAndTry->check_result(res, k));
    delete payAndTry;
}

void Graph::exact_solver(ui k) {
    ExactSolver solver(n, m, given_set, gs_length, pstart, edges);
    printf("The result calculated by ExactSolver is : %d \n",
            solver.solve(k));
}