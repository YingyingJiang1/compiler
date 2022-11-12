#include "../ast/ast.h"
#include "../semantic/symbol.h"
#include "innerCode.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define handleRead()              \
    Operand *op1 = genTmpVarOp(); \
    genCode(READ, 1, op1);        \
    return op1

#define push(pNode) arr[++top] = pNode

#define pop() arr[top--]

// operands of arithmatic and assign operation can't be address
#define expTypeCheck(op)                  \
    if (op->kind == ADDRESS)              \
        op = genOp(GET_VALUE, &(op->no)); \
    else if (op->kind == ADDRESS_V)       \
    op = genOp(GET_VALUE_V, &(op->no))

// all functions in this file are for generating codes

extern Node *root;
#define MAX_CODE_SIZE 0xffff

int varNO = 1;
int tmpNO = 1;
int labelNO = 1;

InnerCode *codes[MAX_CODE_SIZE];
int codeNum = 0; // the number of codes

Symbol* meaningless;

Symbol *findSymbol(char *name);
int calSize(Symbol *varSym);
Operand *translateExp(Node *exp, Symbol **retSym);
Type findStructMem(Symbol *structSym, char *name);
void genTmpVar(Type *type);

Operand *genOp(int kind, void *val)
{
    Operand *ptr = (Operand *)malloc(sizeof(Operand));
    ptr->kind = kind;
    switch (kind)
    {
    case CONST_INT:
        ptr->constInt = *((int *)val);
        break;
    case CONST_FLOAT:
        ptr->constFloat = *((float *)val);
        break;
    case NAME:
    {
        ptr->name = (char *)malloc(sizeof(char) * (strlen(val) + 1));
        strcpy(ptr->name, (char *)val);
        break;
    }
    default:
        ptr->no = *((int *)val);
        break;
    }
    return ptr;
}

/*
if sym->var.op is NULL, then generate a new Operand and return it
else return sym->var.op
*/
Operand *genVarOp(Symbol *sym)
{
    if (sym)
    {

        if (sym->var.op)
            return sym->var.op;
        else
        {
            Operand *ret = genOp(VARIABLE, &varNO);
            ++varNO;
            sym->var.op = ret;
            return ret;
        }
    }
}


InnerCode *genCode(int kind, int argc, ...)
{
    InnerCode *ptr = (InnerCode *)malloc(sizeof(InnerCode));
    ptr->kind = kind;

    va_list valist;
    va_start(valist, argc);

    switch (argc)
    {
    case 1:
    {
        Operand *op = va_arg(valist, Operand *);
        if (kind == JMP)
            ptr->_3ops.result = op;
        else
            ptr->_3ops.op1 = op;
        break;
    }

    case 2:
        ptr->_3ops.op1 = va_arg(valist, Operand *);
        ptr->_3ops.result = va_arg(valist, Operand *);
        break;
    case 3:
        ptr->_3ops.op1 = va_arg(valist, Operand *);
        ptr->_3ops.op2 = va_arg(valist, Operand *);
        ptr->_3ops.result = va_arg(valist, Operand *);
        break;
    default:
        break;
    }
    codes[codeNum++] = ptr;
    return ptr;
}


void genDEC(Symbol *sym, int size)
{
    Operand *op1 = genOp(VARIABLE, &varNO);
    Operand *result = genOp(CONST_INT, &size);
    genCode(DEC, 2, op1, result);
    ++varNO;
    sym->var.op = op1;
}


// get the kind of arithmatic operation
int getAriOpKind(char *val)
{
    if (val[0] == '+')
        return PLUS;
    if (val[0] == '-')
        return MINUS;
    if (val[0] == '*')
        return MUL;
    if (val[0] == '/')
        return DIV;
}


// get the kind of conditional jmp
int getCJMPkind(char *val)
{
    if (strcmp(val, ">") == 0)
        return JG;
    if (strcmp(val, ">=") == 0)
        return JGE;
    if (strcmp(val, "<") == 0)
        return JL;
    if (strcmp(val, "<=") == 0)
        return JLE;
    if (strcmp(val, "==") == 0)
        return JE;
    if (strcmp(val, "!=") == 0)
        return JNE;
}


Operand *genTmpVarOp()
{
    Operand *op = genOp(TMP_VARIABLE, &tmpNO);
    ++tmpNO;
    return op;
}


/*
get the offset relative to the starting position of the struct
param:  sym -> a pointer of pointer of struct symbol
        name -> name of target struct member
*/
int getOffset(Symbol *sym, char *name)
{
    int off = 0;
    int mems = sym->structure.memsNum;
    Symbol **members = sym->structure.members;
    for (int i = 0; i < mems; ++i)
    {
        if (strcmp(members[i]->name, name) == 0)
        {
            return off;
        }
        off += calSize(members[i]);
    }
}


// get base address of symbol 'sym', only meaningful for struct and array variable
Operand *getBaseAddr(Symbol *sym)
{
    Operand *op = sym->var.op;
    if (op)
    {
        if (op->kind == VARIABLE)
        {
            Operand *op1 = genOp(GET_ADDRESS, &(op->no));
            Operand *result = genOp(ADDRESS, &(tmpNO));
            ++tmpNO;
            genCode(ASSIGN, 2, op1, result);
            sym->var.op = result;
            return result;
        }
        else
            return op;
    }
    else
    {
        Operand *op1 = genOp(GET_ADDRESS, &(varNO));
        Operand *result = genOp(ADDRESS, &(tmpNO));
        ++tmpNO;
        ++varNO;
        genCode(ASSIGN, 2, op1, result);
        sym->var.op = result;
        return result;
    }
}


Operand *handleWrite(Node *exp)
{
    Operand *op1 = translateExp(exp->children[0]->children[0], &meaningless);
    if (op1->kind == ADDRESS || op1->kind == ADDRESS_V)
    {
        Operand *tmp = op1;
        op1 = genOp(TMP_VARIABLE, &tmpNO);
        genCode(ASSIGN, 2, genOp(GET_VALUE, &(tmp->no)), op1);
    }
    genCode(WRITE, 1, op1);
    int zero = 0;
    return genOp(CONST_INT, &zero);
}


Operand *genLABEL()
{
    Operand *op1 = genOp(LABEL_NO, &labelNO);
    ++labelNO;
    genCode(LABEL, 1, op1);
    return op1;
}


void add2list(CodeList *listHead, InnerCode *pCode)
{
    CodeList ptr = (CodeList)malloc(sizeof(CodeListNode));
    ptr->pCode = pCode;
    ptr->next = *listHead;
    *listHead = ptr;
}


// merge list1 and list2
CodeList mergeList(CodeList list1, CodeList list2)
{
    if (list1 == NULL)
        return list2;
    CodeList cur = list1;
    while (cur->next)
        cur = cur->next;
    cur->next = list2;
    return list1;
}


void backpatch(CodeList list, Operand *label)
{
    CodeList head = list;
    while (head)
    {
        head->pCode->_3ops.result = label;
        head = head->next;
    }
}


void translateBoolExp(Node *exp, CodeList *tlist, CodeList *flist)
{
    if (exp->type == RELOP_OP)
    {
        Operand *op1 = translateExp(exp->children[0], &meaningless);
        Operand *op2 = translateExp(exp->children[1], &meaningless);
        int kind = getCJMPkind(exp->val);
        expTypeCheck(op1);
        expTypeCheck(op2);
        add2list(tlist, genCode(kind, 3, op1, op2, NULL));
        add2list(flist, genCode(JMP, 1, NULL));
        return;
    }
    // if logical operator is '!', then exchange truelist and falselist
    if (exp->val[0] == '!')
    {
        CodeList tmp = *tlist;
        *tlist = *flist;
        *flist = tmp;
        return;
    }
    if (exp->type == IDENTIFIER)
    {
        int val = 0;
        Symbol *sym = findSymbol(exp->val);
        Operand *op1 = genVarOp(sym);
        Operand *op2 = genOp(CONST_INT, &val);
        expTypeCheck(op1);
        add2list(tlist, genCode(JNE, 3, op1, op2, NULL));
        add2list(flist, genCode(JMP, 1, NULL));
        return;
    }
    if (exp->type == INTEGER)
    {
        int val = atoi(exp->val);
        if (val == 0)
            add2list(flist, genCode(JMP, 1, NULL));
        else
            add2list(tlist, genCode(JMP, 1, NULL));
        return;
    }
    if (exp->type == FLOAT_POINT)
    {
    }

    if (strcmp(exp->val, "&&") == 0)
    {
        CodeList tlist1 = NULL, flist1 = NULL;
        CodeList tlist2 = NULL, flist2 = NULL;
        translateBoolExp(exp->children[0], &tlist1, &flist1);

        Operand *label = genLABEL();
        backpatch(tlist1, label);
        translateBoolExp(exp->children[1], &tlist2, &flist2);

        *tlist = tlist2;
        *flist = mergeList(flist1, flist2);
    }
    else if (strcmp(exp->val, "||") == 0)
    {
        CodeList tlist1 = NULL, flist1 = NULL;
        CodeList tlist2 = NULL, flist2 = NULL;
        translateBoolExp(exp->children[0], &tlist1, &flist1);

        Operand *label = genLABEL();
        backpatch(flist1, label);
        translateBoolExp(exp->children[1], &tlist2, &flist2);

        *tlist = mergeList(tlist1, tlist2);
        *flist = flist2;
    }
}


/*
para:
    exp: root of expression ast
    retSym: a pointer of pointer of a variable symbol.
*/
Operand *translateExp(Node *exp, Symbol **retSym)
{
    // base case
    if (exp->type == IDENTIFIER)
    {
        Symbol *sym = findSymbol(exp->val);
        return genVarOp(sym);
    }
    else if (exp->type == INTEGER)
    {
        int val = atoi(exp->val);
        return genOp(CONST_INT, &val);
    }
    else if (exp->type == FLOAT_POINT)
    {
        float val = atof(exp->val);
        return genOp(CONST_FLOAT, &val);
    }

    switch (exp->type)
    {
    case ARITHMATIC_OP:
    {
        int kind = getAriOpKind(exp->val);
        int zero = 0;
        Operand *op1, *op2, *result;
        if (exp->num == 1)
        {
            op1 = genOp(CONST_INT, &zero);
            op2 = translateExp(exp->children[0], &meaningless);
        }
        else
        {
            op1 = translateExp(exp->children[0], &meaningless);
            op2 = translateExp(exp->children[1], &meaningless);
        }

        expTypeCheck(op1);
        expTypeCheck(op2);
        result = genOp(TMP_VARIABLE, &tmpNO);
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
        Operand *base;
        Operand *op1, *op2, *result;        
        Symbol *sym;
        int width;
        int type = exp->children[0]->type;
        char *name;
        if (type == IDENTIFIER)
        {
            name = exp->children[0]->val;
            sym = findSymbol(name);
            base = getBaseAddr(sym);
            *retSym = sym;
            goto L_IN_ARRAY_REF;
        }
        else
            base = translateExp(exp->children[0], &sym);

        if (type == MEMBER_ACCESS_OP)
        {
            *retSym = sym;            
        }
        else
        {
            printf("Array reference translation is not accounted for.\n");
            abort();
        }

        L_IN_ARRAY_REF: 
        // get the number of elements        
        result = op1 = translateExp(exp->children[1], &meaningless);        
        for (int i = 2; i < exp->num; ++i)
        {
            /*
            for a Operand type varible 'op', union domain of op is 0 if and only if op.kind = CONST_INT and op.constInt = 0.
            there is no need to execute multiple operation when following condition meets.
            */
            if (op1->constInt == 0)
                goto L;

            op2 = genOp(CONST_INT, &(sym->var.eachDimSize[i - 1]));
            result = genTmpVarOp();
            genCode(MUL, 3, op1, op2, result);

            op1 = result;
        L:
            op2 = translateExp(exp->children[i], &meaningless);
            expTypeCheck(op2);
            // there is need to execute plus operation when following condition meets
            if (op1->constInt == 0 && op2->constInt == 0)
                continue;
            if (op1->constInt == 0)
            {
                op1 = op2;
                continue;
            }
            if (op2->constInt == 0)
                continue;
            result = genTmpVarOp();
            genCode(PLUS, 3, op1, op2, result);
            op1 = result;
        }

        if (op1->constInt == 0)
        {
            return base;
        }

        // get offset relative to base address
        if (sym->var.type == STRUCT)
            width = sym->var.structInfo->structure.size;
        else
            width = 4;
        op2 = genOp(CONST_INT, &width);
        result = genTmpVarOp();
        genCode(MUL, 3, op1, op2, result);

        // base + offset
        op1 = base;
        op2 = result;
        result = genOp(ADDRESS, &tmpNO);
        ++tmpNO;
        genCode(PLUS, 3, op1, op2, result);
        return result;
    }

    /*
    The first child node of 'exp' may be IDENTIFIER, ARRAY_REFERENCE, FUNC_CALL, MEMBER_ACCESS_OP.
    For each struct member accessed, we need get start address of the struct variable(base address),
    and then get offset of the struct member relative to the base address. To get offset, we must need the struct symbol, this is
    passed by parameter of 'translateExp': retSym.
    */
    case MEMBER_ACCESS_OP:
    {
        Operand *base;
        Symbol *sym;
        char *name;
        int type = exp->children[0]->type;
        if (type == IDENTIFIER)
        {
            Symbol *mem;
            Type ret;

            // get base address
            name = exp->children[0]->val;
            sym = findSymbol(name);
            base = getBaseAddr(sym);

            /*
            pass the struct member(a variable symbol) currently being accessed.
            e.g: a.b.c, we need get symbol 'b', then we can get offset of 'c' relative to 'b'.
            */
            ret = findStructMem(sym->var.structInfo, exp->children[1]->val);
            *retSym = ret.symAddr;
            goto L_IN_MEM_ACCESS;
        }
        else
            base = translateExp(exp->children[0], &sym);

        // set value of 'retSym'
        if (type == MEMBER_ACCESS_OP)
        {
            Type ret = findStructMem(sym->var.structInfo, exp->children[1]->val);
            *retSym = ret.symAddr;
            goto L_IN_MEM_ACCESS;
        }
        else if (type == ARRAY_REFERENCE)
        {
            *retSym = sym;
            goto L_IN_MEM_ACCESS;
        }
        else if (type == FUNC_CALL)
        {
            Type type;
            name = exp->children[0]->children[0]->val;
            sym = findSymbol(name);
            type = sym->func.retType;
            genTmpVar(&type);
            sym = type.symAddr;
            *retSym = sym;
            goto L_IN_MEM_ACCESS;
        }
        else
        {
            printf("Struct access translations are not accounted for.\n");
            abort();
        }

    // get address of struct member currently being accessed
    L_IN_MEM_ACCESS:
        name = exp->children[1]->val; // get name of struct member
        sym = sym->var.structInfo;
        int off = getOffset(sym, name);

        Operand *result = genOp(ADDRESS, &tmpNO);
        ++tmpNO;
        genCode(PLUS, 3, base, genOp(CONST_INT, &off), result);
        return result;
    }

    case RELOP_OP:
    case LOGICAL_OP:
    {
        int trueVal = 1, falseVal = 0;
        CodeList tlist = NULL, flist = NULL;
        Operand *result = genTmpVarOp();
        translateBoolExp(exp, &tlist, &flist);

        Operand *label1 = genLABEL();
        genCode(ASSIGN, 2, genOp(CONST_INT, &trueVal), result);
        backpatch(tlist, label1);

        Operand *label2 = genLABEL();
        genCode(ASSIGN, 2, genOp(CONST_INT, &falseVal), result);
        backpatch(flist, label2);
        return result;
    }

    case FUNC_CALL:
    {

        if (strcmp(exp->children[0]->val, "read") == 0)
        {
            handleRead();
        }

        if (strcmp(exp->children[0]->val, "write") == 0)
        {
            return handleWrite(exp);
        }
        Node *argList = exp->children[0];
        Symbol *sym = findSymbol(exp->children[0]->val);
        Symbol **paras = sym->func.paras;
        for (int i = argList->num - 1; i >= 0; --i)
        {
            Operand *op = translateExp(argList->children[i], &meaningless);
            /*
            if parameter is array or struct, then argument should be passed by address
            else passing argument by value
            */
            if (paras[i]->var.dimension > 0 || paras[i]->var.type == STRUCT)
            {
                if (op->kind != ADDRESS && op->kind != ADDRESS_V)
                    op = genOp(GET_ADDRESS, &(op->no));
            }
            else
            {
                if (op->kind == ADDRESS)
                    op = genOp(GET_VALUE, &(op->no));
                else if (op->kind == ADDRESS_V)
                    op = genOp(GET_VALUE_V, &(op->no));
            }
            genCode(ARG, 1, op);
        }
        Operand *op1 = genOp(NAME, exp->children[0]->val);
        Operand *result = genTmpVarOp();
        genCode(CALL, 2, op1, result);
        return result;
    }

    case ASSIGN_OP:
    {
        Operand *result = translateExp(exp->children[0], &meaningless);
        Operand *op1 = translateExp(exp->children[1], &meaningless);
        expTypeCheck(op1);
        expTypeCheck(result);
        genCode(ASSIGN, 2, op1, result);
        return result;
    }

    default:
        break;
    }
}
