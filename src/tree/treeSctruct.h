#ifndef TREE_TREESCTRUCT_H
#define TREE_TREESCTRUCT_H
#include "common.h"

const int DSL_POISON = 7777;
const int DSL_MAX_PARAMETERS = 100;

size_t getParametersCount();
dslParameter_t* getParameter(size_t index);
int getParameterIndex(dslParameter_t* param);

treeNode_t* createNumber(int value);
treeNode_t* createOperation(TDtokenType_t operation, treeNode* left, treeNode* right);
treeNode_t* createParameter(wchar_t* param);
treeNode_t* createLinker(treeNode_t* left, treeNode* right);
treeNode_t* createNode(nodeData data, nodeType_t nodeType, treeNode* left, treeNode* right);
treeNode_t* createExpression(TDexpressionType_t expressionType, treeNode* left, treeNode* right);

void initDslParametersValues();

treeNode_t* getRight(treeNode_t* node);
void setRight(treeNode_t* node, treeNode_t* right);

treeNode_t* getLeft(treeNode_t* node);
void setLeft(treeNode_t* node, treeNode_t* left);

treeElType_t getData(treeNode_t* node);
void setData(treeNode_t* node, treeElType_t data);

int getNumber(treeNode* node);
void setNumber(treeNode_t* node, int value);

dslParameter_t *getParameter(treeNode *node);
void setParameter(treeNode_t* node, dslParameter_t* parameter);

void setOperation(treeNode_t* node, TDtokenType_t operation);

nodeType_t getNodeType(treeNode_t* node);
void setNodeType(treeNode_t* node, nodeType_t nodeType);

int validateTree(treeNode_t* root);
void destroyTree(treeNode_t* root);


#endif //TREE_TREESCTRUCT_H