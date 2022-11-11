#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "errorType.h"
#include "symbol.h"
#include "../ast/ast.h"

/* bugs: can't correctly handle define parameters when defining a struct, but I think no body would do like that!*/

/*
using imperative style to implement symbol table to support multiple scopes;
using hash table and close addressing to handle conflicts;
using linked list to connect variables in the same level scope and
implementing a stack to store current level scope and making its top points to the head of the list;

same level of scope means in the same block, for eaxmple:{int a, b;{int c, d}},
a, b are in the same level of scope and c, d are in the same level of scope
*/

Stack scope;
extern Symbol *globalST[];

void freeSymbol(Symbol *sym);
void deleteST(SymbolTable st, int tSize);
Symbol* addVar(SymbolTable st, int tSize, Node* idNode, Type type);
int nameExisted(Symbol** syms, char* name);
Symbol* addStruct(SymbolTable st, int tSize, Node* structNode);

// hash function, size determine the size of the symbol table, this size can be changed according to actual situation
unsigned int hash_pjw(char *name, int size)
{
    --size;// return value is index of array() index < size)
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~size)
            val = (val ^ (i >> 12)) & size;
    }
    return val;
}

SymbolTable createST(int size)
{
    SymbolTable st = (Symbol **)malloc(sizeof(Symbol *) * size);
    memset(st, 0, sizeof(Symbol *) * size);
}

// look up symbol table according to symbol name, return the address of the symbol. If looking up failed,return NULL.
Symbol *lookupST(SymbolTable st, int tableSize,char *name)
{
    int index = hash_pjw(name, tableSize);
    Symbol *cur = st[index];
    while (cur)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
            
        cur = cur->next;
    }
    return NULL;
}

/* look up in current scope first, if not found then look up in the outer scope, and so on.
until a look up is made in global scope, if not found in global scope then return NULL.
*/
Symbol *findSymbol(char *name)
{

    Symbol *ret;
    for (int i = scope.top; i >= 0; i = i - 2)
    {
        ret = lookupST((SymbolTable)scope.arr[i], (int)scope.arr[i - 1], name);
        if (ret)
            return ret;
    }
    return NULL;
}



void getType(Node* typeNode, Type* type)
{
    if(typeNode->type == BASE_TYPE)  
    {
        if(strcmp(typeNode->val, "int") == 0)
            type->type = INT;
        else
            type->type = FLOAT;
        type->symAddr = NULL;
    }
    else                
    {
        Symbol* ret = lookupST(globalST, G_SIZE, typeNode->children[0]->val);                
        if(ret == NULL)
        {
            type->type = ERROR;                 
        }                    
        else
            type->type = STRUCT;
        type->symAddr = ret;
    }
}


/* dType has two  possible value: INT, FLOAT
 */
Symbol *genVarSym(Node *idNode, Type type)
{
    DataType dType = type.type;
    Symbol* structSym = type.symAddr;
    Symbol *varSym = (Symbol *)malloc(sizeof(Symbol));
    memset(varSym, 0, sizeof(Symbol));
    varSym->symType = VAR;
    varSym->lineno = idNode->lineNo;
    strncpy(varSym->name, idNode->val, strlen(idNode->val));

    varSym->var.dimension = idNode->num;
    // arrary type
    if (varSym->var.dimension > 0)
    {
        if (dType == INT)
            varSym->var.type = INT_ARRAY;
        else if (dType == FLOAT)
            varSym->var.type = FLOAT_ARRAY;
        else
            varSym->var.type = STRUCT_ARRAY;

        varSym->var.eachDimSize = (int *)malloc(sizeof(int) * varSym->var.dimension);
        memset(varSym->var.eachDimSize, 0, sizeof(int) * varSym->var.dimension);
        for (int i = 0; i < varSym->var.dimension; ++i)
            varSym->var.eachDimSize[i] = atoi(idNode->children[i]->val);
    }
    else
        varSym->var.type = dType;

    varSym->var.structInfo = structSym;
    return varSym;
}

/*
int calStructSize(Symbol* sym)
{
    int size = 0;
    Symbol** members = sym->structure.members;
    for(int i = 0; i < sym->structure.memsNum; ++i)
    {
        Type type;
        type.type = members[i]->var.type;
        type.symAddr = members[i]->var.structInfo;
        size += calSize(members[i]);
    }
    return size;

}
*/

int calSize(Symbol* varSym)
{
    int size = 0, unit, num = 1;
    int type = varSym->var.type;
    if(type == STRUCT_ARRAY || type == STRUCT)
    {
        unit = varSym->var.structInfo->structure.size;
        if(type == STRUCT)
            goto L;
    }
    else
        unit = 4;
    

    for(int i = 0; i < varSym->var.dimension; ++i)
    {
        num *= varSym->var.eachDimSize[i];
    }
    L:
    size = unit * num;
    return size;
}

/*
fill in domains(memsNum and members) of the struct symbol 'structSym'
para: structNode-> STRUCT_TYPE node, structSym: points to a symbol to be filled in
*/

void fillInStructSymbol(Node *structNode, Symbol* structSym)
{    
    //int errorOccurred = 0;
    int count = 0;
    Node* defList;
    int memsNum = 0, index = 0;            

    if(structNode->num > 1)
        defList = structNode->children[1];
    else
        return;

    // apply for space for members
    for(int i = 0; i < defList->num; ++i)
        memsNum += defList->children[i]->num-1;
    structSym->structure.memsNum = memsNum;

    structSym->structure.members = (Symbol **)malloc(sizeof(Symbol *) * memsNum);
    memset(structSym->structure.members, 0, sizeof(Symbol*)*memsNum);

    // create a symbol(struct Symbol) for each member defination and get addresses of all member symbols then fill in member table
    for (int i = 0; i < defList->num; ++i)
    {
        
        Node *def = defList->children[i];
        Type type;
        getType(def->children[0], &type);
        
        // additional treatment for STRUCT_TYPE member
        if(def->children[0]->type == STRUCT_TYPE)
        {
            // use struct name to define member variable 
            if(def->children[0]->num == 1)
            {
                //the struct hasn't been defined
                // or use the struct name to define variables inside the struct
                if(type.type == ERROR || type.symAddr->symType == UNFINISHED_STRUCTURE)
                    printf("Error type %d at Line %d: struct \"%s\" hasn't been defined yet.\n",
                        UNDEFINED_STRUCT, def->children[0]->children[0]->lineNo, def->children[0]->children[0]->val);
                else
                    goto L;
            }
                
            // define a new struct and use it to define struct member
            else
            {
                Symbol* sym;
                Node* idNode = def->children[0]->children[0];

                //printf("193:name:%s\n", idNode->val);
                sym = lookupST(globalST, G_SIZE, idNode->val);
                // Nested defines structs with the same name
                if(sym)
                    printf("Error type %d at Line %d: symbol \"%s\" was declared before.\n",
                        STRUCT_NAME_REPEATED, idNode->lineNo, idNode->val);
                else
                {
                    sym = addStruct(globalST, G_SIZE, def->children[0]);
                    type.type = STRUCT;
                    type.symAddr = sym;
                    goto L;
                }                    
            }
        }

        else
        {            
            // handle all member vairables
            L:
            for(int j = 1; j < def->num; ++j)
            {
                if(def->children[j]->type == ASSIGN_OP)
                    printf("Error type %d at Line %d: initialize struct member \"%s\".\n",
                            STRUCT_MEMBER_DEFINED_INVALID, def->children[j]->children[0]->lineNo, def->children[j]->children[0]->val);
                else
                {
                    
                    Node* idNode = def->children[j];
                    if(nameExisted(structSym->structure.members, idNode->val))
                        printf("Error type %d at Line %d: struct domain name duplicated.\n",
                            STRUCT_MEMBER_DEFINED_INVALID, idNode->lineNo);
                    
                    // generate struct member and calculate size of struct
                    else
                    {
                        Symbol* sym = genVarSym(idNode,type);                          
                        structSym->structure.members[count++] = sym;
                        structSym->structure.size += calSize(sym);  

                    }                                 
                }
            }
        }
    }
}



/* add variable into symbol table st
paras: st->symbol table, tSize->size of st, idNode->IDENTIFIER node
        dType->type of variable,  structSym->pointer to struct symbol(only useful for struct type variable)
*/
Symbol* addVar(SymbolTable st, int tSize, Node* idNode, Type type)
{    
    int index;
    Symbol *varSym = genVarSym(idNode, type);
    index = hash_pjw(idNode->val, tSize);
    varSym->next = st[index];
    st[index] = varSym;
    return varSym;
    //printf("size:%d, index: %d, name:%s\n", tSize, index, varSym->name);

}


/* add struct into symbol table st
st->symbol table, tSize->size of st, structNode->STRUCT_TYPE node
*/
Symbol* addStruct(SymbolTable st, int tSize, Node* structNode)
{    
    // generate a new symbol and initial part of its domain
    Symbol* structSym ;
    int index;
    char* structName = structNode->children[0]->val;
    structSym = (Symbol*) malloc(sizeof(Symbol));
    memset(structSym, 0, sizeof(Symbol));
    strncpy(structSym->name, structName,  NAME_SIZE);
    structSym->lineno = structNode->children[0]->lineNo;
    structSym->symType = UNFINISHED_STRUCTURE;
    
    /* First add the structure to the symbol table and then assign values to each field of the structure symbol, 
    so that effectively handling the nested definition of structs with the same name*/
    index = hash_pjw(structSym->name, tSize);
    structSym->next = st[index];
    st[index] = structSym;
    fillInStructSymbol(structNode, structSym);
    structSym->symType = STRUCTURE;
    return structSym;
}


int nameExisted(Symbol** syms, char* name)
{
    int i = 0;
    Symbol* cur = syms[i];
    while(cur)
    {
        if(strcmp(cur->name, name) == 0)
            return 1;
        cur = syms[++i];
    }
    return 0;
}

Symbol* genFuncSym(Type retType, Node* funcHead, SymbolType sType)
{
    Symbol *funcSym = (Symbol *)malloc(sizeof(Symbol));
    memset(funcSym, 0, sizeof(Symbol));
    strncpy(funcSym->name, funcHead->val, strlen(funcHead->val));
    funcSym->symType = sType;
    funcSym->lineno = funcHead->lineNo;

    funcSym->func.retType = retType;
    funcSym->func.parac = funcHead->num;
    if (funcHead->num == 0)
        return funcSym;
        
    // get type of parameters(if there is any)
    funcSym->func.paras = (SymbolTable)malloc(sizeof(Symbol*) * funcSym->func.parac);
    memset(funcSym->func.paras, 0, sizeof(Symbol*) * funcSym->func.parac);
    for (int i = 0; i < funcHead->num; ++i)
    {
        Node* typeNode = funcHead->children[i];
        Type type;
        getType(typeNode, &type);
        if(type.type == ERROR)
        {
            printf("Error type %d at Line %d: struct \"%s\" hasn't been defined yet.\n",
                    UNDEFINED_STRUCT, typeNode->lineNo, typeNode->children[0]->val);
        }
        Node* idNode;
        Symbol* sym;
        if(type.type == STRUCT || type.type == ERROR)
            idNode = typeNode->children[typeNode->num-1];
        else
            idNode = typeNode->children[0];
        
        if(nameExisted(funcSym->func.paras, idNode->val))
            printf("Error type %d at Line %d: redefination of parameter \"%s\".\n",
                VAR_REDEFINATION, typeNode->lineNo, idNode->val);
        funcSym->func.paras[i] = genVarSym(idNode,type);
    }
    return funcSym;
}

void addFunc(Type retType, Node* funcHead, SymbolType sType)
{
    Symbol* funcSym = genFuncSym(retType, funcHead, sType);
    // add function symbol into symboal table
    int index = hash_pjw(funcHead->val, G_SIZE);
    funcSym->next = globalST[index];
    globalST[index] = funcSym;

}     




void freeSymbol(Symbol *sym)
{
    if (sym)
    {
        if (sym->symType == VAR)
        {
            if (sym->var.eachDimSize)
                free(sym->var.eachDimSize);
        }
        else if (sym->symType == STRUCTURE)
        {
            if (sym->structure.members)
            {
                for(int i = 0; i < sym->structure.memsNum; ++i)
                    freeSymbol(sym->structure.members[i]);
                free(sym->structure.members);
            }
        }
        else
        {
            if (sym->func.paras)
                deleteST(sym->func.paras, sym->func.parac);
        }
        free(sym);
    }
}

// delete all symbols in st
void deleteST(SymbolTable st, int tSize)
{
    Symbol *cur, *pre;
    for (int i = 0; i < tSize; ++i)
    {
        cur = st[i];
        while (cur)
        {
            pre = cur;
            cur = cur->next;
            freeSymbol(pre);
        }
    }
    if (st)
        free(st);
}

Type findStructMem(Symbol *structSym, char* name)
{
    //printf("389: name:%s, num:%d, memberName:%s\n", structSym->name, structSym->structure.memsNum, name);
    Type type;
    for (int i = 0; i < structSym->structure.memsNum; ++i)
    {
        Symbol* member = structSym->structure.members[i];
        if (member && strcmp(member->name, name) == 0)
        {
            //printf("name:%s, type;%d, structInfo:%x\n", member->name, member->var.type, member->var.structInfo);
            type.type = member->var.type;
            type.dimension = member->var.dimension;
            type.symAddr = member;
            return type;
        }
    }    
    type.type = ERROR;
    type.symAddr = NULL;
    return type;
}

void push(SymbolTable st, int stSize)
{
    scope.arr[++scope.top] = (void *)stSize;
    scope.arr[++scope.top] = (void *)st;
}

void pop()
{
    // only pop out lst
    if (scope.top > 2)
    {
        deleteST((SymbolTable)scope.arr[scope.top], (int)scope.arr[scope.top - 1]);
        scope.top -= 2;
    }
}
