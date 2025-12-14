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

calcOperation_t getOperation(treeNode* node) {
    assert(node);
    return getData(node).operation;
}
void setOperation(treeNode_t* node, calcOperation_t operation) {
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
        wprintf(L"what's the value of parameter '%ls'?  ", param);
        int value = 0;
        if (scanf("%d", &value) != 1) {
            printf("Nepravilno nabran nomer\n");
            return NULL;
        }
        dslParameters[dslParametersCount].name = wcsdup(param);
        dslParameters[dslParametersCount].hash = hash;
        dslParameters[dslParametersCount].value = value;
        result->data.parameter = &dslParameters[dslParametersCount];

        dslParametersCount++;
    }
    printf("\n");
    result->nodeType = PARAM_TYPE;
    result->left = NULL;
    result->right = NULL;

    return result;
}

treeNode_t* createNumber(int value) {
    treeNode_t* result = (treeNode_t*)calloc(1, sizeof(treeNode_t));
    result->nodeType = NUMBER_TYPE;
    result->left = NULL;
    result->right = NULL;

    result->data = {.number = value};

    return result;
}

treeNode_t* createOperation(calcOperation_t operation, treeNode* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*)calloc(1, sizeof(treeNode_t));
    result->nodeType = OPERATION_TYPE;
    result->left = left;
    result->right = right;

    result->data = {.operation = operation};

    return result;
}

treeNode_t* createNode(nodeData data, nodeType_t nodeType, treeNode* left, treeNode* right) {
    treeNode_t* result = (treeNode_t*)calloc(1, sizeof(treeNode_t));
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

