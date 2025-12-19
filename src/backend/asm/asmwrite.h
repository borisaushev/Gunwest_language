#ifndef ASMWRITE_H
#define ASMWRITE_H

#include <stdio.h>
#include "treeSctruct.h"

int writeAsmToFile(FILE* outFile, treeNode_t* node);

int writeAsm(const char* filename, treeNode_t* node);

#endif // ASMWRITE_H