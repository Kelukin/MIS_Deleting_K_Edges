//
// Created by Kelukin on 2020/5/27.
//

#ifndef MIS_DELETING_K_EDGES_NOTIFIER_H
#define MIS_DELETING_K_EDGES_NOTIFIER_H
#include <stack>
#include "../Utility.h"
class Notifier {
protected:
    std::stack<ui> changedVertices;
public:
    Notifier(){}
    Notifier(Notifier &notifier) = delete;
    bool isEmpty();
    ui getItem();
    void addItem(ui x);
    ~Notifier(){}
};


#endif //MIS_DELETING_K_EDGES_NOTIFIER_H
