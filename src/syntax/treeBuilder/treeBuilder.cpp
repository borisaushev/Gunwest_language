#include "treeBuilder.h"

#include "common.h"
#include "input.h"
#include "treeDump.h"

treeNode_t* getPrimaryExpression(TDtokenContext_t* context);
treeNode_t* getNumberExpression(TDtokenContext_t* context);


TDtoken_t* getCurToken(TDtokenContext_t* context) {
    assert(context);
    return context->tokens[context->index];
}

TDtoken_t* nextToken(TDtokenContext_t* context) {
    assert(context);
    return context->tokens[context->index++];
}

void dumpContext(TDtokenContext_t* context) {
    printf("Context:\n    [");
    for (int i = 0; context->tokens[i] != NULL; i++) {
        if (i == context->index) {
            printf("{");
        }

        bool found = false;
        for (int j = 0; j < TD_TOKENS_INFO_SIZE; j++) {
            if (TD_TOKENS_INFO[j].tokenType == context->tokens[i]->type) {
                wprintf(L"%ls", TD_TOKENS_INFO[j].representation);
                found = true;
                break;
            }
        }
        if (found) {
        }
        else if (context->tokens[i]->type == TD_NUMBER) {
            printf("%g", context->tokens[i]->value.number);
        }
        else if (context->tokens[i]->type == TD_STRING) {
            wprintf(L"%s", context->tokens[i]->value.str);
        }
        else {
            PRINTERR("bbq chicken alert");
        }

        if (i == context->index) {
            printf("}");
        }
        printf(", ");
    }
    printf("]\n");
}

treeNode_t* getNumber(TDtokenContext_t* context) {
    assert(context);
    if (getCurToken(context)->type != TD_NUMBER) {
        DPRINTF("wasnt able to parse number\n");
        return NULL;
    }
    DPRINTF("parsed number: %d\n", (int) getCurToken(context)->value.number);
    return createNumber((int) nextToken(context)->value.number);
}

static treeNode_t* getParameter(TDtokenContext_t* context) {
    assert(context);
    if (getCurToken(context)-> type != TD_STRING) {
        DPRINTF("wasnt able to parse parameter\n");
        return NULL;
    }

    DPRINTF("parsed parameter: %ls\n", getCurToken(context)->value.str);
    return createParameter(nextToken(context)->value.str);
}

static treeNode_t* getBlockExpression(TDtokenContext_t* context) {
    assert(context);
    treeNode_t* result = getNumber(context);
    if (result != NULL) {
        dumpContext(context);
        return result;
    }

    if (getCurToken(context)->type == TD_OPENING_BRACKET) {
        nextToken(context);
        DPRINTF("parsed left bracket, parsing new number expression\n");
        treeNode_t* expression = getNumberExpression(context);
        if (expression == NULL) {
            DPRINTF("wasnt able to parse number expression\n");
            dumpContext(context);
            return NULL;
        }
        if (getCurToken(context)->type != TD_CLOSING_BRACKET) {
            PRINTERR("Expected closing bracket ')'. Invalid character at %s:%d:%d\n",
                 TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
            return NULL;
        }
        DPRINTF("parsed block expression as numberExpression, closed bracket\n");
        nextToken(context);
        dumpContext(context);
        return expression;
    }

    result = getParameter(context);
    if (result == NULL) {
        PRINTERR("Expected number, variable or expression. Invalid character at %s:%d:%d\n",
                 TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }

    DPRINTF("parsed block expression as parameter\n");
    dumpContext(context);
    return result;
}

treeNode_t* getNumberExpression(TDtokenContext_t* context) {
    assert(context);
    treeNode_t* result = getPrimaryExpression(context);
    if (result == NULL) {
        DPRINTF("unable to parse primaryExpression in numberExpression\n");
        dumpContext(context);
        return NULL;
    }
    dumpContext(context);
    while (getCurToken(context) != NULL &&
        (getCurToken(context)->type == TD_PLUS || getCurToken(context)->type == TD_MINUS)) {
        TDtokenType_t tokenType = getCurToken(context)->type;
        nextToken(context);
        dumpContext(context);
        DPRINTF("parsing another primaryExpression in NumberExpression\n");
        treeNode_t* expression = getPrimaryExpression(context);
        if (expression == NULL) {
            DPRINTF("unable to parse primaryExpression in NumberExpression\n");
            return NULL;
        }
        DPRINTF("parsed another primaryExpression in NumberExpression\n");
        dumpContext(context);

        if (tokenType) {
            result = createOperation(NODE_ADD, result, expression);
        }
        else {
            result = createOperation(NODE_SUB, result, expression);
        }
        dumpContext(context);
    }
    DPRINTF("parsed number expression\n");

    return result;
}

treeNode_t* getPrimaryExpression(TDtokenContext_t* context) {
    assert(context);
    DPRINTF("parsing primary expression\n");
    treeNode_t* result = getBlockExpression(context);
    if (result == NULL) {
        DPRINTF("unable to parse block expression in PrimaryExpression\n");
        return NULL;
    }
    while (getCurToken(context) != NULL &&
        (getCurToken(context)->type == TD_MULTIPLY || getCurToken(context)->type == TD_DIVIDE)) {
        TDtokenType_t tokenType = getCurToken(context)->type;
        nextToken(context);
        dumpContext(context);
        DPRINTF("parsing another blockExpression in PrimaryExpression\n");
        treeNode_t* expression = getBlockExpression(context);
        if (expression == NULL) {
            DPRINTF("unable to parse blockExpression in PrimaryExpression\n");
            return NULL;
        }

        if (tokenType == TD_MULTIPLY) {
            result = createOperation(NODE_MUL, result, expression);
        }
        else {
            result = createOperation(NODE_DIV, result, expression);
        }
        dumpContext(context);
    }

    return result;
}


treeNode_t* buildTree(TDtokenContext_t* tokenContext) {
    tokenContext->index = 0;
    treeNode* root = getNumberExpression(tokenContext);
    TREE_DUMP(root, "parsed tree", DSL_SUCCESS);
    return NULL;
}

