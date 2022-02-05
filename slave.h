#pragma once
#include "tables.h"
#include "error.h"
#include "master.h"
#include "garbage.h"

#define SLAVE_FILE_LOC "slave.fl"
#define SLAVE_GARBAGE_LOC "slave_garbage"

struct Slave retriveSlaveFromAddr(FILE* sFile, long sAddr) {
    struct Slave res;
    fseek(sFile, sAddr, SEEK_SET);
    fread(&res, sizeof(struct Slave), 1, sFile);
    return res;
}

void setSlaveAtAttr(FILE* sFile, struct Slave slave, long sAddr) {
    fseek(sFile, sAddr, SEEK_SET);
    fwrite(&slave, sizeof(struct Slave), 1, sFile);
}

err_code_t getSlave(struct Slave* res, int masterId, int id) {
    FILE* sFile = fopen(SLAVE_FILE_LOC, "rb");
    if (!sFile)
        return FILESYSTEM_ERROR;

    struct Master master;
    err_code_t err = getMaster(&master, masterId);
    if (err != SUCCESS)
        return err;

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
        return SLAVE_NOT_FOUND;

    memcpy(res, &slave, sizeof(struct Slave));

    fclose(sFile);
    return SUCCESS;
}

err_code_t insertSlave(int masterId, struct Slave slave) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGarbage = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (!mInd || !mFile || !sFile || !sGarbage)
        return FILESYSTEM_ERROR;

    long mAddr;
    err_code_t err = retriveMasterAddr(&mAddr, mInd, masterId);
    if (err != SUCCESS)
        return err;

    struct Master master;
    err = getMaster(&master, masterId);
    if (err != SUCCESS)
        return err;

    slave.nextSlave = -1;

    long sAddr = findAvailableAddr(sFile, sGarbage);

    slave.carId = (sAddr / sizeof(slave)) + 1;
    printf("ID: %i\n", slave.carId);

    setSlaveAtAttr(sFile, slave, sAddr);

    if (master.firstSlaveAddr == -1) {
        master.firstSlaveAddr = sAddr;
        setMasterAtAddr(mFile, mAddr, master);
    }
    else {
        struct Slave slave;
        long nextSlaveAddr = master.firstSlaveAddr;
        while (true) {
            slave = retriveSlaveFromAddr(sFile, nextSlaveAddr);
            if (slave.nextSlave == -1)
                break;

            nextSlaveAddr = slave.nextSlave;
        }

        slave.nextSlave = sAddr;
        setSlaveAtAttr(sFile, slave, nextSlaveAddr);
    }

    fclose(mInd); fclose(mFile); fclose(sFile); fclose(sGarbage);
    return SUCCESS;
}

err_code_t deleteSlave(int masterId, int id) {
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGarbage = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (!sFile || !sGarbage)
        return FILESYSTEM_ERROR;

    struct Master master;
    err_code_t err = getMaster(&master, masterId);
    if (err != SUCCESS)
        return err;

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
        return SLAVE_NOT_FOUND;

    struct Slave prevSlave;
    prevSlave = retriveSlaveFromAddr(sFile, prevAddr);

    prevSlave.nextSlave = slave.nextSlave;
    setSlaveAtAttr(sFile, prevSlave, prevAddr);

    addAddrToGarbage(sGarbage, nextSlaveAddr);

    fclose(sFile); fclose(sGarbage);
    return SUCCESS;
}

err_code_t updateSlave(int masterId, struct Slave updatedSlave) {
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");

    if (!sFile)
        return FILESYSTEM_ERROR;

    struct Master master;
    err_code_t err = getMaster(&master, masterId);
    if (err != SUCCESS)
        return err;

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
        return SLAVE_NOT_FOUND;

    updatedSlave.carId = slave.carId;
    setSlaveAtAttr(sFile, updatedSlave, nextSlaveAddr);

    fclose(sFile);
    return SUCCESS;
}