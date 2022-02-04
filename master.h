#pragma once
#include "tables.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void setMasterAtId(FILE* mInd, FILE* mFile, int id, struct Master master) {
    long mAddr = retriveMasterAddr(mInd, id);
    setMasterAtAddr(mFile, mAddr, master);
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

err_code_t getMaster(struct Master* res, int id) {
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

    memcpy(res, &master, sizeof(struct Master));

    fclose(mInd); fclose(mFile);
    return SUCCESS;
}

err_code_t insertMaster(struct Master master) {
    FILE* mInd = fopen(MASTER_IND_LOC, "r+b");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* mGarbage = fopen(MASTER_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !mGarbage)
        return FILESYSTEM_ERROR;

    master.deleted = 0;
    master.firstSlaveAddr = -1;

    int garbageIdCount = getGarbageCounter(mGarbage);

    if (garbageIdCount != 0) {
        int availableId = retriveAvailableIdFromGarbage(mGarbage);
        master.manufacturerId = availableId;

        setMasterAtId(mInd, mFile, availableId, master);
    }
    else {
        master.manufacturerId = retriveAvailableIdFromIndexTable(mInd);

        long mAddr = addNewAddrToIndexTable(mInd, mFile);

        setMasterAtAddr(mFile, mAddr, master);
    }

    fclose(mInd); fclose(mFile); fclose(mGarbage);
    return SUCCESS;
}

err_code_t deleteMaster(int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* mGarbage = fopen(MASTER_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !mGarbage)
        return FILESYSTEM_ERROR;

    struct Master master;
    err_code_t  err = getMaster(&master, id);
    if (err != SUCCESS)
        return err;

    // save marked as deleted
    master.deleted = true;
    setMasterAtId(mInd, mFile, id, master);

    addIdToGarbage(mGarbage, id);

    fclose(mInd); fclose(mFile); fclose(mGarbage);
    return SUCCESS;
}

err_code_t updateMaster(struct Master master) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");

    if (!mInd || !mFile)
        return FILESYSTEM_ERROR;

    struct Master oldMaster;
    err_code_t err = getMaster(&oldMaster, master.manufacturerId);
    if (err != SUCCESS)
        return err;

    master.manufacturerId = oldMaster.manufacturerId;
    master.deleted = oldMaster.deleted;
    master.firstSlaveAddr = oldMaster.firstSlaveAddr;

    setMasterAtId(mInd, mFile, master.manufacturerId, master);

    fclose(mInd); fclose(mFile);
    return SUCCESS;
}