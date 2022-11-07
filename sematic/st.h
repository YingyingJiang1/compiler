#ifndef ST_H
#define ST_H
#include "symbol.h"

extern Symbol *globalST[];


SymbolTable createST(int size);
Symbol* findSymbol(char* name);
Symbol* addVar(SymbolTable st, int tSize, Node* idNode, Type type);
void addFunc(Type retType, Node* funcHead, SymbolType sType);
Symbol* addStruct(SymbolTable st, int tSize, Node* structNode);
void copyST(SymbolTable dest, int dSize, SymbolTable src, int sSize);
void deleteST(SymbolTable st, int tSize);
Type findStructMem(Symbol* structSym, char* name);
Symbol* genFuncSym(Type retType, Node* funcHead, SymbolType sType);
Symbol *genVarSym(Node *idNode, Type type);
void getType(Node* typeNode, Type* type);
Symbol *lookupST(SymbolTable st, int tableSize, char *name);
void push(SymbolTable st, int stSize);
void pop();
#endif