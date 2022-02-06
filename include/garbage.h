#pragma once
#include <stdio.h>

long getGarbageCounter(FILE* mGarbage);

void addAddrToGarbage(FILE* garbage, long addr);

long findAvailableAddr(FILE* file, FILE* garbage);