#ifndef AKINATOR_AKINATORINPUT_H
#define AKINATOR_AKINATORINPUT_H
#include "treeSctruct.h"

/*

PROGRAM = CMND+

CMND ::= IFS | (EXPRESSION';')
EXPRESSION ::= VAR '=' VALUE
    VALUE ::= STDFUNC | NUMBER_EXPR
    STDFUNC ::= printf|scanf

    NUMBER_EXPR ::= PRIMARY ([+-]PRIMARY)*
    PRIMARY ::= BLOCK ([/*]BLOCK)*
    BLOCK ::= NUMBER|VAR|'('NUMBER_EXPR')'

    VAR = "str"
IFS ::= "if" '(' NUMBER_EXPR ')' '{' PROGRAM '}'
 */


typedef enum TDtokenType {
    TD_PLUS           ,
    TD_MINUS          ,
    TD_MULTIPLY       ,
    TD_DIVIDE         ,
    TD_IF             ,
    TD_SEMICOLON      ,
    TD_EQUALS         ,
    TD_OPENING_BRACKET,
    TD_CLOSING_BRACKET,
    TD_INPUT          ,
    TD_PRINT          ,
    TD_NUMBER         ,
    TD_STRING         ,
} TDtokenType_t;

typedef struct TDtokenTypeInfo {
    TDtokenType_t tokenType;
    const wchar_t* representation;
} TDtokenTypeInfo_t;

const TDtokenTypeInfo_t TD_TOKENS_INFO[] = {
    {TD_PLUS,            L"+"               },
    {TD_MINUS,           L"-"               },
    {TD_MULTIPLY,        L"*"               },
    {TD_DIVIDE,          L"/"               },
    {TD_IF,              L"ШНЕЙНЕ?"         },
    {TD_SEMICOLON,       L"ФА"              },
    {TD_EQUALS,          L"ПЕПЕ"            },
    {TD_OPENING_BRACKET, L"ЛЕВАЯ_НОГА"      },
    {TD_CLOSING_BRACKET, L"ПРАВАЯ_НОГА"     },
    {TD_INPUT,           L"ХОЧУ_ВЫСТРЕЛ"    },
    {TD_PRINT,           L"ЕСТЬ_ЧТО_СКАЗАТЬ"},
};
const size_t TD_TOKENS_INFO_SIZE = sizeof(TD_TOKENS_INFO)/sizeof(TDtokenTypeInfo_t);

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
void dumpBuffer(char** curPos, const char* buffer);
int readFile(const char *file_path, wchar_t** text, int* bytes_read);


#endif //AKINATOR_AKINATORINPUT_H