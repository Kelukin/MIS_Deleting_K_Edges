//
// Created by Kelukin on 2020/5/27.
//

#include "NearLinearPayAndTry.h"
extern int INDEX_TYPE;
void NearLinearPayAndTry::localSearch() {
    BasicVersion localSearch(n, m, given_set, gs_length, tmp_pstart, tmp_edges, ori_k, is);
    localSearch.iterate_local_search();
    const char *ret = localSearch.getOptimum();
    memcpy(is, ret, sizeof(char) * n);
}
bool NearLinearPayAndTry::check_result(ui mis_size, ui ori_k) {
#ifndef NDEBUG
    ui conflict_cnt = 0;
    ui mis_cnt = 0;
    for(ui i = 0; i < n; ++i)
        if(is[i]){
            ++mis_cnt;
            for(ui j = tmp_pstart[i]; j < tmp_pstart[i + 1]; ++j){
                int x = tmp_edges[j];
                if(is[x])   ++conflict_cnt;
            }
        }
    conflict_cnt/=2;
    printf("original k: %d, the fact conflict edges: %d\n", ori_k, conflict_cnt);
    printf("mis : %d %d\n", mis_size, mis_cnt);
    return mis_size == mis_cnt && ori_k >= conflict_cnt;
#endif
    return true;
}

void NearLinearPayAndTry::edge_rewire(ui u, ui u1, ui u2) {
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(edges[i] == u1) {
            edges[i] = u2;
            break;
        }
}


ui NearLinearPayAndTry::edge_rewire(ui u, const ui *pend, ui v, ui w) {
#ifndef NDEBUG
    for(ui i = pstart[u];i < pend[u];i ++) if(edges[i] == w && edgeValid[i])
            printf("WA preexist edge!\n");
#endif

    for(ui i = pstart[u];i < pend[u];i ++) if(edges[i] == v && edgeValid[i]) {
            edges[i] = w;
            edgeDeleteIndex->edgeRewire(i, u, v, w);
            return i;
        }
    printf("WA in edge_rewire!\n");
    return 0;
}
ui NearLinearPayAndTry::get_other_neighbor(ui u, ui u1) {
    ui idx = 0;
    for(ui i = pstart[u];i < pstart[u+1];i ++) if(is[edges[i]]) {
            if(edgeValid && !edgeValid[i])  continue;
            std::swap(edges[pstart[u]+idx], edges[i]);
            if((++ idx) == 2) break;
        }
    assert(edges[pstart[u]] == u1||edges[pstart[u]+1] == u1);
    if(edges[pstart[u]] == u1) return edges[pstart[u]+1];
    return edges[pstart[u]];
}

int NearLinearPayAndTry::delete_vertex(ui v, char *is, int *degree, std::vector<ui> &degree_ones,
                                       std::vector<ui> &degree_twos) {
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

int NearLinearPayAndTry::remove_degree_one_two() {
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
            get_two_neighbors(u, u1, u2);
            assert(u1 != n&&u2 != n);

            ui pre = u, cnt = 1;
            while(u1 != u&&degree[u1] == 2) {
                ++ cnt;
                ui tmp = get_other_neighbor(u1, pre);
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
                ui tmp = get_other_neighbor(u2, pre);
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
                    u = get_other_neighbor(pre, u2);
                    edge_rewire(pre, u, u1);

                    u2 = pre;
                    while(u != u1) {
                        is[u] = 0;
                        ui tmp = get_other_neighbor(u, pre);
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
                    ui tmp = get_other_neighbor(v1, pre);
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

void NearLinearPayAndTry::shrink(ui u, ui &end) {
    ui i = pstart[u];
    while(true) {  //  确保在前的都是尚未被删除的节点
        if(edgeValid != nullptr){
            while(i < end && is[edges[i]] && edgeValid[i])  ++ i;
            while(i < end && (!is[edges[end - 1]] || !edgeValid[end - 1]) ) --end;
        } else{
            while(i < end&&is[edges[i]]) ++ i;
            while(i < end&&!is[edges[end-1]]) -- end;
        }

        if(i >= end) break;
        std::swap(edges[i], edges[end-1]);
        std::swap(tri[i], tri[end-1]);//边的计数也随之交换

        if(edgeValid) std::swap(edgeValid[i], edgeValid[end - 1]);
        if(edgeDeleteIndex)
            edgeDeleteIndex->swap(i, end - 1);
//            std::swap(diff[i], diff[end - 1]);
        if(reverseEdge){
            int p = reverseEdge[i], q = reverseEdge[end - 1];
            std::swap(reverseEdge[i], reverseEdge[end - 1]);
//            if(edgeDeleteIndex) edgeDeleteIndex->swap(reverseEdge[i], reverseEdge[end - 1]);
            reverseEdge[p] = end - 1;
            reverseEdge[q] = i;
//            assert(reverseEdge[p] >= pstart[edges[p]] && reverseEdge[p] < pend[edges[p]]);
//            assert(reverseEdge[q] >= pstart[edges[q]] && reverseEdge[q] < pend[edges[q]]);
//            assert(reverseEdge[i] >= pstart[edges[i]] && reverseEdge[i] < pend[edges[i]]);
//            assert(reverseEdge[end - 1] >= pstart[edges[end - 1]] && reverseEdge[end - 1] < pend[edges[end - 1]]);

        }
    }
}

void NearLinearPayAndTry::addDominated(ui u, ui v) {
    dominate[u] = 1;
    if(edgeDeleteIndex->hasDeletedEdge(u)){
        // in high priority
        dominated.push_back(u);
    }
    else dominated.push_front(u);
}

int NearLinearPayAndTry::compute_triangle_counts() {
    std::vector<ui> vs;
    for(ui i = 0;i < n;i ++) if(is[i]&&degree[i] > 0) vs.push_back(i);

    ui *ids = new ui[vs.size()];
    memset(ids, 0, sizeof(ui)*vs.size());
    for(unsigned int v : vs) ++ ids[degree[v]];
    for(ui i = 1;i < vs.size();i ++) ids[i] += ids[i-1];

    int *order = new int[n];
    memset(order, -1, sizeof(int)*n);
    for(unsigned int v : vs) order[v] = (-- ids[degree[v]]);

    for(unsigned int v : vs) ids[order[v]] = v;//descending in vertex's degree

    int res = 0;
    for(int i = (int)vs.size()-1;i >= 0;i --) {
        ui u = ids[i];
        shrink(u, pend[u]);

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
                                dominated.push_back(v);
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
                    if(order[v] > i){
                        dominate[v] = 1;
                        dominated.push_back(v);
                    }
                }
            }
        }

        for(ui j = pstart[u];j < pend[u];j ++) adj[edges[j]] = 0;
    }

    delete[] order;
    delete[] ids;

    return res;
}

int NearLinearPayAndTry::lp_reduction(const ui *ids, ui ids_n) {
    int *new_id = new int[n];
    memset(new_id, -1, sizeof(int)*n);
    for(ui i = 0;i < ids_n;i ++) new_id[ids[i]] = i;
    for(ui i = 0;i < ids_n;i ++) {
        ui u = ids[i];
        for(ui j = pstart[u];j < pstart[u+1];j ++) edges[j] = new_id[edges[j]];
    }
    delete[] new_id;

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

void NearLinearPayAndTry::update_triangle(ui u1, ui u2) {
    int cnt = 0;
    shrink(u1, pend[u1]);
    shrink(u2, pend[u2]);

    for(ui i = pstart[u1];i < pend[u1];i ++) adj[edges[i]] = 1;

    for(ui i = pstart[u2];i < pend[u2];i ++) if(adj[edges[i]]) {
            ui v = edges[i];
            tri[i] ++;
            edgeDeleteIndex->descDiffValue(i);
//            --diff[i];
            ++ cnt;
            if(tri[i]+1 == degree[u2]&&!dominate[v]) {
                addDominated(v, u2);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u2]) {
                addDominated(u2, v);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u2) {
                    tri[j] ++;
                    edgeDeleteIndex->descDiffValue(j);
//                    --diff[j];
                    break;
                }
        }

    for(ui i = pstart[u1];i < pend[u1];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u2){
            tri[i] = cnt;
//            diff[i] = degree[u2] - 1 - cnt;
            edgeDeleteIndex->setDiffValue(i, degree[u2] - 1 - cnt);
        }
    }

    if(cnt+1 == degree[u1]&&!dominate[u2]) {
        addDominated(u2, u1);
    }
    if(cnt+1 == degree[u2]&&!dominate[u1]) {
        addDominated(u1, u2);
    }

    for(ui i = pstart[u2];i < pend[u2];i ++) adj[edges[i]] = 1;

    for(ui i = pstart[u1];i < pend[u1];i ++) if(adj[edges[i]]) {
            ui v = edges[i];
            tri[i] ++;
//            --diff[i];
            edgeDeleteIndex->descDiffValue(i);
            if(tri[i]+1 == degree[u1]&&!dominate[v]) {
                addDominated(v, u1);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u1]) {
                addDominated(u1, v);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u1) {
                    tri[j] ++;
                    edgeDeleteIndex->incDiffValue(j);
//                    ++diff[j];
                    break;
                }
        }

    for(ui i = pstart[u2];i < pend[u2];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u1){
            tri[i] = cnt;
            edgeDeleteIndex->setDiffValue(i, degree[u1] -1 - cnt);
        }
    }
}

bool NearLinearPayAndTry::canSafeReduction() {
    return !dominated.empty() || !degree_twos.empty();
}

void NearLinearPayAndTry::update_max_d(){
    int v = -1;
    do{
        while(max_d >= 3 && bin_head[max_d] == -1) -- max_d;
        if(max_d < 3)   return;
        for(v = bin_head[max_d]; v != -1;){
            int tmp = bin_next[v];
            if(!is[v] || degree[v] < max_d){
                bin_next[v] = bin_head[degree[v]];
                bin_head[degree[v]] = v;
            }else{
                bin_head[max_d] = v;
                break;
            }

            v = tmp;
        }
        if(v == -1) bin_head[max_d] = -1;
    }while(v == -1);
}

void NearLinearPayAndTry::initial_dominance_and_degree_two_remove() {
    remove_degree_one_two();
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
        ui u = ids[i];
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
            delete_vertex(u, is, degree, degree_ones, degree_twos);
            remove_degree_one_two();
        }
    }

    delete[] ids;
    delete[] order;
}

bool NearLinearPayAndTry::dominated_check(ui u) {
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(edgeValid && !edgeValid[i]) continue;
        if(is[edges[i]] && tri[i] + 1 == degree[edges[i]])
            return true;
    }

    return false;
}

int NearLinearPayAndTry::delete_vertex_dominate(ui u) {
    shrink(u, pend[u]);//certain the range in [pstart[u], pend[u]) must be valid
    is[u] = 0;
    k += edgeDeleteIndex->deleteVertex(u);
    int res = 0;
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(edgeValid && !edgeValid[i]) continue;
        ui v = edges[i];
        adj[v] = 1;
        -- degree[v];
        for(ui j = pstart[v]; j < pend[v]; ++j){
            int w = edges[j];
            if(!is[w] || (edgeValid && !edgeValid[j]))  continue;
            edgeDeleteIndex->descDiffValue(reverseEdge[j]); // descend, because of the change of degree[v]
            assert(edges[reverseEdge[j]] == v);
        }
        if(degree[v] == 0)  ++res;
        else if(degree[v] == 2) degree_twos.push_back(v);
    }

    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(edgeValid && !edgeValid[i]) continue;
        ui v = edges[i];

        for(ui j = pstart[v]; j < pend[v]; ++j)
            if(!is[edges[j]] || (edgeValid && !edgeValid[j]))   continue;
            else{
                ui w = edges[j];
                if(adj[w]){
                    --tri[j];
                    edgeDeleteIndex->incDiffValue(j);
                }
                if(tri[j] + 1 == degree[v] && !dominate[w]){
                    addDominated(w, v);
                }
            }
    }
    //we may check the consistency of diff[i] and diff[reverseEdge[i]] here
    for(ui i = pstart[u]; i < pend[u]; ++i) adj[edges[i]] = 0;
    if(edgeValid)
        for(ui i = pstart[u]; i < pend[u]; ++i){
            edgeValid[i] = edgeValid[reverseEdge[i]] = false;
            assert(edges[reverseEdge[i]] == u);
        }
    return res;
}

void NearLinearPayAndTry::init(){
    for(ui i = 0; i < n; ++i)
        if(is[i]){
            if(degree[i] == 1)  degree_ones.push_back(i);
            else if(degree[i] == 2) degree_twos.push_back(i);
        }
    initial_dominance_and_degree_two_remove();
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

    if(ids_n > 0) lp_reduction(ids, ids_n);
    assert(degree_ones.empty()&&degree_twos.empty());
    for(ui i = 0;i < ids_n;i ++) if(is[ids[i]]&&degree[ids[i]] > 0) {
            if(degree[ids[i]] == 1) degree_ones.push_back(ids[i]);
            else if(degree[ids[i]] == 2) degree_twos.push_back(ids[i]);
        }

    remove_degree_one_two();

    delete[] ids;
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

    for(ui i = 0;i < n;i ++) {
        pend[i] = pstart[i+1];
    }
    //printf("max_d: %d, edges: %u\n", max_dd, new_m/2);

    dominate = new char[n];
    memset(dominate, 0, sizeof(char) * n);
    //the default value of reverseEdge is quite large. must be larger than m
//    diff = new ui[new_m];

    if(new_m <= m/2) tri = edges+new_m;
    else {
        tri = new ui[pstart[n]];
        delete_tri = true; // to avoid repeating free the same memory space
    }


    compute_triangle_counts();

    new_m = 0;
    for(ui i = 0;i < n;i ++) { // shrink the memory for storing edges
        ui tmp = pstart[i];
        pstart[i] = new_m;
        if(!is[i]||degree[i] <= 0) continue;

        for(ui j = tmp;j < pstart[i+1];j ++) if(is[edges[j]]) {
                assert(degree[edges[j]] > 0);
                edges[new_m] = edges[j];
                tri[new_m] = tri[j];
                ++new_m;
            }
    }
    pstart[n] = new_m;

    for(ui i = 0;i < n;i ++) {
        pend[i] = pstart[i+1];
    }




    reverseEdge = new ui[new_m];
    memset(reverseEdge, 0xff, sizeof(ui) * new_m);
    //initialize the triangle count and reverseEdge
    for(ui i = 0; i < n; ++i)
        if(is[i] && degree[i] > 0)
            for(ui u = pstart[i]; u < pend[i]; ++u){
                ui x = edges[u];
                if(!is[x]) continue;
                for(ui t = pstart[x]; t < pend[x] && reverseEdge[u] > m; ++t ){
                    ui y = edges[t];
                    if(y == i){
                        reverseEdge[u] = t;
                        reverseEdge[t] = u;
                        break;
                    }
                }
                assert(reverseEdge[u] < m);
            }
    bin_head = new int[ids_n];
    bin_next = new int[n];
    memset(bin_head, -1, sizeof(int) * ids_n);
    for(ui i = 0;i < n;i ++) if(is[i]&&degree[i] > 0) {
            bin_next[i] = bin_head[degree[i]];
            bin_head[degree[i]] = i;

            if(degree[i] == 2) degree_twos.push_back(i);
            if(degree[i] > max_d) max_d = degree[i];
        }

    fixed = new char[n];
    memset(fixed, 0, sizeof(char) * n);
    if(new_m != 0){
        if(INDEX_TYPE == 1) edgeDeleteIndex = new ProfitDeleteIndex(n, new_m, max_d, edges, is, reverseEdge, pstart, pend, degree);
        else edgeDeleteIndex = new EdgeDeleteIndex(n, new_m, max_d, edges, is, reverseEdge, pstart, pend, degree);
    }
    for(ui i = 0; i < n; ++i)
        if(is[i] && degree[i]>0)
            for(ui u = pstart[i]; u < pend[i]; ++u){
                ui x = edges[u];
                if(!is[x])  continue;

//                diff[u] = degree[x] - 1 - tri[u];//for edge <i, x>
                edgeDeleteIndex->setDiffValue(u, degree[x] - 1 - tri[u]);
            }

}
extern  int LAST_DELETE_TYPE;
void NearLinearPayAndTry::last_delete() {
    printf("the residual K is %d\n", k);
    for(ui i = 0; i < gs_length; ++i)
        is[given_set[i]] = 1;
    //recalculate the k in fact
    k = 0;
    for (ui x = 0; x < n; ++ x)
        if(is[x])
            for(ui i = tmp_pstart[x]; i < tmp_pstart[x + 1]; ++i){
                if(is[tmp_edges[i]])    ++k;
            }
    k = ori_k - k/2;
    printf("the fact residul K is %d\n", k);
    if(LAST_DELETE_TYPE == 0) LastDeleter::greedy_delete_edges(is, tmp_pstart, tmp_edges, k, n);
    else LastDeleter::smart_delete_edges(is, tmp_pstart, tmp_edges, k, n, given_set, gs_length);
}

void NearLinearPayAndTry::safe_reduction() {
    while(!dominated.empty()){
        ui u = dominated.back();
        dominated.pop_back();
        if(!is[u] || degree[u] == 0)    continue;
        if(!dominated_check(u)) dominate[u] = 0;
        else delete_vertex_dominate(u);
    }

    while(!degree_twos.empty()&&dominated.empty()) {
        ui u = degree_twos.back();
        degree_twos.pop_back();
        if(!is[u]||degree[u] != 2) continue;

        shrink(u, pend[u]);//pstart并未被改变
        assert(pend[u] == pstart[u] + 2);
        ui u1 = edges[pstart[u]], u2 = edges[pstart[u]+1];

        ui pre = u, cnt = 1;
        while(u1 != u&&degree[u1] == 2) { // Degree 2 的复杂讨论
            ++ cnt;
            shrink(u1, pend[u1]);
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
            for(ui k = pstart[u1];k < pend[u1];k ++) if(edgeValid[k] && edges[k] != n&&is[edges[k]]) {
                    printf(" %d(tri:%d,dominate:%d)", edges[k], tri[k], dominate[edges[k]]);
                }
            printf("\n");
        }
#endif
        if(u1 == u) {
            delete_vertex_dominate(u);
            assert(!dominated.empty());
            continue;
        }

        pre = u;
        while(degree[u2] == 2) {
            ++ cnt;
            shrink(u2, pend[u2]);
            assert(pend[u2] == pstart[u2] + 2);
            int tmp = u2;
            if(edges[pstart[u2]] != pre) u2 = edges[pstart[u2]];
            else u2 = edges[pstart[u2]+1];
            pre = tmp;
        }
        if(u1 == u2) {
            delete_vertex_dominate(u1);
            assert(!dominated.empty());
            continue;
        }

        if(cnt%2 == 1) {
            if(exist_edge(u1,u2)) {
                delete_vertex_dominate(u1);
                delete_vertex_dominate(u2);
                assert(!dominated.empty());
            }
            else if(cnt > 1) {
                ui idx = pstart[pre];
                shrink(pre, pend[pre]);
                if(edges[idx] == u2) ++ idx;
                assert(degree[pre] == 2&&tri[idx] == 0);
                u = edges[idx];

                ui x = edge_rewire(pre, pend, u, u1);
                edgeDeleteIndex->setDiffValue(x, degree[u1] - 1);

//                edges[idx] = u1;
//                ui x = idx;

                u2 = pre; // target
                while(u != u1) {
                    is[u] = 0;
//                    k += edgeDeleteIndex->deleteVertex(u);
                    int tmp = u;
                    if(edges[pstart[u]] == pre) u = edges[pstart[u]+1];
                    else u = edges[pstart[u]];
                    pre = tmp;
                    S.push_back(std::make_pair(pre,u));
                }
                idx = edge_rewire(u1, pend, pre, u2);
                reverseEdge[idx] = x;
                reverseEdge[x] = idx;

                edgeDeleteIndex->setReverseRelation(idx, x);

                assert(tri[idx] == 0);
            }
        }
        else {
            ui v2 = pre, v1 = pre;
            pre = u2;
            while(v1 != u1) {
                is[v1] = 0;
//                k += edgeDeleteIndex->deleteVertex(v1);
                int tmp = v1;
                if(edges[pstart[v1]] == pre) v1 = edges[pstart[v1]+1];
                else v1 = edges[pstart[v1]];
                pre = tmp;
                S.push_back(std::make_pair(pre,v1));
            }
            v1 = pre;
            if(exist_edge(u1, u2)) {
                if((-- degree[u1]) == 2) degree_twos.push_back(u1);
                if((-- degree[u2]) == 2) degree_twos.push_back(u2);

                assert(degree[u1] > 1&&degree[u2] > 1);

                for(ui k = pstart[u1];k < pend[u1];k ++) if(edgeValid[k]){
                        if(is[edges[k]]&&!dominate[edges[k]]&&tri[k]+1 == degree[u1]) {
                            addDominated(edges[k], u1);
                        }
                        edgeDeleteIndex->descDiffValue(reverseEdge[k]);
                    }
                for(ui k = pstart[u2];k < pend[u2];k ++) if(edgeValid[k]){
                        if(is[edges[k]]&&!dominate[edges[k]]&&tri[k]+1 == degree[u2]) {
                            addDominated(edges[k], u2);
                        }
                        edgeDeleteIndex->descDiffValue(reverseEdge[k]);
                    }
            }
            else {
                ui idx = edge_rewire(u1, pend, v1, u2);
                assert(tri[idx] == 0);
                ui x  = edge_rewire(u2, pend, v2, u1);
                assert(tri[x] == 0);
                reverseEdge[x] = idx;
                reverseEdge[idx] = x;

                edgeDeleteIndex->setReverseRelation(x, idx);

                update_triangle(u1, u2);
            }
        }
    }
}
void NearLinearPayAndTry::deleteEdge(int edgeNo, int u, int v) {
    // decrease two endpoints' degree
    // maintain their triangle count
    if(!edgeValid[edgeNo]) return;
    edgeValid[edgeNo] = edgeValid[reverseEdge[edgeNo]] = false;
    edgeDeleteIndex->deleteEdge(edgeNo);
    edgeDeleteIndex->deleteEdge(reverseEdge[edgeNo]);
    --degree[u];
    --degree[v];
    for(ui i = pstart[v]; i < pend[v]; ++i){
        if(!edgeValid[i] || ! is[edges[i]]) continue;
        edgeDeleteIndex->descDiffValue(reverseEdge[i]);
    }
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(!edgeValid[i] || !is[edges[i]])  continue;
        edgeDeleteIndex->descDiffValue(reverseEdge[i]);
    }

    if(degree[u] == 2)  degree_twos.push_back(u);
    if(degree[v] == 2)  degree_twos.push_back(v);
    for(ui i = pstart[v]; i < pend[v]; ++i) if(edgeValid[i]) adj[edges[i]] = 1;
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(!edgeValid[i] || !is[edges[i]]) continue;
        if(adj[edges[i]]){
            --tri[i];
            --tri[reverseEdge[i]];
            edgeDeleteIndex->incDiffValue(i);
            edgeDeleteIndex->incDiffValue(reverseEdge[i]);

        } else if(degree[u] - 1 == tri[i]){
            if(!dominate[edges[i]]){
                addDominated(edges[i], u);
            }
        }
    }
    for(ui i = pstart[v]; i < pend[v]; ++i) adj[edges[i]] = 0;

    for(ui i = pstart[u]; i < pend[u]; ++i) if(edgeValid[i])adj[edges[i]] = 1;
    for(ui i = pstart[v]; i < pend[v]; ++i){
        if(!edgeValid[i] || ! is[edges[i]]) continue;
        if(adj[edges[i]]){
            --tri[i];
            --tri[reverseEdge[i]];
            edgeDeleteIndex->incDiffValue(i);
            edgeDeleteIndex->incDiffValue(reverseEdge[i]);

        } else if(degree[v] - 1 == tri[i]){
            if(!dominate[edges[i]]){
                addDominated(edges[i], v);
            }
        }
    }
    for(ui i = pstart[u]; i < pend[u]; ++i) adj[edges[i]] = 0;

}

void NearLinearPayAndTry::inexact_delete() {
    //isDone returns false;
    update_max_d();
    ui x = bin_head[max_d];
    S.push_back(std::make_pair(x, n));
    delete_vertex_dominate(x);
    update_max_d();
    assert(bin_head[max_d] != x);
}
bool NearLinearPayAndTry::isDone() {
    update_max_d();
    return max_d <= 2 && dominated.empty() && degree_twos.empty();
}

int NearLinearPayAndTry::pay() {
    auto tmp = edgeDeleteIndex->getMinCostEdge();
    if(k < tmp.first)   return -1;

//    if(tmp.first >=5) return -1;

    k -= tmp.first;
    std::pair<ui, ui> &p = tmp.second;
    int x = p.first, y = p.second;
    shrink(y, pend[y]);
    shrink(x, pend[x]);
//    edgeDeleteIndex->getMinCostEdge();
    for(ui i = pstart[x]; i < pend[x]; ++i) if(edgeValid[i] && is[edges[i]]){
            pay_adj[edges[i]] = 1;
        }
    pay_adj[x] = 1;
#ifndef NDEBUG
    int cnt = 0;
#endif
    for(ui i = pstart[y]; i < pend[y]; ++i)
        if(edgeValid[i]){
            if(!pay_adj[edges[i]] && is[edges[i]]){
                deleteEdge(i, y, edges[i]);
#ifndef NDEBUG
                ++cnt;
#endif
            }
        }

    for(ui i = pstart[x]; i < pend[x]; ++i) if(edgeValid[i]){
            pay_adj[edges[i]] = 0;
        }
    pay_adj[x] = 0;

#ifndef NDEBUG
    assert(cnt == tmp.first);
#endif
//    assert(!dominated.empty());
    return 0;
}

void NearLinearPayAndTry::clear_S() {
    for(std::vector< std::pair<ui, ui> >::reverse_iterator
                it = S.rbegin(); it != S.rend(); ++it){
        ui u1 = it->first, u2 = it->second;
        assert(is[u1] == 0);
        if(u2 != n){
            if(!is[u2]) is[u1] = 1;
        }else{
            bool flag = true;
            for(ui j = pstart[u1]; flag && j < pstart[u1 + 1]; ++j)
                if(is[edges[j]]){
                    flag = false;
                }
            if(flag)    is[u1] = 1;
        }
    }
}

void NearLinearPayAndTry::appending_action() {
    clear_S();
}


bool NearLinearPayAndTry::exist_edge(ui u1, ui u2) {
    if(pstart[u1 + 1]-pstart[u1] < pstart[u2+1]-pstart[u2]) {
        for(ui i = pstart[u1];i < pstart[u1+1];i ++) if(edgeValid[i] && edges[i] == u2) return true;
        return false;
    }
    for(ui i = pstart[u2];i < pstart[u2+1];i ++) if(edgeValid[i] && edges[i] == u1) return true;
    return false;
}

void NearLinearPayAndTry::debugCheck() {
}