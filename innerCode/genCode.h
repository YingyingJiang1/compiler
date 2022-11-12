#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "../ast/ast.h"
#include "../innerCode/innerCode.h"

extern int varNO;
extern int tmpNO;
extern int labelNO;
extern InnerCode* codes[];
extern int codeNum;

// generate code: DEX x [size](apply for space)
void genDEC(Symbol* sym, int size);

// generate code: LABEL label, return Operand 'label'
Operand* genLABEL();

// generate code: RETURN x
void genRETURN(Operand* op1);

// generate code
InnerCode* genCode(int kind, int argc, ...);

// generate operand
Operand* genOp(int kind, void* val);

// generate code for expressions
Operand* translateExp(Node* exp, Symbol** retSym);

// generate code for bool expressions, using backpatch techniques
void translateBoolExp(Node* exp, CodeList* tlistHead, CodeList* flistHead);

// set
void backpatch(CodeList list, Operand* label);


// set the label of all jump statements in the 'list' to 'label'
void genASSIGN(Operand* result, Operand* op1);


// generate a VARIABLE type operand for 3 address code
Operand* genVarOp(Symbol* sym);
#endif