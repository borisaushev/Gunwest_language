#ifndef AKINATOR_AKINATORINPUT_H
#define AKINATOR_AKINATORINPUT_H
#include "treeSctruct.h"

/*

= ::= "ПЕПЕ"
< ::= "ВОТАФА"
!= ::= "ОПП"
if ::= "ШНЕЙНЕ"
while ::= "ЕЩКЕРЕ"
; ::= "ФА"
( ::= "ЛЕВАЯ_НОГА"
) ::= "ПРАВАЯ_НОГА"
input ::= "ХОЧУ_ВЫСТРЕЛ"
print ::= "ЕСТЬ_ЧТО_СКАЗАТЬ"
sqrt ::= "АЙСГЕРГЕРБЕР"
hlt ::= "ДРОПАЙ"
CONDITIONAL_OPERATOR ::= < | != | ==
STD_COMMAND ::=  print | hlt
STD_FUNCTION ::= input | (sqrt VALUE)


PROGRAM = CMND+

CMND ::= IFS | WHILE | (EXPRESSION;)
EXPRESSION ::= VAR = (VALUE | STD_COMMAND)
    VALUE ::= STD_FUNCTION | NUMBER_EXPR

    NUMBER_EXPR ::= PRIMARY ([+-]PRIMARY)*
    PRIMARY ::= BLOCK ([/*]BLOCK)*
    BLOCK ::= NUMBER | VAR | '('NUMBER_EXPR')'

    VAR = "[A-Za-z][A-Za-z0-9]*"
IFS ::= if '(' NUMBER_EXPR CONDITIONAL_OPERATOR NUMBER_EXPR ')' '(' PROGRAM ')'
WHILE ::= while '(' NUMBER_EXPR CONDITIONAL_OPERATOR NUMBER_EXPR ')' '(' PROGRAM ')'
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
    int line = 1;
    int lineIndex = 0;
    wchar_t* buffer;
    wchar_t* start;
} TDtokenContext_t;

TDtokenContext_t* parseTokens(wchar_t* buffer);

long getFileSize(const char* filename);
void skipSpaces(TDtokenContext* tokenContext);
void dumpBuffer(wchar_t **curPos, const wchar_t *buffer);
int readFile(const char *file_path, wchar_t** text, int* bytes_read);


#endif //AKINATOR_AKINATORINPUT_H