#include "treeBuilder.h"

#include "common.h"
#include "input.h"
#include "treeDump.h"

treeNode_t* getPrimaryNumberExpression(TDtokenContext_t* context);
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
#ifdef DEBUG
    printf("Context:\n    [");
    for (int i = 0; context->tokens[i] != NULL; i++) {
        if (i == context->index) {
            printf("{");
        }

        bool found = false;
        for (int j = 0; j < TD_TOKENS_INFO_SIZE; j++) {
            if (TD_TOKENS_INFO[j].tokenType == context->tokens[i]->type) {
                wprintf(L"cmnd: %ls", TD_TOKENS_INFO[j].representation);
                found = true;
                break;
            }
        }
        if (found) {
        }
        else if (context->tokens[i]->type == TD_NUMBER) {
            printf("num: %g", context->tokens[i]->value.number);
        }
        else if (context->tokens[i]->type == TD_STRING) {
            wprintf(L"str: %ls", context->tokens[i]->value.str);
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
#endif
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
    treeNode_t* result = getPrimaryNumberExpression(context);
    if (result == NULL) {
        DPRINTF("unable to parse primaryExpression in numberExpression\n");
        dumpContext(context);
        return NULL;
    }
    dumpContext(context);
    while (getCurToken(context) != NULL &&
        (getCurToken(context)->type == TD_PLUS
      || getCurToken(context)->type == TD_MINUS
      || getCurToken(context)->type == TD_LESS_THAN)) {
        TDtokenType_t tokenType = getCurToken(context)->type;
        nextToken(context);
        dumpContext(context);
        DPRINTF("parsing another primaryExpression in NumberExpression\n");
        treeNode_t* expression = getPrimaryNumberExpression(context);
        if (expression == NULL) {
            DPRINTF("unable to parse primaryExpression in NumberExpression\n");
            return NULL;
        }
        DPRINTF("parsed another primaryExpression in NumberExpression\n");
        dumpContext(context);

        result = createOperation(tokenType, result, expression);
        dumpContext(context);
    }
    DPRINTF("parsed number expression\n");

    return result;
}

treeNode_t* getPrimaryNumberExpression(TDtokenContext_t* context) {
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

        DPRINTF("parsed block expression in primaryNumberExpression\n");
        result = createOperation(tokenType, result, expression);
        dumpContext(context);
    }

    return result;
}

treeNode_t* getValue(TDtokenContext_t* context) {
    assert(context);
    TDtoken_t* token = getCurToken(context);
    if (token->type == TD_INPUT || token->type == TD_PRINT) {
        nextToken(context);
        return createOperation(token->type, NULL, NULL);
    }

    return getNumberExpression(context);
}
treeNode_t* getExpression(TDtokenContext_t* context) {
    assert(context);
    DPRINTF("parsing lang expression\n");

    if (getCurToken(context) == NULL) {
        PRINTERR("null token, unable to parse language expression\n");
        return NULL;
    }

    if (getCurToken(context)->type == TD_PRINT) {
        nextToken(context);
        if (getCurToken(context) == NULL || getCurToken(context)->type != TD_STRING) {
            PRINTERR("null token, unable to parse print value\n");
            return NULL;
        }

        DPRINTF("parsed print expression\n");
        return createOperation(TD_PRINT, createParameter(nextToken(context)->value.str), NULL);
    }

    if (getCurToken(context)->type == TD_HLT) {
        DPRINTF("parsed hlt expression\n");
        nextToken(context);
        return createOperation(TD_HLT, NULL, NULL);
    }

    treeNode_t* parameter = getParameter(context);
    if (parameter == NULL) {
        PRINTERR("expected var name in lang expression. Invalid character at %s:%d:%d\n",
                 TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    DPRINTF("parsed parameter in getExpression\n");
    dumpContext(context);

    if (getCurToken(context) == NULL || getCurToken(context)->type != TD_EQUALS) {
        PRINTERR("expected equals sign in lang expression. Invalid character at %s:%d:%d\n",
                 TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    DPRINTF("parsed equals in getExpression\n");
    dumpContext(context);

    treeNode_t* value = getValue(context);
    if (value == NULL) {
        PRINTERR("expected value in lang expression. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }

    DPRINTF("parsed expression\n");
    dumpContext(context);
    return createExpression(TD_DECLARATION, parameter, value);
}

static bool verifyToken(TDtokenContext_t* context, TDtokenType_t tokenType) {
    assert(context);
    return getCurToken(context) != NULL && getCurToken(context)->type == tokenType;
}

treeNode_t* getIfs(TDtokenContext_t* context) {
    assert(context);
    DPRINTF("parsing ifs expression\n");

    if (!verifyToken(context, TD_IF)) {
        DPRINTF("expected if\n");
        return NULL;
    }
    nextToken(context);

    if (!verifyToken(context, TD_OPENING_BRACKET)) {
        PRINTERR("expected opening bracket. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    treeNode_t* numberExpr1 = getNumberExpression(context);
    if (numberExpr1 == NULL) {
        DPRINTF("unable to parse ifs expression\n");
        return NULL;
    }


    TDtokenType_t cOperator = getCurToken(context)->type;
    if (getCurToken(context) == NULL
    || (cOperator != TD_EQUALS && cOperator != TD_LESS_THAN && cOperator != TD_NOT_EQUALS)) {
        PRINTERR("expected conditional operator. Invalid character at %s:%d:%d\n",
                                 TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    treeNode_t* numberExpr2 = getNumberExpression(context);
    if (numberExpr2 == NULL) {
        DPRINTF("unable to parse ifs second expression\n");
        return NULL;
    }

    if (!verifyToken(context, TD_CLOSING_BRACKET)) {
        PRINTERR("expected closing bracket. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    DPRINTF("parsed if condition and brackets\n");
    dumpContext(context);

    if (!verifyToken(context, TD_OPENING_BRACKET)) {
        PRINTERR("expected opening bracket. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    treeNode_t* code = getProgram(context);
    if (code == NULL) {
        DPRINTF("unable to parse commands in if expression\n");
        return NULL;
    }

    if (!verifyToken(context, TD_CLOSING_BRACKET)) {
        PRINTERR("expected closing bracket. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        return NULL;
    }
    nextToken(context);

    DPRINTF("parsed if with code\n");
    dumpContext(context);

    return createExpression(TD_IFS, createOperation(cOperator, numberExpr1, numberExpr2), code);
}

treeNode_t* getCommand(TDtokenContext_t* context) {
    assert(context);
    DPRINTF("parsing command expression\n");
    treeNode_t* result = getIfs(context);
    if (result != NULL) {
        return result;
    }

    result = getExpression(context);
    if (result == NULL) {
        DPRINTF("unable to parse command expression\n");
        return result;
    }
    DPRINTF("parsed expression in getCommand\n");
    dumpContext(context);
    if (getCurToken(context) == NULL || getCurToken(context)->type != TD_SEMICOLON) {
        DPRINTF("expected semicolon at command expression\n");
        return NULL;
    }
    nextToken(context);
    DPRINTF("parsed command\n");
    dumpContext(context);

    return result;
}

treeNode_t* getProgram(TDtokenContext_t* context) {
    assert(context);
    treeNode_t* result = createLinker(NULL, NULL);
    treeNode_t* cur = NULL;
    while ((cur = getCommand(context)) != NULL) {
        result = createLinker(cur, result);
    }
    DPRINTF("parsed program\n");
    dumpContext(context);

    return result;
}

treeNode_t* buildTree(TDtokenContext_t* context) {
    context->index = 0;
    treeNode* root = getProgram(context);
    if (getCurToken(context) != NULL) {
        PRINTERR("expected end of the program. Invalid character at %s:%d:%d\n",
                         TD_FILE_PATH, getCurToken(context)->line, getCurToken(context)->index);
        dumpContext(context);
        return NULL;
    }
    TREE_DUMP(root, "parsed tree", DSL_SUCCESS);
    return root;
}

