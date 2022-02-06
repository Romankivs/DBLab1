#pragma once
#include "tables.h"
#include "error.h"
#include "garbage.h"
#include "slave.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MASTER_FILE_LOC "master.fl"
#define MASTER_IND_LOC "master.ind"
#define MASTER_GARBAGE_LOC "master_garbage"

bool checkIndexInBounds(FILE* mInd, int id);

int compareIndexRow(const void* arg1, const void* arg2);

err_code_t retriveMasterAddr(long* res, FILE* mInd, int id);

struct Master retriveMasterFromAddr(FILE* mFile, long mAddr);

void setMasterAtAddr(FILE* mFile, long mAddr, struct Master master);

void addNewIndexRowToIndexTable(FILE* mInd, struct IndexRow newRow);

err_code_t getMaster(struct Master* res, int id);

err_code_t insertMaster(struct Master master);

err_code_t deleteMaster(int id);

err_code_t updateMaster(struct Master master);