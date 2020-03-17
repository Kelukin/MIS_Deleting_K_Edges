//
// Created by Kelukin on 2020/3/16.
//
#include "Utility.h"

FILE *open_file(const char *filename, const char *mode){
    FILE *f = fopen(filename, mode);
    if(f == NULL){
        printf("Cannot open file %s!\n", filename);
        exit(1);
    }

    return f;
}
