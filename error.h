#pragma once

typedef enum master_err_code {
    SUCCESS = 0,
    FILESYSTEM_ERROR,
    INDEX_OUT_OF_BOUNDS,
    MASTER_DELETED
} master_err_code_t;

const char* const MASTER_ERR_CODE_STR[] = {
        "Success",
        "Filesystem error",
        "Index out of bounds",
        "Master deleted"
};

const char* mErrToStr(master_err_code_t err) {
    return MASTER_ERR_CODE_STR[err];
}

typedef enum slave_err_code {
    S_SUCCESS = 0,
    S_FILESYSTEM_ERROR,
    S_INDEX_OUT_OF_BOUNDS,
    S_MASTER_DELETED,
    S_NOT_FOUND
} slave_err_code_t;

const char* const SLAVE_ERR_CODE_STR[] = {
        "Success",
        "Filesystem error",
        "Index out of bounds",
        "Slave deleted",
        "Slave not found"
};

const char* sErrToStr(master_err_code_t err) {
    return SLAVE_ERR_CODE_STR[err];
};