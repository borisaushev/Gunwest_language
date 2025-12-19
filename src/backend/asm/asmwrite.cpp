#include "asmwrite.h"
#include "common.h"
#include "treeDump.h"
#include "treeSctruct.h"
#include <wchar.h>

int label = 0;

static wchar_t getReg(treeNode_t * param) {
    assert(param);
    assert(param->nodeType == PARAM_TYPE);

    int index = getParameterIndex(param->data.parameter);
    return (wchar_t) (L'A' + index);
}

static int writeNumberOperation(FILE* outFile, treeNode_t *node) {
    switch (getData(node).operation) {
        case TD_PLUS: {
            fwprintf(outFile, L"ADD\n");
            break;
        }
        case TD_MINUS: {
            fwprintf(outFile, L"SUB\n");
            break;
        }
        case TD_MULTIPLY: {
            fwprintf(outFile, L"MUL\n");
            break;
        }
        case TD_DIVIDE: {
            fwprintf(outFile, L"DIV\n");
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

static int writeNumberExpression(FILE* outFile, treeNode_t* node) {
    assert(node);
    TREE_DUMP(node, "writing Number Expression", DSL_SUCCESS);

    switch (node->nodeType) {
        case NUMBER_TYPE: {
            fwprintf(outFile, L"PUSH %d\n", getNumber(node));
            return DSL_SUCCESS;
        }
        case OPERATION_TYPE: {
            SAFE_CALL(writeNumberExpression(outFile, getLeft(node)));
            SAFE_CALL(writeNumberExpression(outFile, getRight(node)));
            SAFE_CALL(writeNumberOperation(outFile, node));
            return DSL_SUCCESS;
        }
        case PARAM_TYPE: {
            fwprintf(outFile, L"PUSHREG %lcX\n", getReg(node));
            return DSL_SUCCESS;
        }
        default: {
            PRINTERR("invalid node type");
            return DSL_INVALID_INPUT;
        }
    }
    return DSL_SUCCESS;
}

static int writeValue(FILE* outFile, treeNode_t* node) {
    if (getData(node).operation == TD_INPUT) {
        fwprintf(outFile, L"IN\n");
    }
    else if (getData(node).operation == TD_SQRT) {
        SAFE_CALL(writeValue(outFile, getLeft(node)));
        fwprintf(outFile, L"SQRT\n");
    }
    else {
        SAFE_CALL(writeNumberExpression(outFile, node));
    }
    return DSL_SUCCESS;
}

static int writeIf(FILE* outFile, treeNode_t *node) {
    treeNode_t* condition = getLeft(node);
    treeNode_t* code = getRight(node);
    if (condition == NULL) {
        PRINTERR("expected condition in if\n");
        return DSL_INVALID_INPUT;
    }
    if (code == NULL) {
        PRINTERR("expected code in if\n");
        return DSL_INVALID_INPUT;
    }

    fwprintf(outFile, L"\n; if condition:\n");
    SAFE_CALL(writeNumberExpression(outFile, getLeft(condition)));
    SAFE_CALL(writeNumberExpression(outFile, getRight(condition)));

    int labelVal = label++;
    switch (getData(condition).operation) {
        case TD_EQUALS: {
            fwprintf(outFile, L"JNE");
            break;
        }
        case TD_NOT_EQUALS: {
            fwprintf(outFile, L"JE");
            break;
        }
        case TD_LESS_THAN: {
            fwprintf(outFile, L"JAE");
            break;
        }
        default: {
            PRINTERR("invalid operation: %d\n", getData(condition).operation);
            RETURN_ERR(DSL_INVALID_INPUT, "invalid operation");
        }
    }
    fwprintf(outFile, L" :IF_END_%d\n", labelVal);

    fwprintf(outFile, L"; if code:\n");
    SAFE_CALL(writeAsmToFile(outFile, code));
    fwprintf(outFile, L":IF_END_%d\n\n", labelVal);

    return DSL_SUCCESS;
}

static int writeWhile(FILE* outFile, treeNode_t *node) {
    treeNode_t* condition = getLeft(node);
    treeNode_t* code = getRight(node);
    if (condition == NULL) {
        PRINTERR("expected condition in while\n");
        return DSL_INVALID_INPUT;
    }
    if (code == NULL) {
        PRINTERR("expected code in while\n");
        return DSL_INVALID_INPUT;
    }

    int labelVal = label++;
    fwprintf(outFile, L"\n:WHILE_START_%d\n", labelVal);
    fwprintf(outFile, L";while condition\n");
    SAFE_CALL(writeNumberExpression(outFile, getLeft(condition)));
    SAFE_CALL(writeNumberExpression(outFile, getRight(condition)));
    switch (getData(condition).operation) {
        case TD_EQUALS: {
            fwprintf(outFile, L"JNE");
            break;
        }
        case TD_NOT_EQUALS: {
            fwprintf(outFile, L"JE");
            break;
        }
        case TD_LESS_THAN: {
            fwprintf(outFile, L"JAE");
            break;
        }
        default: {
            RETURN_ERR(DSL_INVALID_INPUT, "invalid operation");
        }
    }
    fwprintf(outFile, L" :WHILE_END_%d\n", labelVal);
    fwprintf(outFile, L";while code:\n");
    SAFE_CALL(writeAsmToFile(outFile, code));
    fwprintf(outFile, L"JMP :WHILE_START_%d\n", labelVal);
    fwprintf(outFile, L":WHILE_END_%d\n\n", labelVal);

    return DSL_SUCCESS;
}

static int writeDeclaration(FILE* outFile, treeNode_t *node) {
    treeNode_t* value = getRight(node);
    treeNode_t* parameter = getLeft(node);

    fwprintf(outFile, L"\n; declaration:\n");
    if (value == NULL) {
        PRINTERR("expected value in declaration\n");
        return DSL_INVALID_INPUT;
    }
    if (parameter == NULL) {
        PRINTERR("expected parameter in declaration\n");
        return DSL_INVALID_INPUT;
    }
    SAFE_CALL(writeValue(outFile, value));
    fwprintf(outFile, L"POPREG %lcX\n", getReg(parameter));

    return DSL_SUCCESS;
}

static int writeOperation(FILE* outFile, treeNode_t *node) {
    switch (getData(node).operation) {
        case TD_PRINT: {
            treeNode_t* param = getLeft(node);
            if (param == NULL || param->nodeType != PARAM_TYPE) {
                PRINTERR("invalid print argument");
                return DSL_INVALID_INPUT;
            }
            fwprintf(outFile, L"PUSHREG %lcX\n", getReg(param));
            fwprintf(outFile, L"OUT\n");
            break;
        }
        case TD_HLT: {
            fwprintf(outFile, L"HLT\n");
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

static int writeExpression(FILE* outFile, treeNode_t *node) {
    switch (getData(node).expressionType) {
        case TD_DECLARATION: {
            SAFE_CALL(writeDeclaration(outFile, node));
            break;
        }
        case TD_IF_EXPRESSION_TYPE: {
            SAFE_CALL(writeIf(outFile, node));
            break;
        }
        case TD_WHILE_EXPRESSION_TYPE: {
            SAFE_CALL(writeWhile(outFile, node));
            break;
        }
        default: {
            PRINTERR("invalid expression type");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

int writeAsmToFile(FILE* outFile, treeNode_t* node) {
    if (outFile == NULL) {
        PRINTERR("Output file is NULL\n");
        return DSL_INVALID_INPUT;
    }

    if (node == NULL) {
        return DSL_SUCCESS;
    }

    TREE_DUMP(node, "writing asm to file", DSL_SUCCESS);

    switch (node->nodeType) {
        case LINKER_TYPE: {
            SAFE_CALL(writeAsmToFile(outFile, getRight(node)));
            SAFE_CALL(writeAsmToFile(outFile, getLeft(node)));
            break;
        }
        case OPERATION_TYPE: {
            SAFE_CALL(writeOperation(outFile, node));
            break;
        }
        case EXPRESSION_TYPE: {
            SAFE_CALL(writeExpression(outFile, node));
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }
    return DSL_SUCCESS;
}

int writeAsm(const char* filename, treeNode_t* node) {
    FILE* outFile = fopen(filename, "w");
    if (outFile == NULL) {
        PRINTERR("Cannot open file %s for writing\n", filename);
        return DSL_FILE_NOT_FOUND;
    }

    SAFE_CALL(writeAsmToFile(outFile, node));
    fclose(outFile);

    return DSL_SUCCESS;
}