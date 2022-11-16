#ifndef SYMBOL_H
#define SYMBOL_H
#include "../ast/ast.h"
#include "../innerCode/innerCode.h"
#define G_SIZE 0X3FFF
#define L_SIZE 50
#define VALUE_SIZE 50
#define STACK_SIZE 300
#define NAME_SIZE 50

typedef enum SymbolType
{
    VAR = 1,
    FUNC,
    FUNC_DEC_,   // dedicates function is declared but not defined(add '_' for distinguishing with NodeType)
    STRUCTURE,
    UNFINISHED_STRUCTURE
}SymbolType;

typedef enum DataType
{
    INT = 1,
    FLOAT,
    STRUCT,
    INT_ARRAY,
    FLOAT_ARRAY = 5,
    STRUCT_ARRAY,
    INT_CONSTANT,// lValue int type, including integer litereal, int type function return value
    FLOAT_CONSTANT, //lValue float type
    ERROR
}DataType;

typedef struct Type
{
   DataType type;
   int dimension;   // dimension of variabel
   struct Symbol* symAddr;  //point to a variable symbol or a struct
}Type;



typedef struct Symbol
{
    char name[NAME_SIZE];
    int lineno;
    SymbolType symType;

    union 
    {
        //variable info
        struct 
        {
            DataType type;
            int dimension;
            int* eachDimSize;
            struct Symbol* structInfo;  

            /*
             address of operand, this is useful to generate 3 address code. 
             e.g: in src code, a variable named 'n' may be an operand 'v1' in 3 address code.
             */
            Operand* op;     
            //char val[VALUE_SIZE];               
        }var;

        //function info
        struct 
        {
            struct Type retType;
            int parac;
            struct Symbol** paras;
        }func;

        // struct info
        struct
        {
            int memsNum;
            int size;
            struct Symbol**members; // store struct members in symbol table(this table has a struct scope)
        }structure;
    };
    //handle conflicts
    struct Symbol* next;

}Symbol;

typedef Symbol** SymbolTable;


typedef struct Stack
{
    void* arr[STACK_SIZE];
    int top;
}Stack;








#endif