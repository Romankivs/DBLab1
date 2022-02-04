#pragma once
#include "tables.h"
#include "error.h"
#include "master.h"

#define SLAVE_FILE_LOC "slave.fl"
#define SLAVE_GARBAGE_LOC "slave_garbage"

struct Slave retriveSlaveFromAddr(FILE* sFile, long sAddr) {
    fseek(sFile, sAddr, SEEK_SET);
    fread(&sAddr, sizeof(struct Slave), 1, sFile);
}

void setSlaveAtAttr(FILE* sFile, struct Slave slave, long sAddr) {
    fseek(sFile, sAddr, SEEK_SET);
    fwrite(&slave, sizeof(struct Slave), 1, sFile);
}

int getSlaveGarbageCounter(FILE* sGarbage) {
    long garbageCount;
    fread(&garbageCount, sizeof(long), 1, sGarbage);
    return garbageCount;
}

void addAddrToGarbage(FILE* sGarbage, long addr) {
    // get garbage counter
    int garbageCount = getGarbageCounter(sGarbage);

    // add deleted id to garbage file
    fseek(sGarbage, (garbageCount + 1) * sizeof(long), SEEK_SET);
    fwrite(&addr, sizeof(long), 1, sGarbage);

    // increment counter
    garbageCount = garbageCount + 1;
    fseek(sGarbage, 0, SEEK_SET);
    fwrite(&garbageCount, sizeof(long), 1, sGarbage);
}

long findAvailableAddr(FILE* sFile, FILE* sGarbage) {
    long sAddr;
    long garbageCount = getSlaveGarbageCounter(sGarbage);
    if (garbageCount != 0) {
        fseek(sGarbage, garbageCount * sizeof(long), SEEK_SET);
        fread(&sAddr, sizeof(long), 1, sGarbage);

        garbageCount = garbageCount - 1;
        fseek(sGarbage, 0, SEEK_SET);
        fwrite(&garbageCount, sizeof(long), 1, sGarbage);
    }
    else {
        fseek(sFile, 0, SEEK_END);
        sAddr = ftell(sFile);
    }
    return sAddr;
}

slave_err_code_t getSlave(int masterId, int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "rb");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "rb");

    if (!mInd || !mFile || !sFile)
        return S_FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, masterId)) {
        return S_INDEX_OUT_OF_BOUNDS;
    }

    struct Master master = retriveMaster(mInd, mFile, masterId);

    if (master.deleted) {
        return S_MASTER_DELETED;
    }

    struct Slave slave;
    long nextSlaveAddr = master.firstSlaveAddr;
    bool found = false;
    while (nextSlaveAddr != -1 && !found) {
        slave = retriveSlaveFromAddr(sFile, nextSlaveAddr);

        if(slave.carId == id) {
            found = true;
        }

        nextSlaveAddr = slave.nextSlave;
    }

    if (!found)
        return S_NOT_FOUND;

    printf("Slave id: %i\n", slave.carId);
    printf("Slave release year: %i\n", slave.releaseYear);
    printf("Slave model name: %s\n", slave.modelName);
    printf("Slave number of seats: %i\n", slave.numberOfSeats);

    fclose(mInd);
    fclose(mFile);
    fclose(sFile);
    return S_SUCCESS;
}

slave_err_code_t insertSlave(int masterId, struct Slave slave) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGarbage = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !sFile || !sGarbage)
        return S_FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, masterId)) {
        return S_INDEX_OUT_OF_BOUNDS;
    }

    struct Master master = retriveMaster(mInd, mFile, masterId);

    if (master.deleted) {
        return S_MASTER_DELETED;
    }

    slave.nextSlave = -1;

    long sAddr = findAvailableAddr(sFile, sGarbage);

    slave.carId = (sAddr / sizeof(slave)) + 1;
    printf("ID: %i\n", slave.carId);

    setSlaveAtAttr(sFile, slave, sAddr);

    if (master.firstSlaveAddr == -1) { // if first slave added
        master.firstSlaveAddr = sAddr;
        long mAddr = retriveMasterAddr(mInd, masterId);
        setMasterAtAddr(mFile, mAddr, master);
    }
    else {
        struct Slave slave;
        long nextSlaveAddr = master.firstSlaveAddr;
        while (true) {
            slave = retriveSlaveFromAddr(nextSlaveAddr);

            if (slave.nextSlave == -1)
                break;

            nextSlaveAddr = slave.nextSlave;
        }

        slave.nextSlave = sAddr;
        setSlaveAtAttr(sFile, slave, nextSlaveAddr);
    }

    fclose(mInd);
    fclose(mFile);
    fclose(sFile);
    fclose(sGarbage);
    return S_SUCCESS;
}

slave_err_code_t deleteSlave(int masterId, int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGarbage = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !sFile || !sGarbage)
        return S_FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, masterId)) {
        return S_INDEX_OUT_OF_BOUNDS;
    }

    struct Master master = retriveMaster(mInd, mFile, masterId);

    if (master.deleted) {
        return S_MASTER_DELETED;
    }

    struct Slave slave;
    long prevAddr = -1;
    long nextSlaveAddr = master.firstSlaveAddr;
    bool found = false;
    while (nextSlaveAddr != -1 && !found) {
        slave = retriveSlaveFromAddr(sFile, nextSlaveAddr);

        if(slave.carId == id) {
            found = true;
            break;
        }
        prevAddr = nextSlaveAddr;
        nextSlaveAddr = slave.nextSlave;
    }

    if (!found)
        return S_NOT_FOUND;

    struct Slave prevSlave;
    prevSlave = retriveSlaveFromAddr(sFile, prevAddr);

    prevSlave.nextSlave = slave.nextSlave;
    setSlaveAtAttr(sFile, prevSlave, prevAddr);

    // add to garbage
    addAddrToGarbage(sGarbage, nextSlaveAddr);

    fclose(mInd);
    fclose(mFile);
    fclose(sFile);
    fclose(sGarbage);
    return S_SUCCESS;
}

slave_err_code_t updateSlave(int masterId, struct Slave updatedSlave) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");

    if (!mInd || !mFile || !sFile)
        return S_FILESYSTEM_ERROR;

    if (!checkIndexInBounds(mInd, masterId)) {
        return S_INDEX_OUT_OF_BOUNDS;
    }

    struct Master master = retriveMaster(mInd, mFile, masterId);

    if (master.deleted) {
        return S_MASTER_DELETED;
    }

    struct Slave slave;
    long nextSlaveAddr = master.firstSlaveAddr;
    bool found = false;
    while (nextSlaveAddr != -1) {
        slave = retriveSlaveFromAddr(sFile, nextSlaveAddr);

        if(slave.carId == updatedSlave.carId) {
            found = true;
            break;
        }

        nextSlaveAddr = slave.nextSlave;
    }

    if (!found)
        return S_NOT_FOUND;

    updatedSlave.carId = slave.carId;
    setSlaveAtAttr(sFile, updatedSlave, nextSlaveAddr);

    fclose(mInd);
    fclose(mFile);
    fclose(sFile);
    return S_SUCCESS;
}