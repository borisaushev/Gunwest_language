#include "asmwrite.h"
#include "common.h"
#include "input.h"
#include "treeDump.h"
#include "treeSctruct.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    treeNode_t* root = {};

    const char* treePath = TD_TREE_FILE_PATH;
    const char* asmPath  = TD_ASM_OUTPUT_PATH;

    if (argc > 1) {
        treePath = argv[1];
    }
    if (argc > 2) {
        asmPath  = argv[2];
    }

    printf("Tree file:  %s\n", treePath);
    printf("Asm output: %s\n", asmPath);

    FILE* file = fopen(treePath, "rb");
    if (file == NULL) {
        PRINTERR("unable to open file: %s", treePath);
        return 1;
    }
    fclose(file);

    int bytesRead = -1;
    wchar_t* buffer = NULL;
    SAFE_CALL(readFile(treePath, &buffer, &bytesRead));

    // wprintf(L"read buffer: %ls\n", buffer);

    wchar_t* cur = buffer;
    wchar_t* bufferCopy = wcsdup(cur);
    SAFE_CALL(parseNode(&cur, &root, bufferCopy, cur));

    DPRINTF("parsed tree\n");
    TREE_DUMP(root, "parsed tree", DSL_SUCCESS);

    SAFE_CALL(writeAsm(asmPath, root));

    printf("asm code written to: %s\n", asmPath);

    return 0;
}