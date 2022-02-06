#pragma once
#include "tables.h"

void emptyConsole() {
    char c;
    while((c = getchar()) != '\n' && c != EOF);
}

struct Master enterMaster() {
    struct Master res;
    emptyConsole();
    printf("Enter id:\n");
    scanf("%i", &res.manufacturerId);
    emptyConsole();
    printf("Enter name:\n");
    scanf("%20s", &res.name);
    emptyConsole();
    printf("Enter country:\n");
    scanf("%20s", &res.country);
    emptyConsole();
    printf("Enter website:\n");
    scanf("%20s", &res.website);
    emptyConsole();
    printf("Enter year of foundation:\n");
    scanf("%i", &res.yearOfFoundation);
    emptyConsole();
    return res;
}

int enterMasterId() {
    int res;
    emptyConsole();
    printf("Enter master id\n");
    scanf("%i", &res);
    return res;
}

struct Slave enterSlave() {
    struct Slave res;
    emptyConsole();
    printf("Enter id:\n");
    scanf("%i", &res.carId);
    emptyConsole();
    printf("Enter release year:\n");
    scanf("%i", &res.releaseYear);
    emptyConsole();
    printf("Enter model name:\n");
    scanf("%20s", &res.modelName);
    emptyConsole();
    printf("Enter number of seats:\n");
    scanf("%i", &res.numberOfSeats);
    emptyConsole();
    return res;
}

int enterSlaveId() {
    int res;
    emptyConsole();
    printf("Enter slave id\n");
    scanf("%i", &res);
    return res;
}