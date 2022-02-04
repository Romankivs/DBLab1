#pragma once
#include <stdbool.h>

struct Master {
    int manufacturerId;
    char name[20];
    char country[20];
    char website[20];
    int yearOfFoundation;

    long firstSlaveAddr;

    bool deleted;
};

struct Slave {
    int carId;
    int releaseYear;
    char modelName[20];
    int numberOfSeats;

    long nextSlave;
};
