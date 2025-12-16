#ifndef AKINATOR_AKINATORINPUT_H
#define AKINATOR_AKINATORINPUT_H
#include "treeSctruct.h"

/*

PROGRAM = CMND+

CMND ::= IFS | (EXPRESSION';')
EXPRESSION ::= VAR '=' VALUE | FUNCTION
    VALUE ::= FUNCTION | NUMBER_EXPR
    FUNCTION ::= "str" " str"+

    NUMBER_EXPR ::= PRIMARY ([+-]PRIMARY)*
    PRIMARY ::= BLOCK ([/*]BLOCK)*
    BLOCK ::= NUMBER|VAR|'('NUMBER_EXPR')'

    VAR = "str"
IFS ::= "if" '(' NUMBER_EXPR ')' '{' PROGRAM '}'
 */

typedef union TDtokenValue {
    wchar_t* str;
    double number;
} TDtokenValue_t;

typedef struct TDtoken {
    TDtokenType_t type = TD_OPENING_BRACKET;
    TDtokenValue_t value;
    int index = -1;
    int line = -1;
} TDtoken_t;

const int MAX_TOKENS = 4096;
typedef struct TDtokenContext {
    TDtoken_t* tokens[MAX_TOKENS] = {};
    int index = 0;
    int line = 0;
    wchar_t* buffer;
    wchar_t* start;
} TDtokenContext_t;

TDtokenContext_t* parseTokens(wchar_t* buffer);

long getFileSize(const char* filename);
void skipSpaces(TDtokenContext* tokenContext);
void dumpBuffer(wchar_t **curPos, const wchar_t *buffer);
int readFile(const char *file_path, wchar_t** text, int* bytes_read);


#endif //AKINATOR_AKINATORINPUT_H