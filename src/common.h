#ifndef TREE_COMMON_H
#define TREE_COMMON_H

#include <stdio.h>
#include <io.h>
#include <math.h>
#include <cassert>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>


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

const int MAX_LINE_LENGTH = 1000;

// const char* const TD_FILE_PATH = "../files/durden/kvadratka.bb";
const char* const TD_FILE_PATH = "../files/durden/factorial.bb";

typedef enum TDtokenType {
    TD_PLUS           ,
    TD_MINUS          ,
    TD_MULTIPLY       ,
    TD_DIVIDE         ,
    TD_LESS_THAN      ,
    TD_NOT_EQUALS     ,
    TD_IF             ,
    TD_WHILE          ,
    TD_SEMICOLON      ,
    TD_EQUALS         ,
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
} TDtokenTypeInfo_t;

const TDtokenTypeInfo_t TD_TOKENS_INFO[] = {
    {TD_PLUS,            L"+"               },
    {TD_MINUS,           L"-"               },
    {TD_MULTIPLY,        L"*"               },
    {TD_DIVIDE,          L"/"               },
    {TD_LESS_THAN,       L"ВОТАФА"          },
    {TD_NOT_EQUALS,      L"ОПП"             },
    {TD_IF,              L"ШНЕЙНЕ"          },
    {TD_WHILE,           L"ЕЩКЕРЕ"          },
    {TD_SEMICOLON,       L"ФА"              },
    {TD_EQUALS,          L"ПЕПЕ"            },
    {TD_OPENING_BRACKET, L"ЛЕВАЯ_НОГА"      },
    {TD_CLOSING_BRACKET, L"ПРАВАЯ_НОГА"     },
    {TD_INPUT,           L"ХОЧУ_ВЫСТРЕЛ"    },
    {TD_PRINT,           L"ЕСТЬ_ЧТО_СКАЗАТЬ"},
    {TD_SQRT,            L"АЙСГЕРГЕРБЕР"    },
    {TD_HLT,             L"ДРОПАЙ"          },
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