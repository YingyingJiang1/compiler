#ifndef ST_H
#define ST_H
#include "symbol.h"

extern Symbol *globalST[];

unsigned int hash_pjw(char *name, int size);

// create a symbol table with a size of 'size'
SymbolTable createST(int size);

// find symbol with a name of 'name' and return address of the symbol if found, return NULL if not found
Symbol* findSymbol(char* name);

// generate a new variable symbol and add it to symbol table 'st'
Symbol* addVar(SymbolTable st, int tSize, Node* idNode, Type type);

// generate a new function symbol and add it to symbol table 'globalST'
void addFunc(Type retType, Node* funcHead, SymbolType sType);

// generate a new struct symbol and add it to symbol table 'st'
Symbol* addStruct(SymbolTable st, int tSize, Node* structNode);

// calculate size of variable and reutrn its size(in bytes)
int calSize(Symbol* varSym);

// delete symbol table 'st'
void deleteST(SymbolTable st, int tSize);

// find whether struct symbol 'structSym' has a member named 'name', return a struct Type type.
Type findStructMem(Symbol* structSym, char* name);

// generate a function symbol and return address of this symbol
Symbol* genFuncSym(Type retType, Node* funcHead, SymbolType sType);

// generate a varible symbol and return address of this symbol
Symbol *genVarSym(Node *idNode, Type type);

// parse 'typeNode' and store result in a Type struct pointed by 'type'
void getType(Node* typeNode, Type* type);

// look for symbol named 'name' in 'st', if found return address of the symbol else return NULL
Symbol *lookupST(SymbolTable st, int tableSize, char *name);

// push 'st' into stack 'scope'
void push(SymbolTable st, int stSize);

// pop symbol table at the top of stack 'scope'
void pop();


#endif