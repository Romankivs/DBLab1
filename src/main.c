#include <stdio.h>
#include <string.h>
#include "slave.h"
#include "master.h"
#include "print.h"
#include "input.h"

void help() {
    printf("Available commands:\n");
    printf("get-m get-s\n");
    printf("insert-m insert-s\n");
    printf("delete-m delete-s\n");
    printf("update-m update-s\n");
    printf("garb-m garb-s\n");
}

void getM() {
    int mId = enterMasterId();
    struct Master master;
    err_code_t err = getMaster(&master, mId);
    if (err)
        printf("%s\n", errToStr(err));
    else
        printMaster(master);
}

void insertM() {
    struct Master master = enterMaster();
    err_code_t err = insertMaster(master);
    printf("%s\n", errToStr(err));
}

void deleteM() {
    int mId = enterMasterId();
    err_code_t err = deleteMaster(mId);
    printf("%s\n", errToStr(err));
}

void updateM() {
    struct Master master = enterMaster();
    err_code_t err = updateMaster(master);
    printf("%s\n", errToStr(err));
}

void utilM() {
    int mId = enterMasterId();
    struct Master master;
    err_code_t err = getMaster(&master, mId);
    if (err)
        printf("%s\n", errToStr(err));
    else
        printMasterUtil(master);
}

void garbM() {
    FILE* gb = fopen(MASTER_GARBAGE_LOC, "rb");
    printGarbage(gb);
    fclose(gb);
}

void getS() {
    int mId = enterMasterId();
    int sId = enterSlaveId();
    struct Slave slave;
    err_code_t err = getSlave(&slave, mId, sId);
    if (err)
        printf("%s\n", errToStr(err));
    else
        printSlave(slave);
}

void insertS() {
    int mId = enterMasterId();
    struct Slave slave = enterSlave();
    err_code_t err = insertSlave(mId, slave);
    printf("%s\n", errToStr(err));
}

void deleteS() {
    int mId = enterMasterId();
    int sId = enterSlaveId();
    err_code_t err = deleteSlave(mId, sId);
    printf("%s\n", errToStr(err));
}

void updateS() {
    int mId = enterMasterId();
    struct Slave slave = enterSlave();
    err_code_t err = updateSlave(mId, slave);
    printf("%s\n", errToStr(err));
}

void utilS() {
    int mId = enterMasterId();
    int sId = enterSlaveId();
    struct Slave slave;
    err_code_t err = getSlave(&slave, mId, sId);
    if (err)
        printf("%s\n", errToStr(err));
    else
        printSlaveUtil(slave);
}

void garbS() {
    FILE* gb = fopen(SLAVE_GARBAGE_LOC, "rb");
    printGarbage(gb);
    fclose(gb);
}

int main() {
    FILE* ind = fopen(MASTER_IND_LOC, "r+b");
    FILE* file = fopen(MASTER_FILE_LOC, "r+b");
    FILE* gb = fopen(MASTER_GARBAGE_LOC, "r+b");
    FILE* sFile = fopen(SLAVE_FILE_LOC, "r+b");
    FILE* sGb = fopen(SLAVE_GARBAGE_LOC, "r+b");

    if (!ind || !file || !gb || !sFile || !sGb) {
        ind = fopen(MASTER_IND_LOC, "w+b");
        file = fopen(MASTER_FILE_LOC, "w+b");
        gb = fopen(MASTER_GARBAGE_LOC, "w+b");
        sFile = fopen(SLAVE_FILE_LOC, "w+b");
        sGb = fopen(SLAVE_GARBAGE_LOC, "w+b");
    }

    long garbageIdCount = 0;
    fseek(gb, 0, SEEK_END);
    if (ftell(gb) == 0) // populate if necessary
        fwrite(&garbageIdCount, sizeof(long), 1, gb);
    fseek(sGb, 0, SEEK_END);
    if (ftell(sGb) == 0) // populate if necessary
        fwrite(&garbageIdCount, sizeof(long), 1, sGb);

    fclose(ind); fclose(file); fclose(gb); fclose(sFile); fclose(sGb);

    printf("Enter \"help\" to see available commands\n");
    bool running = true;
    while(running) {
        printf(">");
        char command[20];
        scanf("%20s", &command);
        if (!strcmp(command, "quit")) {
            running = false;
        }
        else if (!strcmp(command, "help")) {
            help();
        }
        else if (!strcmp(command, "get-m")) {
            getM();
        }
        else if (!strcmp(command, "insert-m")) {
            insertM();
        }
        else if (!strcmp(command, "delete-m")) {
            deleteM();
        }
        else if (!strcmp(command, "update-m")) {
            updateM();
        }
        else if (!strcmp(command, "util-m")) {
            utilM();
        }
        else if (!strcmp(command, "garb-m")) {
            garbM();
        }
        else if (!strcmp(command, "get-s")) {
            getS();
        }
        else if (!strcmp(command, "insert-s")) {
            insertS();
        }
        else if (!strcmp(command, "delete-s")) {
            deleteS();
        }
        else if (!strcmp(command, "update-s")) {
            updateS();
        }
        else if (!strcmp(command, "util-s")) {
            utilS();
        }
        else if (!strcmp(command, "garb-s")) {
            garbS();
        }
    }
    return 0;
}
