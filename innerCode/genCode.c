#include "../ast/ast.h"
#include "../sematic/st.h"
#include "innerCode.h"
#include <stdarg.h>
#include <string.h>


// all functions in this file are for generating codes

extern Node* root;
#define MAX_CODE_SIZE 0xffff

int varNO = 1;
int tmpNO = -1;
int labelNO = 1;

InnerCode* codes[MAX_CODE_SIZE];
int codeNum = 0;    // the number of codes

Operand* genOp(int kind, void* val)
{
    Operand* ptr = (Operand*)malloc(sizeof(Operand));
    ptr->kind = kind;
    switch (kind)
    {
    case CONST_INT:
        ptr->constInt = *((int*)val);
        break;
    case CONST_FLOAT:
        ptr->constFloat = *((float*)val);
    case NAME:
        strcpy(ptr->name, (char*)val);
    default:
        ptr->no = *((int*)val);
        break;
    }

    return ptr;
}

InnerCode* genCode(int kind, int argc, ...)
{
    InnerCode* ptr = (InnerCode*)malloc(sizeof(InnerCode));
    ptr->kind = kind;

    va_list valist;
    va_start(valist, argc);
    
    if(kind == CJMP)
    {
        ptr->ops.code = va_arg(valist, InnerCode*);
        ptr->ops.code = va_arg(valist, Operand*);
    }
    else
    {
        switch(argc)
        {
            case 1:                
                ptr->_3ops.op1 = va_arg(valist, Operand*);
                break;
            case 2:
                ptr->_3ops.op1 = va_arg(valist, Operand*);
                ptr->_3ops.result = va_arg(valist, Operand*);
                break;
            case 3:
                ptr->_3ops.op1 = va_arg(valist, Operand*);
                ptr->_3ops.op2 = va_arg(valist, Operand*);
                ptr->_3ops.result = va_arg(valist, Operand*);
                break;
            default:
                break;
        }
    }
    return ptr;
}

/*
def->type = FUNC_DEF
*/
void genFuncHead(Node* def)
{
    // generate codes for function head(function name and function params)
    Node* funcHead = def->children[1];
    Operand* op1 = genOp(NAME, funcHead->val);
    codes[codeNum++] = genCode(FUNCTION, 1, op1);
    for(int i = 0; i < funcHead->num; ++i)
    {
        
        Operand* op1 = genOp(VARIABLE, varNO++);
        codes[codeNum++] = genCode(PARAM, 1, op1);
    }

    // generate codes for function body
    for(int i = 0; i < def->num; ++i)
    {

    }
}

int calStructSize(Symbol* sym)
{
    int size = 0;
    Symbol** members = sym->structure.members;
    for(int i = 0; i < sym->structure.memNums; ++i)
    {
        Type type;
        type.type = members[i]->var.type;
        type.symAddr = members[i]->var.structInfo;
        size += calVarSize(&type, members[i]);
    }
    return size;

}

int calVarSize(Type* type, Symbol* varSym)
{
    int size = 0, unit, num = 1;
    if(type->type == STRUCT)
        unit = calStructSize(type->symAddr);
    else
        unit = 4;
    
    for(int i = 0; i < varSym->var.dimension; ++i)
    {
        num *= varSym->var.eachDimSize[i];
    }
    size = unit * num;
    return size;
}

void genDEC(Type* type, Symbol* varSym)
{
    // calculate size
    int size = calVarSize(type, varSym);    
    
    Operand* op1 = genOp(VARIABLE, varNO++);
    Operand* op2 = genOp(CONST_INT, size);
    codes[codeNum++] = genCode(DEC, 2, op1, op2);
}