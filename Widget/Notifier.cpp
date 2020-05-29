//
// Created by Kelukin on 2020/5/27.
//

#include "Notifier.h"
bool Notifier::isEmpty() {
    return changedVertices.empty();
}

ui Notifier::getItem(){
    assert(!changedVertices.empty());
    ui x =  changedVertices.top();
    changedVertices.pop();
    return x;
}

void Notifier::addItem(ui x){
    changedVertices.push(x);
}
