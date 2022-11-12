#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errorType.h"
#include "symbol.h"
#include "st.h"
#include "../innerCode/genCode.h"

#define isInt(expType) expType.type == INT || expType.type == INT_CONSTANT || \
                           (expType.type == INT_ARRAY && expType.dimension == 0)
#define isFLoat(expType) expType.type == FLOAT || expType.type == FLOAT_CONSTANT || \
                             (expType.type == FLOAT_ARRAY && expType.dimension == 0)
#define isStructType(expType) expType.type == STRUCT || expType.type == STRUCT_ARRAY &&expType.dimension == 0
#define isArray(symPtr) symPtr->var.type == INT_ARRAY || symPtr->var.type == FLOAT_ARRAY || symPtr->var.type == STRUCT_ARRAY
#define assignType(expRet, dataType, addr) \
    expRet->type = dataType;               \
    expRet->symAddr = addr

#define ERROR_OCCURRED_THEN_EXIT                          \
    if (exp1Type.type == ERROR || exp2Type.type == ERROR) \
    {                                                     \
        assignType(expRet, ERROR, NULL);                  \
        return;                                           \
    }

#define varDataType(symInfo) symInfo->var.type
#define isArrayType(dataType) (dataType) == INT_ARRAY || (dataType) == FLOAT_ARRAY || (dataType) == STRUCT_ARRAY


int errorOccurred = 0;
int nameCount = 0;
Symbol *globalST[G_SIZE];
char *funcsName[1000];

extern Symbol* meaningless;
extern Stack scope;

void getRetType(Node *typeNode, Type *type);
void checkExp(Node *exp, Type *expRet);
void checkCompSt(Node *compSt, Type retType);
void checkStmt(Node *stmt, Type retType);
void defFunc(Node *funcDef);
void defVar(SymbolTable st, int tSize, Node *varDef);
int equal(Type type1, Type type2);
int isStructEqual(Symbol *s1, Symbol *s2);
int retValueMatch(Type type1, Type type2);
int parasTypeConsistent(Symbol *sym, Node *paraList);




// add predefined function "read" and "write"
void addPDfunc(int flag)
{
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    memset(sym, 0, sizeof(Symbol));
    sym->symType = FUNC;
    int i;

    if(flag == 0)
    {        
        strcpy(sym->name, "read");
        sym->func.parac = 0;
        sym->func.retType.type = INT;
        sym->func.retType.symAddr = NULL;
        i = hash_pjw("read", G_SIZE);
    }
    else
    {
        strcpy(sym->name, "write");
        sym->func.parac = 1;
        sym->func.paras = (Symbol**)malloc(sizeof(Symbol*));
        Symbol* var = (Symbol*)malloc(sizeof(Symbol));
        var->symType = VAR;
        var->var.type = INT;
        sym->func.paras[0] = var;
        sym->func.retType.type = INT;
        sym->func.retType.symAddr = NULL;
        i = hash_pjw("write", G_SIZE);
    }
    sym->next = globalST[i];
    globalST[i] = sym;
}

// root of ast is an EXT_DEF_LIST, its children node is either GLOABAL_VAR_DEF type or FUNC_DEF type.
void checkProgram(Node *ast)
{
    memset(globalST, 0, sizeof(Symbol *) * G_SIZE);
    scope.top = -1;
    memset(scope.arr, 0, sizeof(void *) * STACK_SIZE);
    push(globalST, G_SIZE);

    // add read()
    addPDfunc(0);
    // add write(int)
    addPDfunc(1);

    // handle all defination first.
    for (int i = 0; i < ast->num; ++i)
    {
        Node *cur = ast->children[i];
        switch (cur->type)
        {
        // GLOBAL_VAR_DEF node has at least one child:first is data type(BASE_TYPE or STRUCT_TYPE),
        // other are variables(IDENTIFIER) to be defined.
        case GLOBAL_VAR_DEF:
        {
            defVar(globalST, G_SIZE, cur);
            break;
        }

        // FUNC_DEF node has three nodes: first is type of return value(BASE_TYPE or TYPE_STRUCT),
        // second is name of func(IDENTIFIER, note: paras of func are children of the second node), third is body of func(COMP_ST).
        case FUNC_DEF:
        {

            defFunc(cur);
            break;
        }

        case FUNC_DEC:
        {
            Type retType;
            getRetType(cur->children[0], &retType);
            // only struct return type will cause ERROR type,
            // the reason for printing error info here instead of in getType is getType is called elsewhere
            if (retType.type == ERROR)
                printf("Error type %d at Line %d: struct \"%s\" hasn't been defined yet.\n",
                       UNDEFINED_STRUCT, cur->children[0]->children[0]->lineNo, cur->children[0]->children[0]->val);
            Symbol *sym = findSymbol(cur->children[1]->val);

            // duplicated function declarations, check whether declarations are consistent
            if (sym)
            {
                if (!retValueMatch(retType, sym->func.retType) || sym->func.parac != cur->children[1]->num || !parasTypeConsistent(sym, cur->children[1]))
                    printf("Error type %d at Line %d: Inconsistent declaration of function \"%s\"\n.",
                           INCONSISTENT_DECLARATION, cur->children[1]->lineNo, cur->children[1]->val);
            }
            // first declaration of function, add function to symbol table but set symType to "FUNC_DEC"
            else
            {
                funcsName[nameCount++] = cur->children[1]->val;
                addFunc(retType, cur->children[1], FUNC_DEC_);
            }

            break;
        }
        default:
            break;
        }
    }
    // check body of all defined functions
    for (int i = 0; i < ast->num; ++i)
    {
        Node *def = ast->children[i];
        if (def->type == FUNC_DEF)
        {
            Type retType;
            getRetType(def->children[0], &retType);
            checkCompSt(def, retType);
        }
    }

    // check function declared but not defined
    for (int i = 0; i < nameCount; ++i)
    {
        Symbol *sym = findSymbol(funcsName[i]);
        if (sym && sym->symType == FUNC_DEC_)
            printf("Error type %d at Line %d: undefined function \"%s\".\n",
                   FUNC_DECLARED_BUT_UNDEFINED, sym->lineno, sym->name);
    }
}

int retValueMatch(Type type1, Type type2)
{
    if (type1.type != type2.type)
        return 0;
    if (type1.type == STRUCT && !isStructEqual(type1.symAddr, type2.symAddr))
        return 0;
    return 1;
}

int parasTypeConsistent(Symbol *sym, Node *paraList)
{
    Symbol *varSym;
    for (int i = 0; i < paraList->num; ++i)
    {
        Type type, type1, type2;
        Node *idNode, *typeNode;
        typeNode = paraList->children[i];
        getType(typeNode, &type);
        if (type.type == STRUCT)
            idNode = typeNode->children[typeNode->num - 1];
        else
            idNode = typeNode->children[0];
        varSym = genVarSym(idNode, type);

        type1.type = varSym->var.type;
        type1.symAddr = varSym;
        type2.type = sym->func.paras[i]->var.type;
        type2.symAddr = sym->func.paras[i];

        if (!equal(type1, type2))
            return 0;
    }
    return 1;
}

void defFunc(Node *funcDef)
{
    Node *cur = funcDef;
    Type retType;
    getRetType(cur->children[0], &retType);
    // only struct return type will cause ERROR type, the reason for printing error info here instead of in getType is getType is called elsewhere
    if (retType.type == ERROR)
        printf("Error type %d at Line %d: struct \"%s\" hasn't been defined yet.\n",
               UNDEFINED_STRUCT, cur->children[0]->children[0]->lineNo, cur->children[0]->children[0]->val);
    Symbol *sym = findSymbol(cur->children[1]->val);
    if (sym)
    {
        if (sym->symType == FUNC_DEC_)
        {
            if (!retValueMatch(retType, sym->func.retType) || sym->func.parac != cur->children[1]->num || !parasTypeConsistent(sym, cur->children[1]))
            {
                printf("Error type %d at Line %d: Inconsistent declaration and defination of function \"%s\".\n",
                       INCONSISTENT_DECLARATION, cur->children[1]->lineNo, cur->children[1]->val);
                sym->symType = FUNC;
                Symbol *funcSym = genFuncSym(retType, cur->children[1], FUNC);
                deleteST(sym->func.paras, sym->func.parac);
                funcSym->next = sym->next;
                memcpy(sym, funcSym, sizeof(Symbol));
            }
            else
                sym->symType = FUNC;
        }
        else
            printf("Error type %d at Line %d: redefination of func \"%s\".\n",
                   FUNC_REDEFINATION, cur->children[1]->lineNo, cur->children[1]->val);
    }
    else
        addFunc(retType, cur->children[1], FUNC);
}

/*
check initialization of variable, if legal initialization then return 1, else return 0
*/
int initCheck(Node *assignNode, Type varType)
{
    Type expType;
    checkExp(assignNode->children[1], &expType);

    // printf("107: varType:type:%d, name:%s, structInfo:%x\n", varType.type,varType.symAddr->name, varType.symAddr->var.structInfo);
    // printf("108:expType:type:%d, name:%s, structInfo:%x\n", expType.type, expType.symAddr->name, expType.symAddr->var.structInfo);
    if (expType.type != ERROR && !equal(varType, expType))
    {
        errorOccurred = 1;
        printf("Error type %d at Line %d: operands type mismatch.\n",
               EXP_TYPE_MISMATCH, assignNode->lineNo);
        return 0;
    }
    return 1;
}

/*
check all variables to be defined, if legal then add the variable to st.
para: varDef-> GLOBAL_VAR_DEF or LOCAL_DEF node, st->symbol table,  tSize->size of st
        type(type, symAddr) ->  type.type: data type of variable(INT, FLOAT, STRUCT), 
                                type.symAddr->only useful for struct type variable
*/
void handleVarList(SymbolTable st, int tSize, Node *varDef, Type type)
{
    for (int i = 1; i < varDef->num; ++i)
    {
        Node *varNode;
        Symbol *sym;

        if (varDef->children[i]->type == ASSIGN_OP)
        {
            varNode = varDef->children[i]->children[0];
        }
        else
            varNode = varDef->children[i];

        // printf("136:name:%s\n", varNode->val);
        sym = lookupST(st, tSize, varNode->val);
        if (sym)
        {
            printf("Error type %d at Line %d: symbol \"%s\" was declared before.\n",
                   VAR_REDEFINATION, varNode->lineNo, varNode->val);
        }
        else
        {
            Type varType;
            Symbol* sym = addVar(st, tSize, varNode, type);
            varType.symAddr = sym;
            varType.type = type.type;
            if (varDef->children[i]->type == ASSIGN_OP)
            {
                initCheck(varDef->children[i], varType);
                if(!errorOccurred)
                {
                    Operand* result = genVarOp(sym);
                    Operand* op1 = translateExp(varDef->children[i]->children[1], &meaningless);
                    if(op1->kind == ADDRESS)
                        op1 = genOp(GET_VALUE, &(op1->no));
                    else if(op1->kind == ADDRESS_V)
                        op1 = genOp(GET_VALUE_V, &(op1->no));
                    genCode(ASSIGN, 2, op1, result);
                }
            }                

            // generate 3 address code: DEC x [size]
            if(type.type == STRUCT || sym->var.dimension > 0)
            {
                int size = calSize(sym);
                genDEC(sym, size);    
            }
        }
    }
}

void defVar(SymbolTable st, int tSize, Node *varDef)
{
    int index = 0;
    Node *typeNode = varDef->children[0];
    if (typeNode->type == BASE_TYPE)
    {
        Type type;
        getType(typeNode, &type);
        handleVarList(st, tSize, varDef, type);
    }
    else // struct type
    {

        Node *structNode = varDef->children[0];
        Type type;

        // use a defined struct type to define variables
        if (structNode->num == 1 && varDef->num > 1)
        {
            getType(structNode, &type);
            if (type.symAddr == NULL)
            {
                printf("Error type %d at Line %d: struct \"%s\" hasn't been defined yet.\n",
                       UNDEFINED_STRUCT, structNode->children[0]->lineNo, structNode->children[0]->val);
                // return;
            }
            // printf("181:num:%d, type:%d, addr:%x\n", varDef->num, type.type, type.symAddr);
            handleVarList(st, tSize, varDef, type);
        }
        // define a new struct and use the new struct to define variables or just define a new struct
        else
        {
            Symbol *sym = lookupST(globalST, G_SIZE, structNode->children[0]->val);
            // printf("298:name:%s\n", structNode->children[0]->val);
            if (sym)
            {
                printf("Error type %d at Line %d: symbol \"%s\" was declared before.\n",
                       STRUCT_NAME_REPEATED, structNode->children[0]->lineNo, structNode->children[0]->val);
                type.type = ERROR;
                type.symAddr = NULL;
                // return;
            }
            // add the new defined struct to symbol table st
            sym = addStruct(globalST, G_SIZE, structNode);
            if (sym)
            {
                type.type = STRUCT;
                type.symAddr = sym;
            }
            handleVarList(st, tSize, varDef, type);
        }
    }
}

// compSt has at most two children: LOCAL_DEF_LIST or STMT_LIST.
void checkCompSt(Node *pNode, Type retType)
{
    if (pNode == NULL)
        return;

    int size = 0;
    Node *compSt = pNode;
    SymbolTable lst = NULL;

    // put paras of function into lst
    if (pNode->type == FUNC_DEF)
    {
        /* 
        generate 3 address code:
        FUNCTION f
        */
       if(errorOccurred)
            goto L;
        Operand* op1 = genOp(NAME, pNode->children[1]->val);
        genCode(FUNCTION, 1, op1);

        L:
        size = pNode->children[1]->num;
        Symbol *funcSym = findSymbol(pNode->children[1]->val);
        compSt = pNode->children[2];
        if (compSt->num > 0 && compSt->children[0]->type == LOCAL_DEF_LIST)
            size += compSt->children[0]->num * 5;
        if (size > 0)
        {
            int paraNum = pNode->children[1]->num;
            lst = (Symbol **)malloc(sizeof(Symbol *) * size);
            memset(lst, 0, sizeof(Symbol *) * size);
            push(lst, size);
            for (int i = 0; i < paraNum; ++i)
            {
                Type type;
                Node *typeNode = pNode->children[1]->children[i];
                Node *idNode = typeNode->children[0];
                Symbol *sym;
                getType(typeNode, &type);
                if (type.type == STRUCT)
                    idNode = typeNode->children[typeNode->num - 1];
                sym = addVar(lst, size, idNode, type);

                if(!errorOccurred)
                {
                    // generate code: PARAM x
                    Operand* op1;
                    if(sym->var.type == STRUCT || sym->var.dimension > 0)
                        op1 = genOp(ADDRESS_V, &varNO);
                    else
                        op1 = genOp(VARIABLE, &varNO);
                    ++varNO;
                    genCode(PARAM, 1, op1);
                    sym->var.op = op1;
                }
                
            }
        }
    }

    for (int i = 0; i < compSt->num; ++i)
    {
        if (compSt->children[i]->type == LOCAL_DEF_LIST)
        {
            Node *defList = compSt->children[i];
            if (lst == NULL)
            {
                size = defList->num * 5;
                lst = createST(size);
                push(lst, size);
            }
            for (int i = 0; i < defList->num; ++i)
            {
                Node *def = defList->children[i];
                defVar(lst, size, def);
            }
        }

        else if (compSt->children[i]->type == STMT_LIST)
        {
            Node *stmtList = compSt->children[i];
            for (int j = 0; j < stmtList->num; ++j)
                checkStmt(stmtList->children[j], retType);
            // check whether last statement of function is a RETURN statement(not necessary)
        }
    }

    // delete local symbol table in this block scope when exiting this block
    if (lst)
        pop();
}

int isStructEqual(Symbol *s1, Symbol *s2)
{
    if (s1 == s2)
        return 1;
    if (s1->structure.memsNum != s2->structure.memsNum)
        return 0;
    for (int i = 0; i < s1->structure.memsNum; ++i)
    {
        Symbol *mem1 = s1->structure.members[i];
        Symbol *mem2 = s2->structure.members[i];
        if (mem1->var.type == mem2->var.type)
        {
            if (isArray(mem1))
            {
                if (mem1->var.dimension != mem2->var.dimension)
                    return 0;
                for (int j = 0; j < mem1->var.dimension; ++j)
                    if (mem1->var.eachDimSize[j] != mem2->var.eachDimSize[j])
                        return 0;
                if (mem1->var.type == STRUCT_ARRAY)
                    if (!isStructEqual(mem1->var.structInfo, mem2->var.structInfo))
                        return 0;
            }
            else if (mem1->var.type == STRUCT)
            {
                if (!isStructEqual(mem1->var.structInfo, mem2->var.structInfo))
                    return 0;
            }
        }
        else
            return 0;
    }
    return 1;
}

void genTmpVar(Type *type)
{
    Symbol *tmp = (Symbol *)malloc(sizeof(Symbol));
    tmp->symType = VAR;
    tmp->var.type = type->type;
    tmp->var.structInfo = type->symAddr;
    tmp->var.dimension = type->dimension;

    type->symAddr = tmp;
}

/* check whether variable(this is important, for struct and func, this function will occurr error) are equivalent.
if equals return 1, else return 0
*/
int equal(Type type1, Type type2)
{
    // printf("438:symType:%d, type:%d, dimensin:%d, name:%s\n", type1.symAddr->symType, type1.type, type1.dimension, type1.symAddr->name);
    // printf("439:symType:%d, type:%d, dimensin:%d, name:%s\n", type2.symAddr->symType, type2.type, type2.dimension, type2.symAddr->name);

    int tmp1 = 0, tmp2 = 0;
    int ret;
    if (type1.symAddr && type1.symAddr->symType != VAR)
    {
        tmp1 = 1;
        genTmpVar(&type1);
    }
    if (type2.symAddr && type2.symAddr->symType != VAR)
    {
        tmp2 = 1;
        genTmpVar(&type2);
    }

    // printf("451:type1 = %d, type2 = %d, d1;%d, d2:%d\n", type1.type, type2.type, type1.dimension, type2.dimension);
    if (type1.type == STRUCT_ARRAY && type2.type == STRUCT_ARRAY)
    {
        if (type1.dimension == type2.dimension && isStructEqual(type1.symAddr->var.structInfo, type2.symAddr->var.structInfo))
            ret = 1;
        else
            ret = 0;
    }
    else if ((type1.type == INT_ARRAY && type2.type == INT_ARRAY) || (type1.type == FLOAT_ARRAY && type2.type == FLOAT_ARRAY))
    {
        if (type1.dimension == type2.dimension)
            ret = 1;
        else
            ret = 0;
    }
    else if ((isInt(type1)) && (isInt(type2)))
    {
        ret = 1;
    }
    else if ((isFLoat(type1)) && (isFLoat(type2)))
    {
        ret = 1;
    }
    else if ((isStructType(type1)) && (isStructType(type2)) && isStructEqual(type1.symAddr->var.structInfo, type2.symAddr->var.structInfo))
    {
        ret = 1;
    }
    else
        ret = 0;

    if (tmp1)
        free(type1.symAddr);
    if (tmp2)
        free(type2.symAddr);
    return ret;
}

/*
if legal reutrn 1 else return 0
notes:args of function are children of pNode, symInfo is a pointer to the funciton symbol in symbol table
*/
int checkArgs(Symbol *symInfo, Node *pNode)
{
    Type expType, type;
    if (symInfo->func.parac != pNode->num)
        return 0;
    for (int i = 0; i < pNode->num; ++i)
    {
        checkExp(pNode->children[i], &expType);
        // printf("501: symType:%dl type:%d, dimension:%d, name:%s\n",expType.symAddr->symType,expType.type, expType.dimension, expType.symAddr->name);
        // printf("502:symType:%d, type:%d, dimension:%d, name:%s\n", symInfo->func.paras[i]->symType, symInfo->func.paras[i]->var.type, symInfo->func.paras[i]->var.dimension, symInfo->func.paras[i]->name);
        type.type = symInfo->func.paras[i]->var.type;
        type.dimension = symInfo->func.paras[i]->var.dimension;
        type.symAddr = symInfo->func.paras[i];
        if (!equal(expType, type))
        {
            return 0;
        }
    }
    return 1;
}


void checkStmt(Node *stmt, Type retType)
{
    switch ((stmt->type))
    {
    // WHILE_LOOP has two children: the first is condition(EXP), second is a stmt
    case WHILE_LOOP:
    {
        Type expType;

        Operand* label1 = genLABEL();     

        checkExp(stmt->children[0], &expType);
        if(errorOccurred)
            goto L;

        // no error occurred then generate 3 address code
        CodeList trueList = NULL;
        CodeList falseList = NULL;
        translateBoolExp(stmt->children[0], &trueList, &falseList);
        Operand* label2 = genLABEL();
        backpatch(trueList, label2);
        free(trueList);

        L:
        checkStmt(stmt->children[1], retType);
        if(errorOccurred)
            goto L1;

        genCode(JMP, 1, label1);
        Operand* label3 = genLABEL();
        backpatch(falseList, label3);
        free(falseList);
        
        L1:
        break;
    }
    // BRANCH has two or three children: the first is conditon(EXP), other are stmt
    case BRANCH:
    {
        Type expType;
        checkExp(stmt->children[0], &expType);
        if(errorOccurred)
            goto L2;

        CodeList trueList = NULL;
        CodeList falseList = NULL;
        translateBoolExp(stmt->children[0], &trueList, &falseList);

        // IF EXP S1
        Operand* label1 = genLABEL();
        backpatch(trueList, label1);
        free(trueList);
        
        L2:
        checkStmt(stmt->children[1], retType);
        // ELSE S2
        if(stmt->num == 3)
        {
            // has an ELSE statment, so add a GOTO statement after S1, but the label is unknowed now.
            genCode(JMP, 1, NULL);
            int index = codeNum - 1;

            Operand* label2 = genLABEL();
            backpatch(falseList, label2);
            free(falseList);

            checkStmt(stmt->children[2], retType);
            if(errorOccurred)
                goto L4;
            
            // set label of GOTO statement generated before
            Operand* label3 = genLABEL();
            codes[index]->_3ops.result = label3;
        }  
        else
        {
            Operand* label2 = genLABEL();
            backpatch(falseList, label2);
            free(falseList);
        }          
        L4:
        break;
    }
    // RETURN EXP: EXP is children of RETURN node
    case CALL_RETURN:
    {
        Type expType;
        Symbol *tmpSym;
        if (stmt->num == 0)
        {
            printf("Error type %d at Line %d: return value mismatch.\n",
                   RETURN_VALUE_MISMATCH, stmt->lineNo);
            return;
        }
        checkExp(stmt->children[0], &expType);

        // these steps are for making symAddr points to a variable symbol, so then can use function equal
        genTmpVar(&retType);

        if (expType.type != ERROR && !equal(expType, retType))
            printf("Error type %d at Line %d: return value mismatch.\n",
                   RETURN_VALUE_MISMATCH, stmt->lineNo);
        else    // no error then generate 3 address code
        {          
            Operand* op1 = translateExp(stmt->children[0], &meaningless);
            
            // !!!!!!!!!! if return type is not only int, then this processing logic has problem
            if(op1->kind == ADDRESS)
                op1 = genOp(GET_VALUE, &(op1->no));
            else if(op1->kind == ADDRESS_V)
                op1 = genOp(GET_VALUE_V, &(op1->no));

            genCode(RETURN , 1, op1);
        }
        free(retType.symAddr);
        break;
    }
    case COMP_ST:
    {
        checkCompSt(stmt, retType);
        break;
    }
    //case ARITHMATIC_OP,RELOP_OP, LOGICAL_OP, ASSIGN_OP, FUNC_CALL, ARRAY_REFERENCE, MEMBER_ACCESS_OP:
    default:
    {
        Type type;
        checkExp(stmt, &type);
        if(type.type == ERROR)
            errorOccurred = 1;
        
        // no error occurred then generate code
        if(!errorOccurred)   
            translateExp(stmt, &meaningless);
                    
        break;
    }
    }
}

/* if not struct type return 1, else return 0
 */
int notStructType(Type type)
{
    if (type.type == STRUCT)
        return 0;
    else if (type.type == STRUCT_ARRAY)
    {
        if (0 == type.dimension)
            return 0;
    }
    return 1;
}

int notInt(Type type)
{
    if (type.type == INT || type.type == INT_CONSTANT)
        return 0;
    if (type.type == INT_ARRAY && 0 == type.dimension)
        return 0;
    return 1;
}

int notFloat(Type type)
{
    if (type.type == FLOAT || type.type == FLOAT_CONSTANT)
        return 0;
    if (type.type == FLOAT_ARRAY && 0 == type.dimension)
        return 0;
    return 1;
}

// check exp and put return value into expType
void checkExp(Node *exp, Type *expRet)
{
    // base cases
    // integer constant
    if (exp->type == INTEGER)
    {
        assignType(expRet, INT_CONSTANT, NULL);
        return;
    }
    // float point constant
    if (exp->type == FLOAT_POINT)
    {
        assignType(expRet, FLOAT_CONSTANT, NULL);
        return;
    }
    // identifier
    if (exp->type == IDENTIFIER)
    {
        Symbol *sym = findSymbol(exp->val);
        if (sym == NULL || sym->symType == STRUCTURE)
        {
            printf("Error type %d at Line %d: undefined variable \"%s\".\n",
                   VAR_UNDEFINED, exp->lineNo, exp->val);
            assignType(expRet, ERROR, NULL);
            return;
        }
        if (sym->symType == VAR)
        {
            expRet->type = sym->var.type;
            expRet->dimension = sym->var.dimension;
            expRet->symAddr = sym;
        }

        // type is useless for struct and function
        else
        {
            expRet->type = 0;
            expRet->symAddr = sym;
        }
        return;
    }

    switch (exp->type)
    {
    case ARITHMATIC_OP:
    case RELOP_OP:
    {
        Type exp1Type, exp2Type;
        if (exp->num == 1)
        {
            checkExp(exp->children[0], expRet);
            if (expRet->symAddr && expRet->symAddr->symType != VAR)
                printf("Error type %d at Line %d: operand and operator mismatch.\n",
                       OPERAND_TYPE_MISMATCH, exp->lineNo);
            return;
        }
        checkExp(exp->children[0], &exp1Type);
        checkExp(exp->children[1], &exp2Type);

        ERROR_OCCURRED_THEN_EXIT

        if ((notInt(exp1Type) && notFloat(exp1Type)) || (notInt(exp2Type) && notFloat(exp2Type)))
        {
            printf("Error type %d at Line %d: operand and operator mismatch.\n",
                   OPERAND_TYPE_MISMATCH, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }
        if (!equal(exp1Type, exp2Type))
        {
            printf("Error type %d at Line %d: operands type mismatch.\n",
                   OPERAND_TYPE_MISMATCH, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }
        if (exp->type == RELOP_OP)
        {
            assignType(expRet, INT_CONSTANT, NULL);
        }
        else
        {
            expRet->type = notInt(exp1Type) ? FLOAT_CONSTANT : INT_CONSTANT;
            expRet->symAddr = NULL;
        }
        return;
    }

    case LOGICAL_OP:
    {
        Type exp1Type, exp2Type;
        exp2Type.type = 0;
        checkExp(exp->children[0], &exp1Type);
        if (exp->num > 1)
            checkExp(exp->children[1], &exp2Type);

        ERROR_OCCURRED_THEN_EXIT

        // Consider that there is only one operand for taking a non-operation
        if (notInt(exp1Type) || (notInt(exp2Type) && exp2Type.type != 0))
        {
            printf("Error type %d at Line %d: operands type mismatch.\n",
                   OPERAND_TYPE_MISMATCH, exp->lineNo);
            assignType(expRet, ERROR, NULL);
        }
        assignType(expRet, INT_CONSTANT, NULL);
        return;
    }

    case ASSIGN_OP:
    {
        Type exp1Type, exp2Type;
        checkExp(exp->children[0], &exp1Type);
        checkExp(exp->children[1], &exp2Type);

        ERROR_OCCURRED_THEN_EXIT

        // only variables can be assigned
        if (!(exp1Type.symAddr && exp1Type.symAddr->symType == VAR))
        {
            printf("Error type %d at Line %d: rValue can't be assigned.\n",
                   RVALUE_ASSIGNED, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }

        if (notInt(exp1Type) && notFloat(exp1Type) && notStructType(exp1Type) || notInt(exp2Type) && notFloat(exp2Type) && notStructType(exp2Type))
        {
            printf("Error type %d at Line %d: operand and operator mismatch.\n",
                   OPERAND_TYPE_MISMATCH, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }

        // left operand is lValue, need check whether oeprands match
        if (!equal(exp1Type, exp2Type))
        {
            printf("Error type %d at Line %d: expression type mismatch.\n",
                   EXP_TYPE_MISMATCH, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }
        *expRet = exp2Type;
        return;
    }

    case MEMBER_ACCESS_OP:
    {
        // has two children
        Type exp1Type;
        checkExp(exp->children[0], &exp1Type);
        // checkExp(exp->children[1], &exp2Type);

        if (exp1Type.type == ERROR)
        {
            *expRet = exp1Type;
            return;
        }
        if (notStructType(exp1Type))
        {
            printf("Error type %d at Line %d: non-struct type has no operator '.'.\n",
                   ACCESS_MEMBER_OF_NONSTRUCT, exp->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }
        // check exp at the right of '.'
        /*  ????? this if statement is useless, right of '.' is always an IDENTIFIER */
        if (exp->children[1]->type == MEMBER_ACCESS_OP)
        {
            checkExp(exp->children[1], expRet);
            if (expRet->type == ERROR)
                return;
            *expRet = findStructMem(exp1Type.symAddr->var.structInfo, exp->children[1]->children[0]->val);
            if (expRet->type == ERROR)
            {
                printf("Error type %d at Line %d: struct has no member \"%s\".\n",
                       VISIT_UNDEFINED_STRUCT_MEMBER, exp->lineNo, exp->children[1]->val);
            }
            return;
        }

        else if (exp->children[1]->type == IDENTIFIER)
        {
            (*expRet) = findStructMem(exp1Type.symAddr->var.structInfo, exp->children[1]->val);
            // printf("664:type:%d, name:%s, dimension:%d\n", expRet->type, expRet->symAddr->name, expRet->symAddr->var.dimension);
            if (expRet->type == ERROR)
            {
                printf("Error type %d at Line %d: struct has no member \"%s\".\n",
                       VISIT_UNDEFINED_STRUCT_MEMBER, exp->lineNo, exp->children[1]->val);
            }
            return;
        }
        else
        {
            printf("Error type %d at Line %d: struct has no member \"%s\".\n",
                   VISIT_UNDEFINED_STRUCT_MEMBER, exp->lineNo, exp->children[1]->val);
            assignType(expRet, ERROR, NULL);
            return;
        }
        // printf("addr:%x, name:%s\n", exp1Type.symAddr->var.structInfo, exp->children[1]->val);
    }

    case FUNC_CALL:
    {
        // printf("nums:%d, name:%s\n", exp->children[0]->num, exp->children[0]->val);
        //  exp->children[0]->val is name of function
        Symbol *symInfo = findSymbol(exp->children[0]->val);
        // name of symbol didn't found
        if (symInfo == NULL)
        {
            printf("Error type %d at Line %d: undefined function \"%s\".\n",
                   FUNC_UNDEFINED, exp->children[0]->lineNo, exp->children[0]->val);
            assignType(expRet, ERROR, NULL);
            return;
        }
        // find the name in symbol table
        // check symbol type whether is FUNC
        if (symInfo->symType != FUNC)
        {
            if (symInfo->symType == FUNC_DEC_)
            {
                printf("Error type %d at Line %d: undefined function \"%s\".\n",
                       FUNC_DECLARED_BUT_UNDEFINED, symInfo->lineno, symInfo->name);
                assignType(expRet, ERROR, NULL);
                return;
            }
            printf("Error type %d at Line %d: non-function has no function call operator.\n",
                   CALL_OP_MISUSED, exp->children[0]->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }
        // check paras and args are matched(args are children of exp->children[0])
        if (!checkArgs(symInfo, exp->children[0]))
        {
            printf("Error type %d at Line %d: paras and args of function are mismatched.\n",
                   PAR_ARG_MISMATCH, exp->children[0]->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }

        (*expRet) = symInfo->func.retType;
        return;
    }

    case ARRAY_REFERENCE:
    {
        Type expType;
        checkExp(exp->children[0], &expType);

        if (expType.type == ERROR)
            return;
        // if variable is not an array
        if (expType.type != INT_ARRAY && expType.type != FLOAT_ARRAY && expType.type != STRUCT_ARRAY)
        {
            printf("Error type %d at Line %d: non-array type has no operator '[]'.\n",
                   ARRAY_OP_MISUSED, exp->children[0]->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }

        // if var is array type, check its dimension and check whether all indices are int(int type variable or integer literal)
        for (int i = 1; i < exp->num; ++i)
        {
            Type expType;
            checkExp(exp->children[i], &expType);
            if (notInt(expType))
            {
                printf("Error type %d at Line %d: index of arary isn't integer.\n",
                       NONINTEGER_IN_ARRAY_OP, exp->children[0]->lineNo);
                assignType(expRet, ERROR, NULL);
                return;
            }
        }
        // printf("num:%d, dimension:%d\n", exp->num-1, symInfo->var.dimension);
        if (exp->num - 1 > expType.symAddr->var.dimension)
        {
            printf("Error type %d at Line %d: non-array type has no operator '[]'.\n",
                   ARRAY_OP_MISUSED, exp->children[0]->lineNo);
            assignType(expRet, ERROR, NULL);
            return;
        }

        expRet->type = expType.type;
        expRet->symAddr = expType.symAddr;
        // e.g:int a[10][10][10] then dimension of a[10] is 2
        expRet->dimension = expType.symAddr->var.dimension - exp->num + 1;
        return;
    }
    }
}

// parse 'typeNode' and put the parsing result in 'type', mainly used to get return value function
void getRetType(Node *typeNode, Type *type)
{

    if (typeNode->type == BASE_TYPE) // ret type is base type
    {
        if (strcmp(typeNode->val, "int") == 0)
            type->type = INT_CONSTANT;
        else
            type->type = FLOAT_CONSTANT;
        type->symAddr = NULL;
    }
    else // return type is struct type
    {
        Symbol *ret = findSymbol(typeNode->children[0]->val);
        if (ret == NULL)
        {
            // there is no need to print error message here, because it is already printed when function is typeNodeined
            type->type = ERROR;
        }
        else
            type->type = STRUCT;
        type->symAddr = ret;
    }
}
