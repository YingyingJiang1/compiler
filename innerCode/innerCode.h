#ifndef INNER_CODE
#define INNER_CODE

// struct of operand
typedef struct Operand
{
    /*
    domain 'kind' determines how to translate union type and 
    what prefix to be added for the operand when output 3-address code.
    The correspondence between the 'kind' and union domain:
    NAME: char* name
    CONST_INT: int cosntInt
    CONST_FLOAT: float contFloat
    others: int no
    */
    enum {VARIABLE, TMP_VARIABLE, CONST_INT, CONST_FLOAT, LABEL_NO, 
        NAME, ADDRESS, ADDRESS_V, GET_ADDRESS, GET_VALUE, GET_VALUE_V} kind;
    union 
    {
        char* name;     // remember to free when free struct Operand
        float constFloat;  
        int no;
        int addr;
        int constInt;      
    };
    
}Operand;

// struct of three address code
typedef struct InnerCode
{
    enum {ASSIGN, PLUS, MINUS, MUL, DIV, AND, OR, NOT, PARAM, ARG, 
        CALL, READ, WRITE, LABEL, FUNCTION, RETURN, DEC, JMP, JG, JGE, 
        JL, JLE, JE, JNE} kind;
    struct {Operand* result, *op1, *op2;} _3ops; 

}InnerCode;

typedef struct CodeListNode
{
    InnerCode* pCode;
    struct CodeListNode* next;
}CodeListNode;

typedef CodeListNode* CodeList;


void gen3addrCode();

#endif