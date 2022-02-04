#pragma once
#include "tables.h"
#include "error.h"
#include <stdio.h>
#include <string.h>

#define MASTER_FILE_LOC "master.fl"
#define MASTER_IND_LOC "master.ind"
#define MASTER_GARBAGE_LOC "master_garbage"

bool checkIndexInBounds(FILE* mInd, int id) {
    fseek(mInd, 0, SEEK_END);
    long mIndSize = ftell(mInd);

    if (mIndSize <= 0 || (id * sizeof(long) > mIndSize)) {
        return false;
    }
    return true;
}

long retriveMasterAddr(FILE* mInd, int id) {
    long mAddr;
    fseek(mInd, (id - 1) * sizeof(long), SEEK_SET);
    fread(&mAddr, sizeof(long), 1, mInd);

    return mAddr;
}

struct Master retriveMasterFromAddr(FILE* mFile, long mAddr) {
    struct Master master;
    fseek(mFile, mAddr, SEEK_SET);
    fread(&master, sizeof(master), 1, mFile);

    return master;
}

struct Master retriveMaster(FILE* mInd, FILE* mFile, int id) {
    long mAddr = retriveMasterAddr(mInd, id);
    return retriveMasterFromAddr(mFile, mAddr);
}

void setMasterAtAddr(FILE* mFile, long mAddr, struct Master master) {
    fseek(mFile, mAddr, SEEK_SET);
    fwrite(&master, sizeof(master), 1, mFile);
}

int getGarbageCounter(FILE* mGarbage) {
    int garbageIdCount;
    fread(&garbageIdCount, sizeof(int), 1, mGarbage);
    return garbageIdCount;
}

void addIdToGarbage(FILE* mGarbage, int id) {
    // get garbage counter
    int garbageIdCount = getGarbageCounter(mGarbage);

    // add deleted id to garbage file
    fseek(mGarbage, (garbageIdCount + 1) * sizeof(int), SEEK_SET);
    fwrite(&id, sizeof(int), 1, mGarbage);

    // increment counter
    garbageIdCount = garbageIdCount + 1;
    fseek(mGarbage, 0, SEEK_SET);
    fwrite(&garbageIdCount, sizeof(int), 1, mGarbage);
}

int retriveAvailableIdFromGarbage(FILE* mGarbage) {
    int garbageIdCount = getGarbageCounter(mGarbage);

    // get available id
    int availableId;
    fseek(mGarbage, garbageIdCount * sizeof(int), SEEK_SET);
    fread(&availableId, sizeof(int), 1, mGarbage);

    // decrement garbage counter
    int decrementedId = garbageIdCount - 1;
    fseek(mGarbage, 0, SEEK_SET);
    fwrite(&decrementedId, sizeof(int), 1, mGarbage);

    return availableId;
}

long addNewAddrToIndexTable(FILE* mInd, FILE* mFile) {
    fseek(mFile, 0, SEEK_END);
    long mAddr = ftell(mFile);
    fwrite(&mAddr, sizeof(long), 1, mInd);
    return mAddr;
}

int retriveAvailableIdFromIndexTable(FILE* mInd) {
    fseek(mInd, 0, SEEK_END);
    int newId = (ftell(mInd) / sizeof(long)) + 1;
    return newId;
}

master_err_code_t getMaster(int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "rb");

    if (!mInd || !mFile) {
        return FILESYSTEM_ERROR;
    }

    if (!checkIndexInBounds(mInd, id)) {
        return INDEX_OUT_OF_BOUNDS;
    }

    struct Master master = retriveMaster(mInd, mFile, id);

    if (master.deleted) {
        return MASTER_DELETED;
    }

    printf("Master id: %i\n", master.manufacturerId);
    printf("Master name: %s\n", master.name);
    printf("Master country: %s\n", master.country);
    printf("Master website: %s\n", master.website);
    printf("Master year of foundation: %i\n", master.yearOfFoundation);

    fclose(mInd);
    fclose(mFile);
    return SUCCESS;
}

master_err_code_t insertMaster(struct Master master) {
    FILE* mInd = fopen(MASTER_IND_LOC, "r+b");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* mGarbage = fopen(MASTER_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !mGarbage)
        return FILESYSTEM_ERROR;

    master.deleted = 0;
    master.firstSlaveAddr = -1;

    int garbageIdCount = getGarbageCounter(mGarbage);

    if (garbageIdCount != 0) {
        // get available id
        int availableId = retriveAvailableIdFromGarbage(mGarbage);
        master.manufacturerId = availableId;

        // get master address
        long mAddr = retriveMasterAddr(mInd, availableId);

        // replace deleted master
        setMasterAtAddr(mFile, mAddr, master);
    }
    else {
        // get new id
        master.manufacturerId = retriveAvailableIdFromIndexTable(mInd);

        // store addr in index table
        long mAddr = addNewAddrToIndexTable(mInd, mFile);

        // store master in master file
        setMasterAtAddr(mFile, mAddr, master);
    }

    fclose(mInd);
    fclose(mFile);
    fclose(mGarbage);
    return SUCCESS;
}

master_err_code_t deleteMaster(int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* mGarbage = fopen(MASTER_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !mGarbage)
        return FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, id)) {
        return INDEX_OUT_OF_BOUNDS;
    }

    long mAddr = retriveMasterAddr(mInd, id);

    struct Master master = retriveMasterFromAddr(mFile, mAddr);

    if (master.deleted) {
        return MASTER_DELETED;
    }

    // save marked as deleted
    master.deleted = true;
    setMasterAtAddr(mFile, mAddr, master);

    addIdToGarbage(mGarbage, id);

    fclose(mInd);
    fclose(mFile);
    fclose(mGarbage);

    return SUCCESS;
}

master_err_code_t updateMaster(struct Master master) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");

    if (!mInd || !mFile)
        return FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, master.manufacturerId)) {
        return INDEX_OUT_OF_BOUNDS;
    }

    long mAddr = retriveMasterAddr(mInd, master.manufacturerId);

    // get master from address to check if deleted
    struct Master oldMaster = retriveMasterFromAddr(mFile, mAddr);

    if (oldMaster.deleted) {
        return MASTER_DELETED;
    }

    // id cannot be changed
    master.manufacturerId = oldMaster.manufacturerId;

    // update master
    setMasterAtAddr(mFile, mAddr, master);

    fclose(mInd);
    fclose(mFile);
    return SUCCESS;
}