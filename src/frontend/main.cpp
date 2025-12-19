#include <windows.h>
#include <stdio.h>

#include "asmwrite.h"
#include "treeDump.h"
#include "input.h"
#include "treeBuilder/treeBuilder.h"

static void setEncodings() {
    SetConsoleOutputCP(1251);
    SetConsoleCP      (1251);

    setlocale  (LC_ALL, "ru_RU.CP1251");
    _wsetlocale(LC_ALL, L"Russian_Russia.ACP");
    _wsetlocale(LC_NUMERIC, L"C");
}

int main(int argc, char* argv[]) {
    setEncodings();

    const char* sourcePath = TD_SOURCE_FILE_PATH;
    const char* treePath   = TD_TREE_FILE_PATH;

    if (argc > 1) {
        sourcePath = argv[1];
    }
    if (argc > 2) {
        treePath   = argv[2];
    }

    FILE* file = fopen(sourcePath, "rb");
    if (file == NULL) {
        PRINTERR("Unable to open file: %s", sourcePath);
        return 1;
    }
    fclose(file);

    int bytesRead = -1;
    wchar_t* buffer = NULL;
    SAFE_CALL(readFile(sourcePath, &buffer, &bytesRead));

    // wprintf(L"read buffer: %ls\n", buffer);

    TDtokenContext_t* tokenContext = parseTokens(buffer);
    treeNode_t* root = buildTree(tokenContext);

    saveTree(root, treePath);
    free(buffer);

    printf("finished\ntree is written to: %s\n", treePath);

    return 0;
}