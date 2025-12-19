#include "treeSctruct.h"

#include <cassert>
#include <cstdio>

#include "common.h"
#include "stack.h"
#include "treeDump.h"

static dslParameter_t dslParameters[DSL_MAX_PARAMETERS] = {};
static size_t dslParametersCount = 0;

size_t getParametersCount() {
    return dslParametersCount;
}

dslParameter_t* getParameter(size_t index) {
    assert(index < dslParametersCount);
    return &dslParameters[index];
}

int getParameterIndex(dslParameter_t* param) {
    for (size_t i = 0; i < dslParametersCount; i++) {
        if (param->hash == dslParameters[i].hash) {
            return (int) i;
        }
    }
    return -1;
}

treeNode_t* getRight(treeNode_t* node) {
    assert(node);
    return node->right;
}

void setRight(treeNode_t* node, treeNode_t* right) {
    assert(node);
    node->right = right;
}

treeNode_t* getLeft(treeNode_t* node) {
    assert(node);
    return node->left;
}

void setLeft(treeNode_t* node, treeNode_t* left) {
    assert(node);
    node->left = left;
}

treeElType_t getData(treeNode_t* node) {
    assert(node);
    return node->data;
}

void setData(treeNode_t* node, treeElType_t data) {
    assert(node);
    node->data = data;
}

int getNumber(treeNode* node) {
    assert(node);
    return getData(node).number;
}
void setNumber(treeNode_t* node, int number) {
    assert(node);
    node->data.number = number;
}

dslParameter_t* getParameter(treeNode *node) {
    assert(node);
    return getData(node).parameter;
}

void setParameter(treeNode_t* node, dslParameter_t* parameter) {
    assert(node);
    (node->data).parameter = parameter;
}

TDtokenType_t getOperation(treeNode* node) {
    assert(node);
    return getData(node).operation;
}
void setOperation(treeNode_t* node, TDtokenType_t operation) {
    assert(node);
    node->data.operation = operation;
}

nodeType_t getNodeType(treeNode_t* node) {
    assert(node);
    return node->nodeType;
}

void setNodeType(treeNode_t* node, nodeType_t nodeType) {
    assert(node);
    node->nodeType = nodeType;
}


void initDslParametersValues() {
    for (int i = 0; i < DSL_MAX_PARAMETERS; i++) {
        dslParameters[i].value = DSL_POISON;
        dslParameters[i].hash = DSL_POISON;
        dslParameters[i].name = L"";
    }
}

treeNode_t* createParameter(wchar_t* param) {
    assert(param);
    treeNode_t* result = (treeNode_t*)calloc(1, sizeof(treeNode_t));

    unsigned int hash = djb2StrHash(param);
    for (size_t i = 0; i < dslParametersCount; i++) {
        if (dslParameters[i].hash == hash) {
            result->data.parameter = &dslParameters[i];
            break;
        }
    }
    if (result->data.parameter == NULL) {
        dslParameters[dslParametersCount].name = wcsdup(param);
        dslParameters[dslParametersCount].hash = hash;
        dslParameters[dslParametersCount].value = DSL_POISON;
        result->data.parameter = &dslParameters[dslParametersCount];

        dslParametersCount++;
    }
    result->nodeType = PARAM_TYPE;
    result->left = NULL;
    result->right = NULL;

    return result;
}

treeNode_t* createNumber(int value) {
    treeNode_t* result = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    result->nodeType = NUMBER_TYPE;
    result->left = NULL;
    result->right = NULL;

    result->data = {.number = value};

    return result;
}

treeNode_t* createOperation(TDtokenType_t operation, treeNode* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    result->nodeType = OPERATION_TYPE;
    result->left = left;
    result->right = right;

    result->data = {.operation = operation};

    return result;
}

treeNode_t* createExpression(TDexpressionType_t expressionType, treeNode* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    result->nodeType = EXPRESSION_TYPE;
    result->left = left;
    result->right = right;

    result->data = {.expressionType = expressionType};

    return result;
}

treeNode_t* createLinker(treeNode_t* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    result->nodeType = LINKER_TYPE;
    result->left = left;
    result->right = right;

    result->data = {};

    return result;
}

treeNode_t* createNode(nodeData data, nodeType_t nodeType, treeNode* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*) calloc(1, sizeof(treeNode_t));
    result->nodeType = nodeType;
    result->left = left;
    result->right = right;

    result->data = data;

    return result;
}


static void countTreeSize(treeNode_t* node, size_t* size) {
    if ((*size)++ > MAX_TREE_DEPTH) {
        return;
    }

    if (getLeft(node) != NULL) {
        countTreeSize(getLeft(node), size);
    }
    if (getRight(node) != NULL) {
        countTreeSize(getRight(node), size);
    }
}

int validateTree(treeNode_t* root) {
    if (root == NULL) {
        RETURN_ERR(TR_NULL_PTR, "root is null");
    }

    size_t size = 0;
    countTreeSize(root, &size);
    if (size > MAX_TREE_DEPTH) {
        RETURN_ERR(TR_INVALID_SIZE, "tree is cycled");
    }

    return TR_SUCCESS;
}


static void destroyNode(treeNode_t* node) {
    if (node != NULL) {
        destroyNode(getLeft(node));
        destroyNode(getRight(node));
        free(node);
    }
}

static int destroyLeftNode(treeNode* parentNode) {
    destroyNode(getLeft(parentNode));
    setLeft(parentNode, NULL);

    return TR_SUCCESS;
}

static int destroyRightNode(treeNode* parentNode) {
    destroyNode(getRight(parentNode));
    setRight(parentNode, NULL);

    return TR_SUCCESS;
}

void destroyTree(treeNode_t* root) {
    destroyNode(root);
}

void writeNodeRec(treeNode_t* node, FILE* file) {
    if (node == NULL) {
        fwprintf(file, L"nil ");
    }
    else {
        fwprintf(file, L"( \"");
        switch (node->nodeType) {
            case EXPRESSION_TYPE: {
                switch (getData(node).expressionType) {
                case TD_IF_EXPRESSION_TYPE: {
                    fwprintf(file, L"if");
                    break;
                }
                case TD_WHILE_EXPRESSION_TYPE: {
                    fwprintf(file, L"while");
                    break;
                }
                case TD_DECLARATION: {
                    fwprintf(file, L"=");
                    break;
                }
                default: {
                    PRINTERR("invalid expression type\n");
                }
                }
                break;
            }
            case LINKER_TYPE: {
                fwprintf(file, L";");
                break;
            }
            case OPERATION_TYPE: {
                bool found = false;
                for (size_t i = 0; i < TD_TOKENS_INFO_SIZE; i++) {
                    if (getData(node).operation == TD_TOKENS_INFO[i].tokenType) {
                        fwprintf(file, L"%ls", TD_TOKENS_INFO[i].treeRepresentation);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    PRINTERR("operation not found: %d\n", getData(node).operation);
                }
                break;
            }
            case NUMBER_TYPE: {
                fwprintf(file, L"%d", getData(node).number);
                break;
            }
            case PARAM_TYPE: {
                fwprintf(file, L"%ls", getData(node).parameter->name);
                break;
            }
            default: {
                PRINTERR("invalid node type");
                return;
            };
        }

        fwprintf(file, L"\" ");
        writeNodeRec(getLeft(node), file);
        writeNodeRec(getRight(node), file);
        fwprintf(file, L") ");
    }
}

int saveTree(treeNode_t* root, const char* fileName) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        RETURN_ERR(DSL_NULL_PTR, "unable to open file");
    }
    writeNodeRec(root, file);

    fclose(file);
    return DSL_SUCCESS;
}

static void dumpBuffer(wchar_t **curPos, const wchar_t *buffer) {
// #ifdef DEBUG
    assert(curPos);
    assert(*curPos);

    wchar_t dumpString[MAX_LINE_LENGTH] = {};
    wcscat(dumpString, L"buffer:\n\t</p>\n\t<font color=\"#00bfff\">\n\t\t");
    wcsncat(dumpString, buffer,  wcslen(buffer) - wcslen(*curPos));
    wcscat(dumpString, L"\n\t</font>\n");

    wcscat(dumpString, L"\t<font color=\"#0e2466\">\n\t\t");
    wcscat(dumpString, *curPos);
    wcscat(dumpString, L"\n\t</font>");

    treeLog(dumpString);
// #endif
}

static void skipSpaces(wchar_t** curPos) {
    while (iswspace(**curPos)) {
        (*curPos)++;
    }
}

int parseNode(wchar_t** curPos, treeNode_t** cur, const wchar_t* bufferCopy, const wchar_t* buffer) {
    skipSpaces(curPos);

    treeLog(L"Parsing node");
    dumpBuffer(curPos, bufferCopy);

    if (**curPos == '(') {
        treeLog(L"Read '(' ");
        skipSpaces(curPos);
        (*curPos)++;

        treeLog(L"skipped '('");
        dumpBuffer(curPos, bufferCopy);

        skipSpaces(curPos);
        if (**curPos != L'"') {
            treeLog(L"expected '\"'");
                    PRINTERR("Expected '\"', invalid character '%c' (%d) at %s:%d:%zu\n", **curPos, **curPos, TD_TREE_FILE_PATH, 1, (*curPos - buffer + 1));

            RETURN_ERR(DSL_FILE_NOT_FOUND, "expected '\"'");
        }

        wchar_t* end = wcschr(*curPos + 1, L'"');
        if (end == NULL) {
            treeLog(L"Havent found the end of the string");
            PRINTERR("Cannot find '\"', current character '%c' (%d) at %s:%d:%zu\n", **curPos, **curPos, TD_TREE_FILE_PATH, 1, (*curPos - buffer + 1));

            RETURN_ERR(DSL_INVALID_INPUT, "invalid input tree");
        }

        *end = '\0';
        wchar_t* str = wcsdup(*curPos + 1);
        TDtokenType_t tokenType = TD_STRING;
        if (iswdigit(str[0]) || (str[0] == L'-' && iswdigit(str[1]))) {
            tokenType = TD_NUMBER;
        }
        else {
            for (size_t i = 0; i < TD_TOKENS_INFO_SIZE; i++) {
                if (!wcscmp(str, TD_TOKENS_INFO[i].treeRepresentation)) {
                    tokenType = TD_TOKENS_INFO[i].tokenType;
                    break;
                }
            }
        }
        switch (tokenType) {
            case TD_NUMBER: {
                int number = -1;
                swscanf(str, L"%d", &number);
                *cur = createNumber(number);
                break;
            }
            case TD_STRING: {
                *cur = createParameter(str);
                break;
            }
            case TD_MINUS:
            case TD_PLUS:
            case TD_MULTIPLY:
            case TD_NOT_EQUALS:
            case TD_LESS_THAN:
            case TD_HLT:
            case TD_SQRT:
            case TD_PRINT:
            case TD_INPUT:
            case TD_EQUALS:
            case TD_DIVIDE: {
                *cur = createOperation(tokenType, NULL, NULL);
                break;
            }
            case TD_SEMICOLON: {
                *cur = createLinker(NULL, NULL);
                break;
            }
            case TD_IF: {
                *cur = createExpression(TD_IF_EXPRESSION_TYPE, NULL, NULL);
                break;
            }
            case TD_WHILE: {
                *cur = createExpression(TD_WHILE_EXPRESSION_TYPE, NULL, NULL);
                break;}
            case TD_DECLARE: {
                *cur = createExpression(TD_DECLARATION, NULL, NULL);
                break;
            }
            default: {
                PRINTERR("Unknown token type '%d'", tokenType);
                break;
            }
        }

        treeLog(L"Created new node");
        TREE_DUMP(*cur, "Created node", DSL_SUCCESS);

        *curPos = end + 1;

        skipSpaces(curPos);
        treeLog(L"skipped data");
        dumpBuffer(curPos, bufferCopy);

        treeLog(L"Parsing left subtree");
        SAFE_CALL(parseNode(curPos, &(*cur)->left, bufferCopy, buffer));
        treeLog(L"Parsed left subtree");
        TREE_DUMP(*cur, "Parsed left subtree", DSL_SUCCESS);

        treeLog(L"Parsing right subtree");
        SAFE_CALL(parseNode(curPos, &(*cur)->right, bufferCopy, buffer));
        treeLog(L"Parsed right subtree");
        TREE_DUMP(*cur, "Parsed right subtree", DSL_SUCCESS);

        skipSpaces(curPos);
        if (**curPos != ')') {
            PRINTERR("Expecting ')', Invalid character '%c' (%d) at %s:%d:%zu\n", **curPos, **curPos, TD_TREE_FILE_PATH, 1, (*curPos - buffer + 1));

            RETURN_ERR(DSL_INVALID_INPUT, "expected ')'");
        }
        (*curPos)++;
        skipSpaces(curPos);
        treeLog(L"skipped ')'");
        dumpBuffer(curPos, bufferCopy);

        return DSL_SUCCESS;
    }
    else if (wcsncmp(*curPos, L"nil", 3) == 0) {
            treeLog(L"found nil node");
            *curPos += strlen("nil");

            treeLog(L"Skipped nil");
            dumpBuffer(curPos, bufferCopy);

            return DSL_SUCCESS;
        }
    else {
        PRINTERR("Expected '(' or nil, invalid character '%c', (%d) at %s:%d:%zu\n", **curPos, **curPos, TD_TREE_FILE_PATH, 1, (*curPos - buffer + 1));
        RETURN_ERR(DSL_INVALID_INPUT, "invalid input tree");
    }
}