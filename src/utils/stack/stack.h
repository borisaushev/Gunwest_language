#ifndef ASM_CALC_STACK_H
#define ASM_CALC_STACK_H

#include <algorithm>

#include "common.h"

typedef struct characterProperty {
    const char* question;
    bool answer;
} characterProperty_t;

typedef characterProperty_t element_t;
# define REG "%d"

const int STACK_BASE_SIZE = 100;
const element_t POISON = {"777", false};
const char CANARRAY = 'W';

typedef struct stack {
    element_t* array;
    size_t capacity;
    size_t elementCount;
#ifdef ENABLE_PRT
    element_t* cnr1;
    element_t* cnr2;
    size_t hash;
#endif
} stack_t;


//Юзать относительные пути
const char* const STACK_LOG_FILE = "..\\files\\logs\\stack.log";
const char* const PROC_LOG_FILE = "..\\files\\logs\\proc.log";
const size_t MAX_REASONABLE_CAPACITY = 1024 * 1024 * 1024;

typedef enum errors {
    ST_SUCCESS = 0,
    ST_NULL_PTR,
    ST_INVALID_CAPACITY,
    ST_INVALID_INDEX,
    ST_INVALID_PTR,
    ST_CONTENTS_MODIFIED,
    ST_CANT_OPEN_FILE,
    ST_FILE_NOT_FOUND,
    ST_FILE_NOT_READABLE,
    ST_INVALID_INPUT
} error_t;

int stackDump(const stack_t *stack, const char *file, int line, const char *function, int validation);

#define STACK_VALID(stack) \
BEGIN \
    int valid ## __LINE__ = validateStack(stack); \
    if (valid ## __LINE__ != 0) { \
        SAFE_CALL(stackDump(stack, __FILE__, __LINE__, __func__, valid ## __LINE__)); \
    return valid ## __LINE__;\
    } \
END

void initStack(stack_t* stack, size_t capacity);

size_t getStackElementCount(stack_t* stack);

int stackPop(stack_t* stack, element_t* element);

int stackPush(stack_t *stack, element_t element);

int fprintStack(FILE* file, stack_t* stack);

int printStack(stack_t* stack);

int DPrintStack(stack_t* stack);

int stackDestroy(stack_t* stack);

int validateStack(const stack_t* stack);

int stackDumpStream(const stack_t *stack, int validation, FILE* dumpFile,
                        const char *file, int line, const char *function);

unsigned int djb2StrHash(const wchar_t *str);

unsigned int djb2Hash(const void *ptr, size_t size);

#endif //ASM_CALC_STACK_H