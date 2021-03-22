//
// Created by Kelukin on 2021/3/22.
//

#ifndef MIS_DELETING_K_EDGES_LOGWRITER_H
#define MIS_DELETING_K_EDGES_LOGWRITER_H
#include "../Utility.h"

extern std::string LOG_PATH;
class LogWriter {
private:
    static FILE* fp;
public:
    FILE* getFilePointer(){
        if(fp != nullptr)   return fp;
        fp = fopen(LOG_PATH.c_str(), "w");
        return fp;
    }
};


#endif //MIS_DELETING_K_EDGES_LOGWRITER_H
