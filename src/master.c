#include "master.h"

int compareIndexRow(const void* arg1, const void* arg2) {
    const struct IndexRow* first = (const struct IndexRow*)arg1;
    const struct IndexRow* second = (const struct IndexRow*)arg2;
    if (first->index < second->index) {
        return -1;
    }
    if (first->index > second->index) {
        return 1;
    }
    return 0;
}

err_code_t retriveMasterAddr(long* res, FILE* mInd, int id) {
    if (id <= 0)
        return INDEX_OUT_OF_BOUNDS;

    fseek(mInd, 0, SEEK_END);
    long indTableSize = ftell(mInd);
    struct IndexRow* indTable = malloc(indTableSize);
    fseek(mInd, 0, SEEK_SET);
    fread(indTable, indTableSize, 1, mInd);

    struct IndexRow rowToFind = {id, 0};
    struct IndexRow* found = bsearch(&rowToFind, indTable,
                                     indTableSize / sizeof(struct IndexRow),
                                     sizeof(struct IndexRow), compareIndexRow);

    if (found == NULL)
        return MASTER_NOT_FOUND;

    memcpy(res, &found->addr, sizeof(long));
    return SUCCESS;
}

struct Master retriveMasterFromAddr(FILE* mFile, long mAddr) {
    struct Master master;
    fseek(mFile, mAddr, SEEK_SET);
    fread(&master, sizeof(master), 1, mFile);

    return master;
}

void setMasterAtAddr(FILE* mFile, long mAddr, struct Master master) {
    fseek(mFile, mAddr, SEEK_SET);
    fwrite(&master, sizeof(master), 1, mFile);
}

void addNewIndexRowToIndexTable(FILE* mInd, struct IndexRow newRow) {
    fseek(mInd, 0, SEEK_END);
    long indTableSize = ftell(mInd);
    struct IndexRow* indTable = malloc(indTableSize + sizeof(struct IndexRow));
    fseek(mInd, 0, SEEK_SET);
    fread(indTable, indTableSize, 1, mInd);

    memcpy((indTable + indTableSize / sizeof(struct IndexRow)),
           &newRow, sizeof(struct IndexRow));
    qsort(indTable, indTableSize / sizeof(struct IndexRow), sizeof(struct IndexRow), compareIndexRow);

    fclose(mInd);
    mInd = fopen(MASTER_IND_LOC, "w+b");
    fwrite(indTable, indTableSize + sizeof(struct IndexRow), 1, mInd);
}

err_code_t getMaster(struct Master* res, int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "rb");

    if (!mInd || !mFile) {
        return FILESYSTEM_ERROR;
    }

    long mAddr;
    err_code_t err = retriveMasterAddr(&mAddr, mInd, id);
    if (err != SUCCESS)
        return err;

    struct Master master = retriveMasterFromAddr(mFile, mAddr);

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

    long mAddr = findAvailableAddr(mFile, mGarbage);
    struct IndexRow newRow = {master.manufacturerId, mAddr};

    struct Master check;
    if (getMaster(&check, master.manufacturerId) == SUCCESS)
        return MASTER_ID_ALREADY_TAKEN;
    addNewIndexRowToIndexTable(mInd, newRow);

    setMasterAtAddr(mFile, mAddr, master);

    fclose(mInd); fclose(mFile); fclose(mGarbage);
    return SUCCESS;
}

err_code_t deleteMaster(int id) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");
    FILE* mGarbage = fopen(MASTER_GARBAGE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");

    if (!mInd || !mFile || !mGarbage)
        return FILESYSTEM_ERROR;

    long mAddr;
    err_code_t  err = retriveMasterAddr(&mAddr, mInd, id);
    if (err != SUCCESS)
        return err;

    struct Master master;
    err = getMaster(&master, id);
    if (err != SUCCESS)
        return err;

    // save marked as deleted
    master.deleted = true;
    setMasterAtAddr(mFile, mAddr, master);

    addAddrToGarbage(mGarbage, mAddr);

    long nextSlaveAddr = master.firstSlaveAddr;
    while (nextSlaveAddr != -1) {
        struct Slave slave = retriveSlaveFromAddr(sFile, nextSlaveAddr);
        nextSlaveAddr = slave.nextSlave;
        deleteSlave(id, slave.carId);
    }

    fclose(mInd); fclose(mFile); fclose(mGarbage); fclose(sFile);
    return SUCCESS;
}

err_code_t updateMaster(struct Master master) {
    FILE* mInd = fopen(MASTER_IND_LOC, "rb");
    FILE* mFile = fopen(MASTER_FILE_LOC, "r+b");

    if (!mInd || !mFile)
        return FILESYSTEM_ERROR;

    long mAddr;
    err_code_t  err = retriveMasterAddr(&mAddr, mInd, master.manufacturerId);
    if (err != SUCCESS)
        return err;

    struct Master oldMaster;
    err = getMaster(&oldMaster, master.manufacturerId);
    if (err != SUCCESS)
        return err;

    master.manufacturerId = oldMaster.manufacturerId;
    master.deleted = oldMaster.deleted;
    master.firstSlaveAddr = oldMaster.firstSlaveAddr;

    setMasterAtAddr(mFile, mAddr, master);

    fclose(mInd); fclose(mFile);
    return SUCCESS;
}