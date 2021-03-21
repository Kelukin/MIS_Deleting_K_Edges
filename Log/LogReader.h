//
// Created by Kelukin on 2021/3/22.
//

#ifndef MIS_DELETING_K_EDGES_LOGREADER_H
#define MIS_DELETING_K_EDGES_LOGREADER_H

#include "../Utility.h"
extern std::string LOG_PATH;
class LogReader {
private:
    static FILE* fp;
public:
    FILE* getFileReader(){
        if(fp != nullptr)   return fp;
        fp = fopen(LOG_PATH.c_str(), "r");
        return fp;
    }
};


#endif //MIS_DELETING_K_EDGES_LOGREADER_H
