#pragma once
#include "master.h"
#include "slave.h"

void printMaster(struct Master master) {
    printf("Master id: %i\n", master.manufacturerId);
    printf("Master name: %s\n", master.name);
    printf("Master country: %s\n", master.country);
    printf("Master website: %s\n", master.website);
    printf("Master year of foundation: %i\n", master.yearOfFoundation);
}

void printMasterUtil(struct Master master) {
    printMaster(master);
    printf("Master first slave address: %i\n", master.firstSlaveAddr);
    printf("Master is deleted: %i", master.deleted);
}

void printSlave(struct Slave slave) {
    printf("Slave id: %i\n", slave.carId);
    printf("Slave release year: %i\n", slave.releaseYear);
    printf("Slave model name: %s\n", slave.modelName);
    printf("Slave number of seats: %i\n", slave.numberOfSeats);
}

void printSlaveUtil(struct Slave slave) {
    printSlave(slave);
    printf("Slave next slave address: %i\n", slave.nextSlave);
}

void printGarbage(FILE* garbage) {
    long counter = getGarbageCounter(garbage);
    long addr;
    for (int i = 1; i <= counter; ++i) {
        fseek(garbage, i * sizeof(long), SEEK_SET);
        fread(&addr, sizeof(long), 1, garbage);
        printf("%li", addr);
        if (i != counter)
            printf(", ");
    }
    printf("\n");
}