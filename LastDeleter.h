//
// Created by Kelukin on 2020/4/8.
//

#include "Utility.h"
#ifndef MIS_DELETING_K_EDGES_LASTDELETER_H
#define MIS_DELETING_K_EDGES_LASTDELETER_H


class LastDeleter {
protected:
    LastDeleter(){};
    static LastDeleter* lastDeleter;
public:
    static LastDeleter* getLastDeleter(){
        if(lastDeleter == nullptr){
            lastDeleter = new LastDeleter();
        }
        return lastDeleter;
    };
    LastDeleter(const LastDeleter &obj) = delete;
    LastDeleter operator = (const LastDeleter &obj) = delete;
    static int greedy_delete_edges(char *is, ui *pstart, ui *edges, ui k, ui n);
};


#endif //MIS_DELETING_K_EDGES_LASTDELETER_H
