//
// Created by Kelukin on 2020/5/27.
//

#include "RiskPayAndTry.h"

void RiskPayAndTry::addDominated(ui u, ui v) {
    if(edgeDeleteIndex->hasDeletedEdge(u)){
        if(dominate[u]) return;
        dominate[u] = 1;
        inRisk[u] = 0;
        dominated.push_back(u);
    }else if(edgeDeleteIndex->hasDeletedEdge(v)){
        //has risk
        if(!inRisk[u] && !dominate[u]){
            inRisk[u] = 1;
            riskDominated.push_back(u);
        }
    }else{
        if(dominate[u]) return;
        inRisk[u] = 0;
        dominate[u] = 1;
        dominated.push_front(u);
    }
}

int RiskPayAndTry::delete_vertex_dominate(ui u) {
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
                if(tri[j] + 1 == degree[v] ){
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


    while(!notifier->isEmpty()){
        ui u = notifier->getItem();
        shrink(u, pend[u]);
        for(ui i = pstart[u]; i < pend[u]; ++i){
            if(edgeValid && !edgeValid[i]) continue;
            ui v = edges[i];
            if(degree[u] + 1 == tri[i]){
                addDominated(v, u);
            }
        }
    }
    return res;
}

void RiskPayAndTry::init(){
    NearLinearPayAndTry::init();
    edgeDeleteIndex->setNotifier(notifier);
}

bool RiskPayAndTry::canRiskReduction() {
    return !riskDominated.empty();
}

void RiskPayAndTry::riskReduction() {
    while(!riskDominated.empty()){
        ui u = riskDominated.back();
        riskDominated.pop_back();
        if(!is[u] || degree[u] == 0)    continue;
        if(!inRisk[u]) continue;
        if(!dominated_check(u)){
            dominate[u] = 0;
            inRisk[u] = 0;
        }
        else{
            delete_vertex_dominate(u);
            break;
        }
    }
}

bool RiskPayAndTry::canSafeReduction() {
    return !dominated.empty() || (!degree_twos.empty() && riskDominated.empty());
}

void RiskPayAndTry::safe_reduction() {
    while(!dominated.empty()){
        ui u = dominated.back();
        dominated.pop_back();
        if(!is[u] || degree[u] == 0)    continue;
        if(edgeDeleteIndex->hasDeletedEdge(u)){
            dominate[u] = 0;
            if(!inRisk[u]){
                inRisk[u] = 1;
                riskDominated.push_back(u);
            }
            continue;
        }
        if(!dominated_check(u)) dominate[u] = 0;
        else delete_vertex_dominate(u);
    }
    if(++reduct_cnt > 10){
        reduct_cnt = 0;
        while(!riskDominated.empty())   riskReduction();
    }
    degree2Reduction();
//    if(riskDominated.empty())
}

void RiskPayAndTry::clearRisk() {
    while(!riskDominated.empty()) riskReduction();
}

void RiskPayAndTry::degree2Reduction(){
    while(!degree_twos.empty()&&dominated.empty() && riskDominated.empty()) {
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
            if(dominated.empty() && riskDominated.empty()){
                printf("error %d\n", u);
            }
            assert(!dominated.empty() || !riskDominated.empty());
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
            assert(!dominated.empty() || !riskDominated.empty());
            continue;
        }

        if(cnt%2 == 1) {
            if(exist_edge(u1,u2)) {
                delete_vertex_dominate(u1);
                delete_vertex_dominate(u2);
                if(dominated.empty() && riskDominated.empty()){
                    printf("error %d\n", u);
                }
                assert(!dominated.empty() || !riskDominated.empty());
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
                        if(is[edges[k]]&&tri[k]+1 == degree[u1]) {
                            addDominated(edges[k], u1);
                        }
                        edgeDeleteIndex->descDiffValue(reverseEdge[k]);
                    }
                for(ui k = pstart[u2];k < pend[u2];k ++) if(edgeValid[k]){
                        if(is[edges[k]]&&tri[k]+1 == degree[u2]) {;
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