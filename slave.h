#pragma once
#include "tables.h"
#include "error.h"
#include "master.h"
#include "garbage.h"

#define SLAVE_FILE_LOC "slave.fl"
#define SLAVE_GARBAGE_LOC "slave_garbage"

struct Slave retriveSlaveFromAddr(FILE* sFile, long sAddr);

void setSlaveAtAttr(FILE* sFile, struct Slave slave, long sAddr);

err_code_t getSlave(struct Slave* res, int masterId, int id);

err_code_t insertSlave(int masterId, struct Slave slave);

err_code_t deleteSlave(int masterId, int id);

err_code_t updateSlave(int masterId, struct Slave updatedSlave);