#ifndef INNER_CODE
#define INNER_CODE

// struct of operand
typedef struct Operand
{
    enum {VARIABLE, CONST_INT, CONST_FLOAT, LABEL_NO, NAME} kind;
    union 
    {
        char name[64]; 
        float constFloat;  
        int no;
        int constInt,        
    };
    
}Operand;

// struct of three address code
typedef struct InnerCode
{
    enum {ASSIGN, PLUS, MINUS, MUL, DIV, AND, OR, NOT, PARAM, ARG, CALL, 
        READ, WRITE, LABEL, FUNCTION, GET_ADDRESS, GET_VALUE, RETURN, DEC, JMP, CJMP} kind;
    union
    {
        struct{Operand* result, *op1, *op2} _3ops;
        struct {InnerCode* code, Operand* result} ops;
    };    

}InnerCode;


void gen3addrCode();

#endif