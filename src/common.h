#ifndef TREE_COMMON_H
#define TREE_COMMON_H

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <cassert>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>

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


const char* const HTML_FILE_PATH = "..\\files\\logs\\tree\\tree_dump.html";
const char* const DOT_FILE_PATH = "..\\files\\logs\\tree\\images\\tree.dot";
const char* const SVG_FORMAT = "..\\files\\logs\\tree\\images\\tree_%zu.svg";

const int MAX_TREE_DEPTH = 10000;
const int BACK_COLOR = 0x56db70;

#define BEGIN do {
#define END   } while (0)

#ifdef DEBUG
#define DPRINTF(...) \
    BEGIN \
        printf("\033[33m"); \
        printf(__VA_ARGS__); \
        printf("\033[0m"); \
        fflush(stdout); \
    END
#else
    #define DPRINTF(...) ;
#endif //DEBUG

//define for printing in stderr
#define PRINTERR(...) \
    BEGIN \
        fprintf(stderr, "\033[31m"); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, " at %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, "\033[0m"); \
        fflush(stderr); \
    END

#define RETURN_ERR(code, desc) \
    BEGIN \
        PRINTERR("ERROR [%s:%d]: %s (code %d)\n", __FILE__, __LINE__, desc, code); \
        return code; \
    END

#define SAFE_CALL(func) \
    BEGIN \
        int sf_call ## __LINE__ = (func); \
        if (sf_call ## __LINE__ != 0) { \
            return sf_call ## __LINE__; \
        } \
    END

#define      BB_TRY                  int __tx_error = 0; { goto __tx_try; } __tx_try: {
#define      BB_CHECKED( cmd )       { if ((__tx_error = (cmd)) != 0) {goto __tx_catch;}}
#define      BB_CATCH                goto __tx_finally; __tx_catch: {
#define      BB_ENDCATCH             return __tx_error;}
#define      BB_FAIL( error_code )   { __tx_error = error_code; goto __tx_catch; }
#define      BB_FINALLY              __tx_finally:
#define      BB_ENDTRY               }

#define FAIL_ERR(code, desc) \
    BEGIN \
        PRINTERR("ERROR [%s:%d]: %s (code %d)\n", __FILE__, __LINE__, desc, code); \
        BB_FAIL(code); \
    END

typedef enum dsl_error {
    DSL_SUCCESS = 0,
    DSL_NULL_PTR,
    DSL_INVALID_CAPACITY,
    DSL_INVALID_INDEX,
    DSL_INVALID_PTR,
    DSL_FILE_NOT_FOUND,
    DSL_INVALID_INPUT,
    DSL_CANT_OPEN_FILE
} dsl_error_t;

const int MAX_LINE_LENGTH = 10000;

// const char* const TD_FILE_PATH = "../files/durden/kvadratka.bb";
const char* const TD_SOURCE_FILE_PATH = "../files/durden/factorial.bb";
const char* const TD_TREE_FILE_PATH = "../files/durden/tree.txt";
const char* const TD_ASM_OUTPUT_PATH = "C:/Users/bossb/CLionProjects/asm_calc/files/FA.asm";

typedef enum TDtokenType {
    TD_PLUS=1         ,
    TD_MINUS          ,
    TD_MULTIPLY       ,
    TD_DIVIDE         ,
    TD_LESS_THAN      ,
    TD_NOT_EQUALS     ,
    TD_IF             ,
    TD_WHILE          ,
    TD_SEMICOLON      ,
    TD_EQUALS         ,
    TD_DECLARE        ,
    TD_OPENING_BRACKET,
    TD_CLOSING_BRACKET,
    TD_INPUT          ,
    TD_PRINT          ,
    TD_SQRT           ,
    TD_HLT            ,
    TD_NUMBER         ,
    TD_STRING         ,
} TDtokenType_t;

typedef struct TDtokenTypeInfo {
    TDtokenType_t tokenType;
    const wchar_t* representation;
    const wchar_t* treeRepresentation;
} TDtokenTypeInfo_t;

const TDtokenTypeInfo_t TD_TOKENS_INFO[] = {
    {TD_PLUS           , L"+"               , L"+"       },
    {TD_MINUS          , L"-"               , L"-"       },
    {TD_MULTIPLY       , L"*"               , L"*"       },
    {TD_DIVIDE         , L"/"               , L"/"       },
    {TD_LESS_THAN      , L"ВОТАФА"          , L"less"    },
    {TD_NOT_EQUALS     , L"ОПП"             , L"!="      },
    {TD_IF             , L"ШНЕЙНЕ"          , L"if"      },
    {TD_WHILE          , L"ЕЩКЕРЕ"          , L"while"   },
    {TD_SEMICOLON      , L"ФА"              , L";"       },
    {TD_DECLARE        , L"ПЕПЕ"            , L"="       },
    {TD_EQUALS         , L"ГЛОК"            , L"=="      },
    {TD_OPENING_BRACKET, L"ЛЕВАЯ_НОГА"      , L"obracket"},
    {TD_CLOSING_BRACKET, L"ПРАВАЯ_НОГА"     , L"cbracket"},
    {TD_INPUT          , L"ХОЧУ_ВЫСТРЕЛ"    , L"scanf"   },
    {TD_PRINT          , L"ЕСТЬ_ЧТО_СКАЗАТЬ", L"print"   },
    {TD_SQRT           , L"АЙСГЕРГЕРБЕР"    , L"sqrt"    },
    {TD_HLT            , L"ДРОПАЙ"          , L"hlt"     },
};
const size_t TD_TOKENS_INFO_SIZE = sizeof(TD_TOKENS_INFO)/sizeof(TDtokenTypeInfo_t);

typedef enum nodeType {
    OPERATION_TYPE,
    NUMBER_TYPE,
    PARAM_TYPE,
    EXPRESSION_TYPE,
    LINKER_TYPE,
} nodeType_t;

typedef struct dslParameter {
    const wchar_t* name;
    unsigned int hash;
    int value;
} dslParameter_t;

typedef enum TDexpressionType {
    TD_IF_EXPRESSION_TYPE,
    TD_WHILE_EXPRESSION_TYPE,
    TD_DECLARATION,
} TDexpressionType_t;

union nodeData {
    int number;
    dslParameter_t* parameter;
    TDexpressionType expressionType;
    TDtokenType_t operation;
};

typedef nodeData treeElType_t;

typedef struct treeNode {
    nodeType_t nodeType;
    treeElType_t data;
    treeNode* left;
    treeNode* right;
} treeNode_t;

typedef enum tree_error {
    TR_SUCCESS = 0,
    TR_NULL_PTR,
    TR_INVALID_PTR,
    TR_FILE_NOT_FOUND,
    TR_INVALID_INPUT,
    TR_INVALID_SIZE,
    TR_CANT_OPEN_FILE
} tree_error_t;

#endif //TREE_COMMON_H