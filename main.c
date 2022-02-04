#include <stdio.h>
#include "slave.h"
#include "master.h"
#include "print.h"

int main() {
    struct Master m = {1, "BM", "Germany", "bmw.com",
                       1999, -1, 0, false};

    struct Master m2 = {1, "CCTV", "Germany", "bmw.com",
                       1999, -1, 0, false};

    struct Slave s = {1, 1999, "FIAT", 4};
    struct Slave s2 = {3, 1999, "JEEP", 4};

    FILE* ind = fopen(MASTER_IND_LOC, "r+b");
    FILE* file = fopen(MASTER_FILE_LOC, "r+b");
    FILE* gb = fopen(MASTER_GARBAGE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGb = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (gb == 0) {
        ind = fopen(MASTER_IND_LOC, "w+b");
        file = fopen(MASTER_FILE_LOC, "w+b");
        gb = fopen(MASTER_GARBAGE_LOC, "w+b");
        sFile = fopen(SLAVE_FILE_LOC, "w+b");
        sGb = fopen(SLAVE_GARBAGE_LOC, "w+b");
    }

    int garbageIdCount = 0;
    fseek(gb, 0, SEEK_END);
    if (ftell(gb) == 0) // populate if necessary
        fwrite(&garbageIdCount, sizeof(int), 1, gb);
    fseek(sGb, 0, SEEK_END);
    if (ftell(sGb) == 0) // populate if necessary
        fwrite(&garbageIdCount, sizeof(int), 1, sGb);

    fclose(ind);
    fclose(file);
    fclose(gb);
    fclose(sFile);
    fclose(sGb);

    insertMaster(m);
    insertMaster(m2);

    insertSlave(1, s);
    insertSlave(1, s);
    insertSlave(1, s);
    deleteSlave(1, 2);
    insertSlave(1, s2);
    //updateSlave(1, s2);

    struct Slave slave;
    err_code_t  sRes = getSlave(&slave, 1, 4);
    if (sRes != 0) printf("%s\n", errToStr(sRes));
    printSlaveUtil(slave);
    struct Master mRes;
    err_code_t res = getMaster(&mRes, 1);
    if (res != 0) printf("%s", errToStr(res));
    printMasterUtil(mRes);
    return 0;
}
