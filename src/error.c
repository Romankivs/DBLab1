#include "error.h"

const char* const ERR_CODE_STR[] = {
        "Success",
        "Filesystem error",
        "Index out of bounds",
        "Master id already taken",
        "Slave id already taken",
        "Master deleted",
        "Master not found",
        "Slave not found"
};

const char* errToStr(err_code_t err) {
    return ERR_CODE_STR[err];
}