//
// Created by lj on 2020/9/18.
//


#include "ProbabilityPayAndTry.h"
extern bool OPEN_COST_FUNCTION;
int ProbabilityPayAndTry::compute_triangle_counts() {
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

void ProbabilityPayAndTry::init() {
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
        printf("The Index built begin.");
        edgeDeleteIndex = new  ProbabilityDeleteIndex(n, new_m, max_d, edges, is, reverseEdge, pstart, pend, degree);
        // the two optional way to calculate probability
        // with or without knowing  about the query set
        if(!OPEN_COST_FUNCTION) edgeDeleteIndex->calProbability();
        else edgeDeleteIndex->calProbability(given_set, gs_length);
        printf("The Index built ends.");
    }
    else edgeDeleteIndex = nullptr;
}



int ProbabilityPayAndTry::pay() {
    if(k == 0 || edgeDeleteIndex->empty())  return - 1;
    ui x, y;
    while(!edgeDeleteIndex->empty()){
        auto p = edgeDeleteIndex->recommendEdge();
        if(!is[p.first] || !is[p.second])   continue;
        x = p.second;
        y = p.first;
        bool flag = false;
        for(ui i = pstart[x]; i < pend[x]; ++i)
            if(edges[i] == y){
                if(!edgeValid[i])   break;
                else{
                    flag = true;
                    deleteEdge(i, x, y);
                    assert(!edgeValid[i]);
                }
            }
        if(flag){
            k -= 1;
//            printf("K : %d\n", k);
            return 0;
        }
    }

    return -1;
}

void ProbabilityPayAndTry::safe_reduction() {
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
                    }
                for(ui k = pstart[u2];k < pend[u2];k ++) if(edgeValid[k]){
                        if(is[edges[k]]&&!dominate[edges[k]]&&tri[k]+1 == degree[u2]) {
                            addDominated(edges[k], u2);
                        }
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

void ProbabilityPayAndTry::deleteEdge(int edgeNo, int u, int v) {
    if(!edgeValid[edgeNo]) return;
    edgeValid[edgeNo] = edgeValid[reverseEdge[edgeNo]] = false;
    edgeDeleteIndex->deleteEdge(edgeNo);
    edgeDeleteIndex->deleteEdge(reverseEdge[edgeNo]);
    --degree[u];
    --degree[v];
    if(degree[u] == 2)  degree_twos.push_back(u);
    if(degree[v] == 2)  degree_twos.push_back(v);
    for(ui i = pstart[v]; i < pend[v]; ++i) if(edgeValid[i]) adj[edges[i]] = 1;
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(!edgeValid[i] || !is[edges[i]]) continue;
        if(adj[edges[i]]){
            --tri[i];
            --tri[reverseEdge[i]];

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
        } else if(degree[v] - 1 == tri[i]){
            if(!dominate[edges[i]]){
                addDominated(edges[i], v);
            }
        }
    }
    for(ui i = pstart[u]; i < pend[u]; ++i) adj[edges[i]] = 0;

}
ui ProbabilityPayAndTry::edge_rewire(ui u, const ui *pend, ui v, ui w) {
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

void ProbabilityPayAndTry::shrink(ui u, ui &end) {
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

void ProbabilityPayAndTry::addDominated(ui u, ui v) {
    dominate[u] = 1;
    if(edgeDeleteIndex->hasDeletedEdge(u)){
        // in high priority
        dominated.push_back(u);
    }
    else dominated.push_front(u);
}

void ProbabilityPayAndTry::update_triangle(ui u1, ui u2) {
    int cnt = 0;
    shrink(u1, pend[u1]);
    shrink(u2, pend[u2]);

    for(ui i = pstart[u1];i < pend[u1];i ++) adj[edges[i]] = 1;

    for(ui i = pstart[u2];i < pend[u2];i ++) if(adj[edges[i]]) {
            ui v = edges[i];
            tri[i] ++;
            ++ cnt;
            if(tri[i]+1 == degree[u2]&&!dominate[v]) {
                addDominated(v, u2);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u2]) {
                addDominated(u2, v);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u2) {
                    tri[j] ++;
                    break;
                }
        }

    for(ui i = pstart[u1];i < pend[u1];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u2){
            tri[i] = cnt;
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
            if(tri[i]+1 == degree[u1]&&!dominate[v]) {
                addDominated(v, u1);
            }
            if(tri[i]+1 == degree[v]&&!dominate[u1]) {
                addDominated(u1, v);
            }

            for(ui j = pstart[v];j < pend[v];j ++) if(edges[j] == u1) {
                    tri[j] ++;
                    break;
                }
        }

    for(ui i = pstart[u2];i < pend[u2];i ++) {
        adj[edges[i]] = 0;
        if(edges[i] == u1){
            tri[i] = cnt;
        }
    }
}


int ProbabilityPayAndTry::delete_vertex_dominate(ui u) {
    shrink(u, pend[u]);//certain the range in [pstart[u], pend[u]) must be valid
    is[u] = 0;
    k += edgeDeleteIndex->deleteVertex(u);
    int res = 0;
    for(ui i = pstart[u]; i < pend[u]; ++i){
        if(edgeValid && !edgeValid[i]) continue;
        ui v = edges[i];
        adj[v] = 1;
        -- degree[v];
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