#pragma once

typedef enum err_code {
    SUCCESS = 0,
    FILESYSTEM_ERROR,
    INDEX_OUT_OF_BOUNDS,
    MASTER_ID_ALREADY_TAKEN,
    SLAVE_ID_ALREADY_TAKEN,
    MASTER_DELETED,
    MASTER_NOT_FOUND,
    SLAVE_NOT_FOUND
} err_code_t;


const char* errToStr(err_code_t err);