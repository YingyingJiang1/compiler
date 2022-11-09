#include "../ast/ast.h"
#include "../semantic/symbol.h"
#include "innerCode.h"
#include <stdarg.h>
#include <string.h>


// all functions in this file are for generating codes

extern Node* root;
#define MAX_CODE_SIZE 0xffff

int varNO = 1;
int tmpNO = -1;
int labelNO = 1;

static int top = -1;

InnerCode* codes[MAX_CODE_SIZE];
int codeNum = 0;    // the number of codes

Symbol* findSymbol(char* name);
int calSize(Symbol* varSym);

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
    {
        ptr->name = (char*)malloc(sizeof(char)*strlen(val));
        strcpy(ptr->name, (char*)val);        
    }        
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

    switch(argc)
    {
        case 1:       
        {
            Operand* op = va_arg(valist, Operand*);
            if(kind == JMP)
                ptr->_3ops.result = op;
            else
                ptr->_3ops.op1 = op;
            break;
        }         
            
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
    codes[codeNum++] = ptr;
    return ptr;
}



//inline ??
void genDEC(Symbol* sym, int size)
{    
    Operand* op1 = genOp(VARIABLE, &varNO);
    Operand* op2 = genOp(CONST_INT, &size);
    codes[codeNum++] = genCode(DEC, 2, op1, op2);    
    ++varNO;
    sym->var.op = op1;
}

// get the kind of arithmatic operation
int getAriOpKind(char* val)
{
    if(val[0] == '+')
        return PLUS;
    if(val[0] == '-')
        return MINUS;
    if(val[0] == '*')
        return MUL;
    if(val[0] == '/')
        return DIV;
}

// get the kind of conditional jmp
int getCJMPkind(char*val)
{
    if(strcmp(val, ">") == 0)
        return JG;
    if(strcmp(val, ">=") == 0)
        return JGE;
    if(strcmp(val, "<") == 0)
        return JL;
    if(strcmp(val, "<=") == 0)
        return JLE;
    if(strcmp(val, "==") == 0)
        return JE;
    if(strcmp(val, "!=") == 0)
        return JNE;
}


Operand* genTmpVar()
{
    Operand* op = genOp(VARIABLE, &tmpNO);
    ++tmpNO;
    return op;
}


inline void push(Node** stack, Node* pNode)
{
    stack[++top] = pNode;
}

inline Node* pop(Node** stack)
{
    --top;
    return stack[top+1];
}

inline int isempty()
{
    if(top == -1)
        return 1;
    return 0;
}

// !!!!!!! i don't know how to clearly state the function of this function
// get offset of struct member and set value of 'sym' to a struct
int getOffset(Symbol** sym, char* name)
{
    int off = 0;
    int mems = (*sym)->structure.memNums;
    Symbol** members = (*sym)->structure.members;
    for(int i = 0; i < mems; ++i)
    {
        if(strcmp(members[i]->name, name) == 0)
        {
            *sym = members[i]->var.structInfo;
            return off;
        }
        off += calSize(sym);
    }
}

Operand* getMemAddr(Node* pNode)
{
    Node** arr[100];
    push(arr, pNode);
    while(pNode->children[0] == MEMBER_ACCESS_OP)
    {
        pNode = pNode->children[0];
        push(arr, pNode);
    }
    
    Node* cur = pop(arr);    
    Symbol* sym = findSymbol(cur->children[0]->val);
    Operand* base = genVarOp(sym);
    int off = getOffset(&sym, cur->children[1]->val);
    while(!isempty(arr))
    {
        cur = pop(arr);
        off += getOffset(&sym, cur->children[1]->val);
    }

    Operand* result = genOp(ADDRESS, &tmpNO);
    ++tmpNO;
    genCode(ASSIGN, 3, base, genOp(CONST_INT, &off), result);
    return result;

}

Operand* translateExp(Node* exp)
{
    if(exp->type == IDENTIFIER)
    {
        Symbol* sym = findSymbol(exp->val);
        return genVarOp(sym);
    }
    else if(exp->type == INT_CONSTANT)
    {
        int val = atoi(exp->val);
        return genOp(CONST_INT, &val);
    }
    else if(exp->type == FLOAT_CONSTANT)
    {
        float val = atof(exp->val);
        return genOp(CONST_FLOAT, &val);
    }

    switch (exp->type)
    {
    case ARITHMATIC_OP:
    {
        int kind = getAriOpKind(exp->val);
        Operand* op1 = translateExp(exp->children[0]);
        Operand* op2 = translateExp(exp->children[1]);
        Operand* result = genOp(VARIABLE, &tmpNO);
        ++tmpNO;
        genCode(kind, 3, op1, op2, result);
        return result;
    }
        
    /*
    address of k-dimension array A[i1][i2]...[ik]: base + ((i1*n2)+i2)*n3+i3)...)*nk+ik)*w
    ni is the number of elements in ith dimension, w is width of each element
    */ 
    case ARRAY_REFERENCE:
    {
        Symbol* sym = findSymbol(exp->children[0]->val);
        // base address of array
        Operand* base = genVarOp(sym);

        // get offset
        Operand* op1, *op2, *result;
        result = op1 = translateExp(exp->children[1]);
        int width;
        for(int i = 2; i < exp->num - 1; ++i)
        {
            op2 = genOp(CONST_INT, &(sym->var.eachDimSize[i+1]));
            result = genTmpVar();
            genCode(MUL, 3, op1, op2, result);
            
            op1 = result;
            op2 = translateExp(exp->children[i]);
            result = genTmpVar();
            genCode(PLUS, 3, op1, op2, result);

        }
        op1 = result;
        if(sym->var.type == STRUCT)
            width = sym->var.structInfo->structure.size;
        else
            width = 4;
        op2 = genOp(CONST_INT, &width);
        result = genTmpVar();
        genCode(MUL, 3, op1, op2, result);

        // base + offset
        op1 = base;
        op2 = result;
        result = genOp(ADDRESS, &tmpNO);
        ++tmpNO;
        genCode(PLUS, 3, op1, op2, result);

        return result;
    }

    case MEMBER_ACCESS_OP:
    {
        return getMemAddr(exp);
    }

    case RELOP_OP:
    case LOGICAL_OP:
    {
        int trueVal = 1, falseVal = 0;
        CodeList tlist = NULL, flist = NULL;
        Operand* result = genTmpVar();
        translateBoolExp(exp, &tlist, &flist);

        Operand* label1 = genLABEL();
        genCode(ASSIGN, 2, genOp(CONST_INT, &trueVal), result);
        backpatch(tlist, label1);

        Operand* label2 = genLABEL();
        genCode(ASSIGN, 2, genOp(CONST_INT, &falseVal), result);
        backpatch(flist, label2);
        return result;
    }

    

    case FUNC_CALL:
    {
        Node* argList = exp->children[0];
        for(int i = 0; i < argList->num; ++i)
        {   
            Operand* op = translateExp(argList->children[i]);
            genCode(ARG, 1, op);
        }
        Operand* op1 = genOp(NAME, exp->children[0]->val);
        Operand* result = genTmpVar();
        genCode(CALL, 2, op1, result);
        return result;
    }

    case ASSIGN_OP:
    {
        Operand* result = translateExp(exp->children[0]);
        Operand* op1 = translateExp(exp->children[1]);
        genCode(ASSIGN, 2, op1, result);
        return result;
    }
    
    default:
        break;
    }
}

// inline ??
void genRETURN(Node* exp)
{
    Operand* op1 = genCodeforExp(exp);

    codes[codeNum++] = genCode(RETURN, 1, op1);
}


Operand* genLABEL()
{
    Operand* result = genOp(LABEL_NO, &labelNO);
    ++labelNO;    
    codes[codeNum++] = genCode(LABEL, 1, result);
}

void add2list(CodeList* listHead, InnerCode* pCode)
{
    CodeList ptr = (CodeList)malloc(sizeof(CodeListNode));
    ptr->pCode = pCode;
    ptr->next = *listHead;
    *listHead = ptr;
}

// merge list1 and list2 
CodeList mergeList(CodeList list1, CodeList list2)
{
    if(list1 == NULL)
        return list2;
    CodeList cur = list1;
    while(cur->next)
        cur = cur->next;
    cur->next = list2;
    return list1;

}

void translateBoolExp(Node* exp, CodeList* tlist, CodeList* flist)
{
    if(exp->type == RELOP_OP)
    {
        Operand* op1 = translateExp(exp->children[0]);
        Operand* op2 = translateExp(exp->children[1]);
        int kind = getCJMPkind(exp->val);
        add2list(tlist, genCode(kind, 3, op1, op2, NULL));
        add2list(flist, genCode(JMP, 1, NULL));
        return;
    }
    // if logical operator is '!', then exchange truelist and falselist
    if(exp->val[0] == '!')
    {
        CodeList tmp = *tlist;
        *tlist = *flist;
        *flist = tmp;
    }
    if(exp->type == IDENTIFIER)
    {
        int val = 0;
        Symbol* sym = findSymbol(exp->val);
        Operand* op1 = genVarOp(sym);
        Operand* op2 = genOp(CONST_INT, &val);
        add2list(tlist, genCode(JNE, 3, op1, op2, NULL));
        add2list(flist, genCode(JMP, 1, NULL));
        return;
    }
    if(exp->type == INTEGER || exp->type == FLOAT_POINT)
    {
        int val = 0;
        float val = atof(exp->val);
        if(val == 0)
            add2list(flist, genCode(JMP, 1, NULL));
        else
            add2list(tlist, genCode(JMP, 1, NULL));                
        return;        
    }
    
    if(strcmp(exp->val, "&&") == 0)
    {        
        CodeList tlist1 = NULL, flist1 = NULL;
        CodeList tlist2 = NULL, flist2 = NULL;
        translateBoolExp(exp->children[0], &tlist1, &flist1);

        Operand* label = genLABEL();
        backpatch(tlist1, label);
        translateBoolExp(exp->children[1], &tlist2, &flist2);

        *tlist = tlist2;
        *flist = mergeList(flist1, flist2);

    }
    else if(strcmp(exp->val, "||") == 0)
    {
        CodeList tlist1 = NULL, flist1 = NULL;
        CodeList tlist2 = NULL, flist2 = NULL;
        translateBoolExp(exp->children[0], &tlist1, &flist1);

        Operand* label = genLABEL();
        backpatch(flist1, label);
        translateBoolExp(exp->children[1], &tlist2, &flist2);

        *tlist = mergeList(tlist1, tlist2);
        *flist = flist2;
    }



}

void backpatch(CodeList list, Operand* label)
{
    CodeList head = list;
    while(head)
    {
        head->pCode->_3ops.result = label;
        head = head->next;
    }

}


int genGOTO(Operand* label)
{
    Operand* result = genOp(LABEL_NO, &labelNO);
    codes[codeNum++] = genCode(LABEL, 1, result);
    ++labelNO;
    return codeNum-1;
}




/*
if sym->var.op is NULL, then generate a new Operand and return it
else return sym->var.op
*/
Operand* genVarOp(Symbol* sym)
{
    if(sym)
    {
        if(sym->var.op)
            return sym->var.op;
        else
        {
            Operand* ret;
            // variable is a struct or an array, then get address of it 
            if(sym->var.dimension > 0 || sym->var.type == STRUCT)
                ret = genOp(GET_ADDRESS, &varNO);
            else
                ret = genOp(VARIABLE, &varNO);
            ++varNO;
            sym->var.op = ret;
            return ret;
        }
        
        
    }
        
}



