#ifndef AKINATOR_AKINATORINPUT_H
#define AKINATOR_AKINATORINPUT_H
#include "stack.h"
#include "treeSctruct.h"

static TDtokenTypeInfo_t TD_TOKENS_INFO[] = {
    {TD_PLUS           , L"+"               , L"+"       , djb2StrHash(L"+")               },
    {TD_MINUS          , L"-"               , L"-"       , djb2StrHash(L"-")               },
    {TD_MULTIPLY       , L"*"               , L"*"       , djb2StrHash(L"*")               },
    {TD_DIVIDE         , L"/"               , L"/"       , djb2StrHash(L"/")               },
    {TD_LESS_THAN      , L"ВОТАФА"          , L"less"    , djb2StrHash(L"ВОТАФА")          },
    {TD_NOT_EQUALS     , L"ОПП"             , L"!="      , djb2StrHash(L"ОПП")             },
    {TD_IF             , L"ШНЕЙНЕ"          , L"if"      , djb2StrHash(L"ШНЕЙНЕ")          },
    {TD_WHILE          , L"ЕЩКЕРЕ"          , L"while"   , djb2StrHash(L"ЕЩКЕРЕ")          },
    {TD_SEMICOLON      , L"ФА"              , L";"       , djb2StrHash(L"ФА")              },
    {TD_DECLARE        , L"ПЕПЕ"            , L"="       , djb2StrHash(L"ПЕПЕ")            },
    {TD_EQUALS         , L"ГЛОК"            , L"=="      , djb2StrHash(L"ГЛОК")            },
    {TD_OPENING_BRACKET, L"ЛЕВАЯ_НОГА"      , L"obracket", djb2StrHash(L"ЛЕВАЯ_НОГА")      },
    {TD_CLOSING_BRACKET, L"ПРАВАЯ_НОГА"     , L"cbracket", djb2StrHash(L"ПРАВАЯ_НОГА")     },
    {TD_INPUT          , L"ХОЧУ_ВЫСТРЕЛ"    , L"scanf"   , djb2StrHash(L"ХОЧУ_ВЫСТРЕЛ")    },
    {TD_PRINT          , L"ЕСТЬ_ЧТО_СКАЗАТЬ", L"print"   , djb2StrHash(L"ЕСТЬ_ЧТО_СКАЗАТЬ")},
    {TD_SQRT           , L"АЙСГЕРГЕРБЕР"    , L"sqrt"    , djb2StrHash(L"АЙСГЕРГЕРБЕР")    },
    {TD_HLT            , L"ДРОПАЙ"          , L"hlt"     , djb2StrHash(L"ДРОПАЙ")          },
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