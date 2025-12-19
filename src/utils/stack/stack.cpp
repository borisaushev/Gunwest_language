#include "stack.h"

#include <cassert>
#include <cstdio>

int validateStack(const stack_t* stack) {
    // DPRINTF("=== STACK VALIDATION STARTED ===\n");

    if (stack == NULL) {
        RETURN_ERR(ST_NULL_PTR, "null stack");
    }
    if (stack->array == NULL) {
        RETURN_ERR(ST_NULL_PTR, "null array");
    }

    if (stack->capacity > MAX_REASONABLE_CAPACITY) {
        RETURN_ERR(ST_INVALID_CAPACITY, "stack size is too big");
    }

    // Check index bounds
    if ((size_t)stack->elementCount > stack->capacity || stack->elementCount > MAX_REASONABLE_CAPACITY) {
        RETURN_ERR(ST_INVALID_INDEX, "index too big");
    }

#ifdef ENABLE_PRT
    assert(stack->cnr1);
    assert(stack->cnr2);
    if (*(stack->cnr1) != CANARRAY) {
        RETURN_ERR(CONTENTS_MODIFIED, "first canarray is modified");
    }
    if (*(stack->cnr2) != CANARRAY) {
        RETURN_ERR(CONTENTS_MODIFIED, "second canarray is modified");
    }
#endif

    // DPRINTF("=== STACK VALIDATION PASSED ===\n");
    return ST_SUCCESS;
}

static void dumpData(FILE* output, const stack_t *stack) {
    assert(stack);
    assert(output);

#ifdef ENABLE_PRT
    fprintf(output, "\t\t\tcnr1: ptr: '%p' val: '%c' or '%d'; cnr2: ptr: '%p' val: '%c' or '%d'\n",
        stack->cnr1, *stack->cnr1, *stack->cnr1, stack->cnr2, *stack->cnr2, *stack->cnr2);
#endif

    fprintf(output, "\t\t\tcapacity: %llu\n", stack->capacity);
    if (stack->array == NULL) {
        fprintf(output, "\t\t\t[NULL DATA POINTER!]\n");
    } else if (stack->elementCount <= 0) {
        fprintf(output, "\t\t\t[empty]\n");
    } else {
        if ((size_t) stack->elementCount > stack->capacity) {
            fprintf(output, "\t\t\t--- MEMORY CORRUPTION DETECTED ---\n");
            size_t max = stack->elementCount > MAX_REASONABLE_CAPACITY ? 10 : stack->elementCount;
            for (size_t i = 0; i < max; i++) {
                fprintf(output, "\t\t\t**el[%zu]: " REG "\n", i, stack->array[i]);
            }
        }
        else {
            for (size_t i = 0; i < stack->elementCount; i++) {
            }
            for (size_t i = stack->elementCount; i < stack->capacity; i++) {
            }
        }
    }
}

int stackDump(const stack_t *stack,const char *file, int line,
                  const char *function, int validation) {
    static FILE* dumpFile = NULL;
    if (dumpFile == NULL) {
        dumpFile = fopen(STACK_LOG_FILE, "wa");
    }
    if (dumpFile == NULL) {
        // DPRINTF("StackDump() {\n");
        // DPRINTF("\tCannot open dump file!\n");

        RETURN_ERR(ST_CANT_OPEN_FILE, "dump file not found");
    }

    stackDumpStream(stack, validation, dumpFile, file, line, function);

    fclose(dumpFile);

    return ST_SUCCESS;
}

int stackDumpStream(const stack_t *stack, int validation, FILE* dumpFile,
                        const char *file, int line, const char *function) {
    assert(dumpFile);

    fprintf(dumpFile, "StackDump() {\n");

    if (stack == NULL) {
        fprintf(dumpFile, "\tStack [NULL] { ERROR! NULL POINTER\n\t}\n}\n");
        fclose(dumpFile);
        RETURN_ERR(ST_NULL_PTR, "null pointer in dump");
    }

    const char *status = (validation == ST_SUCCESS) ? "" : "  ERROR! ";

    fprintf(dumpFile, "\t%s  [%p] {%s  \n\t\t\tfrom %s() at %s:%d\n",
            "stack", stack, status, function, file, line);

    fprintf(dumpFile, "\t\telements count = %zu", stack->elementCount);

    if (stack->elementCount > stack->capacity) {
        fprintf(dumpFile, " (SIZE EXCEEDS CAPACITY!)");
    }
    fprintf(dumpFile, ";\n");

    fprintf(dumpFile, "\t\tdata[%p] {\n", (void*)stack->array);

    dumpData(dumpFile, stack);

    fprintf(dumpFile, "\t\t}\n\t}\n}\n");

    return ST_SUCCESS;
}

int DPrintStack(stack_t* stack) {
    assert(stack);

    STACK_VALID(stack);

    DPRINTF("stack {\n");
    for (size_t i = 0; i < stack->elementCount; i++) {
        DPRINTF("\tel[%llu] = ", i);
        DPRINTF(REG, stack->array[i]);
        DPRINTF("\n");
    }
    DPRINTF("}\n");
    fflush(stdout);

    return ST_SUCCESS;
}

void initStack(stack_t* stack, size_t capacity) {
    assert(stack);
    DPRINTF("initializing stack\n");
    stack->capacity = capacity;
    stack->elementCount = 0;

#ifdef ENABLE_PRT
    element_t* mem = (element_t*) calloc(2 + capacity, sizeof(element_t)); //защита канарейкой

    stack->cnr1 = mem;
    stack->cnr2 = mem + stack->capacity + 1;
    *(stack->cnr1) = CANARRAY;
    *(stack->cnr2) = CANARRAY;
    stack->array = (mem+1);
#else
    stack->array = (element_t*) calloc(capacity, sizeof(element_t));
#endif

    for (size_t i = 0; i < capacity; i++) {
        stack->array[i] = POISON;
    }

    DPRINTF("after init: \n");
    DPrintStack(stack);

    DPRINTF("stack init with poison values and canarrays\n");
}

size_t getStackElementCount(stack_t* stack) {
    return stack->elementCount;
}

int stackPop(stack_t* stack, element_t* element) {
    assert(stack);
    assert(element);

    STACK_VALID(stack);

    if (stack->elementCount == 0) {
        DPRINTF("stack empty, returning poison\n");
        *element = POISON;
        return ST_SUCCESS;
    }

    element_t result = stack->array[stack->elementCount - 1];
    stack->array[stack->elementCount - 1] = POISON;
    stack->elementCount--;

    STACK_VALID(stack);

    *element = result;

    // DPRINTF("after pop: \n");
    // DPRINTF("elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);
    // DPrintStack(stack);

    return ST_SUCCESS;
}

int stackPush(stack_t *stack, element_t element) {
    assert(stack);

    STACK_VALID(stack);

    if (stack->elementCount >= stack->capacity) {
        size_t newCapacity = stack->capacity * 2 + 1;
        DPRINTF("stack full, reallocating to size: %llu\n", newCapacity);
        stack->capacity = newCapacity;

        #ifdef ENABLE_PRT
            element_t* mem = (element_t*) realloc(stack->cnr1, (newCapacity + 2)*sizeof(element_t)); //защита канарейкой

            stack->cnr1 = mem;
            stack->cnr2 = mem + stack->capacity + 1;
            *(stack->cnr1) = CANARRAY;
            *(stack->cnr2) = CANARRAY;
            stack->array = (mem+1);
        #else
            stack->array = (element_t*) realloc(stack->array, newCapacity*sizeof(element_t));
        #endif

        for (size_t i = stack->elementCount; i < newCapacity; i++) {
            stack->array[i] = POISON;
        }
    }

    stack->array[stack->elementCount] = element;
    stack->elementCount++;

    STACK_VALID(stack);


    // DPRINTF("after push: \n");
    // DPRINTF("elcount: %llu; capacity: %llu\n", stack->elementCount, stack->capacity);
    // DPrintStack(stack);

    return ST_SUCCESS;
}

int printStack(stack_t* stack) {
    SAFE_CALL(fprintStack(stdout, stack));

    return ST_SUCCESS;
}

int fprintStack(FILE* file, stack_t* stack) {
    assert(stack);

    STACK_VALID(stack);

    fflush(file);
    fprintf(file, "stack {\n");
    for (size_t i = 0; i < stack->elementCount; i++) {
        fprintf(file, "    el[%llu] = ", i);
        fprintf(file, REG, stack->array[i]);
        fprintf(file, "\n");
    }
    fprintf(file, "}\n");
    fflush(file);

    return ST_SUCCESS;
}

int stackDestroy(stack_t* stack) {
    assert(stack);
    #ifdef ENABLE_PRT
        assert(stack->cnr1);
        free(stack->cnr1);
    #else
        assert(stack->array);
        free(stack->array);
    #endif

    return ST_SUCCESS;
}

unsigned int djb2StrHash(const wchar_t *str) {
    unsigned int hash = 5381;
    for (size_t i = 0; i < wcslen(str); i++) {
        hash = ((hash << 5) + hash) + towlower(str[i]);
    }

    return hash;
}


unsigned int djb2Hash(const void *ptr, size_t size) {
    unsigned int hash = 5381;
    for (size_t i = 0; i < size; i++) {
        hash = ((hash << 5) + hash) + ((const unsigned char*) ptr)[i];
    }

    return hash;
}