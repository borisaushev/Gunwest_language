
#include <windows.h>
#include "treeDump.h"
#include "input.h"
#include "treeBuilder/treeBuilder.h"

int main() {

    SetConsoleOutputCP(1251);  // Установить кодировку консоли в UTF-8
    SetConsoleCP      (1251);

    setlocale  (LC_ALL, "ru_RU.CP1251");
    _wsetlocale(LC_ALL, L"Russian_Russia.ACP");
    _wsetlocale(LC_NUMERIC, L"C");

    FILE* file = fopen(TD_FILE_PATH, "rb");
    if (file == NULL) {
        PRINTERR("unable to open file");
    }
    int bytesRead = -1;
    wchar_t* buffer = NULL;
    SAFE_CALL(readFile(TD_FILE_PATH, &buffer, &bytesRead));

    wprintf(L"read buffer: %ls\n", buffer);

    TDtokenContext_t* tokenContext = parseTokens(buffer);
    buildTree(tokenContext);

    return 0;
}
