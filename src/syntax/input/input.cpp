#include "input.h"

#include "common.h"
#include "treeDump.h"


TDtoken_t* createNumberToken(TDtokenType_t type, double number, TDtokenContext_t* tokenContext) {
    assert(tokenContext);

    TDtoken_t* token = (TDtoken_t*) calloc(1, sizeof(TDtoken_t));

    token->type = type;
    token->value = {.number = number};
    token->index = tokenContext->lineIndex;
    token->line = tokenContext->line;

    return token;
}

//Как находятся ключевые слова
//Где описан синтаксис
TDtoken_t* createStringToken(wchar_t* str, TDtokenContext_t* tokenContext) {
    assert(str);
    assert(tokenContext);

    TDtoken_t* token = (TDtoken_t*) calloc(1, sizeof(TDtoken_t));

    token->type = TD_STRING;
    token->value = {.str = str};
    token->index = tokenContext->lineIndex;
    token->line = tokenContext->line;

    return token;
}

TDtoken_t* createEmptyToken(TDtokenType_t type, TDtokenContext_t* tokenContext) {
    assert(tokenContext);

    TDtoken_t* token = (TDtoken_t*) calloc(1, sizeof(TDtoken_t));

    token->type = type;
    token->value = {.str = NULL};
    token->index = tokenContext->lineIndex + 1;
    token->line = tokenContext->line;

    return token;
}

void initTokenContext(TDtokenContext_t* tokenContext, wchar_t* buffer) {
    assert(buffer);
    assert(tokenContext);

    tokenContext->buffer = buffer;
    tokenContext->start = buffer;
    tokenContext->line = 1;
    tokenContext->lineIndex = 0;
}
void setNewToken(TDtokenContext_t* tokenContext, TDtoken_t* token) {
    assert(token);
    assert(tokenContext);

    tokenContext->tokens[tokenContext->index++] = token;
}

wchar_t curChar(TDtokenContext_t* tokenContext) {
    return *(tokenContext->buffer);
}

wchar_t* getBuffer(TDtokenContext_t* tokenContext) {
    assert(tokenContext);
    return tokenContext->buffer;
}
void moveBuffer(TDtokenContext_t* tokenContext, int n) {
    assert(tokenContext);
    tokenContext->buffer += n;
    tokenContext->lineIndex += n;
}

void parseInput(TDtokenContext_t *tokenContext, wchar_t* input) {
    int n = 0;
    if (iswalpha(curChar(tokenContext))) {
        while (curChar(tokenContext) != L'\0'
               && (iswalpha(curChar(tokenContext)) ||  curChar(tokenContext) == L'_')) {
            input[n] = curChar(tokenContext);
            moveBuffer(tokenContext, 1);
            n++;
        }
    }
    else {
        while (!iswspace(curChar(tokenContext))) {
            input[n] = curChar(tokenContext);
            moveBuffer(tokenContext, 1);
            n++;
        }
    }
}

TDtokenContext_t* parseTokens(wchar_t* buffer) {
    TDtokenContext_t* tokenContext = (TDtokenContext_t*) calloc(1, sizeof(TDtokenContext_t));
    if (tokenContext == NULL) {
        PRINTERR("unable to allocate TDtokenContext_t");
        return NULL;
    }
    initTokenContext(tokenContext, buffer);

    while (curChar(tokenContext) != '\0') {
        skipSpaces(tokenContext);
        if ((curChar(tokenContext) == '-' && isdigit(*(tokenContext->buffer + 1)))
            || isdigit(curChar(tokenContext))) {
            if (curChar(tokenContext) == '-') {
                moveBuffer(tokenContext, 1);
                int n = 0;
                double input = NAN;
                swscanf(getBuffer(tokenContext), L"%lf%n", &input, &n);

                setNewToken(tokenContext, createNumberToken(TD_NUMBER, -input, tokenContext));
                moveBuffer(tokenContext, n);
            }
            else {
                int n = 0;
                double input = NAN;
                swscanf(getBuffer(tokenContext), L"%lf%n", &input, &n);

                setNewToken(tokenContext, createNumberToken(TD_NUMBER, input, tokenContext));
                moveBuffer(tokenContext, n);
            }
        }
        else {
            wchar_t input[MAX_LINE_LENGTH] = {};
            parseInput(tokenContext, input);

            bool found = false;
            wprintf(L"searching for: %ls\n", input);
            for (size_t i = 0; i < TD_TOKENS_INFO_SIZE; i++) {
                if (!wcsicmp(input, TD_TOKENS_INFO[i].representation)) {
                    wprintf(L"found: %ls\n", TD_TOKENS_INFO[i].representation);
                    found = true;
                    TDtoken_t * newToken = createEmptyToken(TD_TOKENS_INFO[i].tokenType, tokenContext);
                    setNewToken(tokenContext, newToken);
                    break;
                }
            }
            if (!found) {
                TDtoken_t* newToken = createStringToken(wcsdup(input), tokenContext);
                setNewToken(tokenContext, newToken);
            }
        }
        skipSpaces(tokenContext);
    }

    for (int i = 0; i < tokenContext->index; i++) {
        bool found = false;
        printf("%d: ", i);
        for (int j = 0; j < TD_TOKENS_INFO_SIZE; j++) {
            if (TD_TOKENS_INFO[j].tokenType == tokenContext->tokens[i]->type) {
                wprintf(L"command: %ls\n", TD_TOKENS_INFO[j].representation);
                found = true;
                break;
            }
        }
        if (found) {
            continue;
        }
        if (tokenContext->tokens[i]->type == TD_NUMBER) {
            printf("number: %g\n", tokenContext->tokens[i]->value.number);
        }
        else if (tokenContext->tokens[i]->type == TD_STRING) {
            wprintf(L"string: %ls", tokenContext->tokens[i]->value.str);
        }
        else {
            PRINTERR("bbq chicken alert");
        }
    }
    printf("\n");

    return tokenContext;
}

long getFileSize(const char* filename) {
    struct stat st = {};
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }

    return -1;
}

void skipSpaces(TDtokenContext_t* tokenContext) {
    while (iswspace(curChar(tokenContext))) {
        if (curChar(tokenContext) == '\n') {
            (tokenContext->line)++;
            tokenContext->lineIndex = 0;
        }
        moveBuffer(tokenContext, 1);
    }
    if (curChar(tokenContext) == '#') {
        while (curChar(tokenContext) != '\n') {
            moveBuffer(tokenContext, 1);
        }
        moveBuffer(tokenContext, 1);
        tokenContext->lineIndex = 0;
        (tokenContext->line)++;
        skipSpaces(tokenContext);
    }
}

int readFile(const char *file_path, wchar_t** text, int* bytes_read) {
    FILE* file = fopen(file_path, "rb");  // Бинарный режим
    if (!file) {
        PRINTERR("Could not open file %s\n", file_path);
        RETURN_ERR(DSL_FILE_NOT_FOUND, "Could not open file");
    }

    long file_size = getFileSize(file_path);
    DPRINTF("file size: %ld bytes\n", file_size);

    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        RETURN_ERR(DSL_NULL_PTR, "Out of memory");
    }

    size_t read_bytes = fread(buffer, 1, file_size, file);
    buffer[read_bytes] = '\0';
    *bytes_read = (int)read_bytes;
    fclose(file);

    DPRINTF("Read %ld bytes\n", *bytes_read);


    *text = (wchar_t *)calloc(read_bytes + 1, sizeof(wchar_t));
    if (!*text) {
        free(buffer);
        RETURN_ERR(DSL_NULL_PTR, "Out of memory");
    }

    size_t converted = mbstowcs(*text, buffer, read_bytes);
    (*text)[converted] = L'\0';

    free(buffer);

    return 0;
}

void dumpBuffer(char **curPos, const char *buffer) {
    char dumpString[MAX_LINE_LENGTH] = {};
    strcat(dumpString, "buffer:\n\t</p>\n\t<font color=\"#00bfff\">\n\t\t");
    strncat(dumpString, buffer,  strlen(buffer) - strlen(*curPos));
    strcat(dumpString, "\n\t</font>\n");

    strcat(dumpString, "\t<font color=\"#0e2466\">\n\t\t");
    strcat(dumpString, *curPos);
    strcat(dumpString, "\n\t</font>");

    treeLog(dumpString);
}