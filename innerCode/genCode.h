#ifndef GEN_CODE_H
#define GEN_CODE_H
#include "../ast/ast.h"

extern int varNO;
extern int tmpNO;
extern int labelNO;

void genFuncHead(Node* def);
void genDEC(Type* type, Symbol* varSym);

#endif