#include "garbage.h"

#pragma once
#include <stdio.h>

long getGarbageCounter(FILE* mGarbage) {
    long garbageIdCount;
    fseek(mGarbage, 0, SEEK_SET);
    fread(&garbageIdCount, sizeof(long), 1, mGarbage);
    return garbageIdCount;
}

void addAddrToGarbage(FILE* garbage, long addr) {
    // get garbage counter
    long garbageCount = getGarbageCounter(garbage);

    // add deleted addr to garbage file
    fseek(garbage, (garbageCount + 1) * sizeof(long), SEEK_SET);
    fwrite(&addr, sizeof(long), 1, garbage);

    // increment counter
    garbageCount = garbageCount + 1;
    fseek(garbage, 0, SEEK_SET);
    fwrite(&garbageCount, sizeof(long), 1, garbage);
}

long findAvailableAddr(FILE* file, FILE* garbage) {
    long mAddr;
    long garbageCount = getGarbageCounter(garbage);
    if (garbageCount > 0) {
        fseek(garbage, garbageCount * sizeof(long), SEEK_SET);
        fread(&mAddr, sizeof(long), 1, garbage);

        garbageCount = garbageCount - 1;
        fseek(garbage, 0, SEEK_SET);
        fwrite(&garbageCount, sizeof(long), 1, garbage);
    }
    else {
        fseek(file, 0, SEEK_END);
        mAddr = ftell(file);
    }
    return mAddr;
}