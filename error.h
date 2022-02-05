#pragma once

typedef enum err_code {
    SUCCESS = 0,
    FILESYSTEM_ERROR,
    INDEX_OUT_OF_BOUNDS,
    MASTER_ID_ALREADY_TAKEN,
    MASTER_DELETED,
    MASTER_NOT_FOUND,
    SLAVE_NOT_FOUND
} err_code_t;

const char* const ERR_CODE_STR[] = {
        "Success",
        "Filesystem error",
        "Index out of bounds",
        "Master id already taken",
        "Master deleted",
        "Master not found",
        "Slave not found"
};

const char* errToStr(err_code_t err) {
    return ERR_CODE_STR[err];
}