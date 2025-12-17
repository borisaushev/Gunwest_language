#include "asmwrite.h"

#include "common.h"
#include "treeDump.h"
#include "treeSctruct.h"

wchar_t getReg(treeNode_t * param) {
    assert(param);
    assert(param->nodeType == PARAM_TYPE);

    int index = getParameterIndex(param->data.parameter);
    return (wchar_t) (L'A' + index);
}

static int writeNumberOperation(treeNode_t *node) {
    switch (getData(node).operation) {
        case TD_PLUS: {
            wprintf(L"ADD\n");
            break;
        }
        case TD_MINUS: {
            wprintf(L"SUB\n");
            break;
        }
        case TD_MULTIPLY: {
            wprintf(L"MUL\n");
            break;
        }
        case TD_DIVIDE: {
            wprintf(L"DIV\n");
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

int writeNumberExpression(treeNode_t* node) {
    assert(node);
    TREE_DUMP(node, "writing Number Expression", DSL_SUCCESS);
    switch (node->nodeType) {
        case NUMBER_TYPE: {
            wprintf(L"PUSH %d\n", getNumber(node));
            return DSL_SUCCESS;
        }
        case OPERATION_TYPE: {
            SAFE_CALL(writeNumberExpression(getLeft(node)));
            SAFE_CALL(writeNumberExpression(getRight(node)));
            SAFE_CALL(writeNumberOperation(node));
            return DSL_SUCCESS;
        }
        case PARAM_TYPE: {
            wprintf(L"PUSHREG %lcX\n", getReg(node));
            return DSL_SUCCESS;
        }
        default: {
            PRINTERR("invalid node type");
            return DSL_INVALID_INPUT;
        }
    }
    return DSL_SUCCESS;
}

static int writeValue(treeNode_t* node) {
    if (getData(node).operation == TD_INPUT) {
        wprintf(L"IN\n");
    }
    else if (getData(node).operation == TD_SQRT) {
        SAFE_CALL(writeValue(getLeft(node)));
        wprintf(L"SQRT\n");
    }
    else {
        SAFE_CALL(writeNumberExpression(node));
    }
    return DSL_SUCCESS;
}

static int writeIf(treeNode_t *node, int &label) {
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

    SAFE_CALL(writeNumberExpression(getLeft(condition)));
    SAFE_CALL(writeNumberExpression(getRight(condition)));

    int labelVal = label++;
    switch (getData(condition).operation) {
        case TD_EQUALS: {
            wprintf(L"JNE");
            break;
        }
        case TD_NOT_EQUALS: {
            wprintf(L"JE");
            break;
        }
        case TD_LESS_THAN: {
            wprintf(L"JAE");
            break;
        }
        default: {
            RETURN_ERR(DSL_INVALID_INPUT, "invalid operation");
        }
    }
    wprintf(L" :IF_END_%d\n", labelVal);
    SAFE_CALL(writeAsm(code));
    wprintf(L":IF_END_%d\n", labelVal);

    return DSL_SUCCESS;
}

static int writeDeclaration(treeNode_t *node) {
    treeNode_t* value = getRight(node);
    treeNode_t* parameter = getLeft(node);
    if (value == NULL) {
        PRINTERR("expected value in declaration\n");
        return DSL_INVALID_INPUT;
    }
    if (parameter == NULL) {
        PRINTERR("expected parameter in declaration\n");
        return DSL_INVALID_INPUT;
    }
    SAFE_CALL(writeValue(value));
    wprintf(L"POPREG %lcX\n", getReg(parameter));

    return DSL_SUCCESS;
}

int writeOperation(treeNode_t *node) {
    switch (getData(node).operation) {
        case TD_PRINT: {
            treeNode_t* param = getLeft(node);
            if (param == NULL || param->nodeType != PARAM_TYPE) {
                PRINTERR("invalid print argument");
                return DSL_INVALID_INPUT;
            }
            wprintf(L"PUSHREG %lcX\n", getReg(param));
            wprintf(L"OUT\n");
            break;
        }
        case TD_HLT: {
            wprintf(L"HLT\n");
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

static int writeExpression(treeNode_t *node, int &label) {
    switch (getData(node).expressionType) {
        case TD_DECLARATION: {
            SAFE_CALL(writeDeclaration(node));
            break;
        }
        case TD_IFS: {
            SAFE_CALL(writeIf(node, label));
            break;
        }
        default: {
            PRINTERR("invalid expression type");
            return DSL_INVALID_INPUT;
        }
    }

    return DSL_SUCCESS;
}

int writeAsm(treeNode_t* node) {
    static int label = 0;
    if (node == NULL) {
        return DSL_SUCCESS;
    }
    TREE_DUMP(node, "writing asm", DSL_SUCCESS);
    switch (node->nodeType) {
        case LINKER_TYPE: {
            SAFE_CALL(writeAsm(getRight(node)));
            SAFE_CALL(writeAsm(getLeft(node)));
            break;
        }
        case OPERATION_TYPE: {
            SAFE_CALL(writeOperation(node));
            break;
        }
        case EXPRESSION_TYPE: {
            SAFE_CALL(writeExpression(node, label));
            break;
        }
        default: {
            PRINTERR("invalid operation");
            return DSL_INVALID_INPUT;
        }
    }
    return DSL_SUCCESS;
}
