#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "../ast/ast.h"
#include "../innerCode/innerCode.h"

extern int varNO;
extern int tmpNO;
extern int labelNO;
extern InnerCode* codes[];
extern int codeNum;

// Generates code: DEX x [size](apply for space).
void genDEC(Symbol* sym, int size);

// Generates code: LABEL label.
Operand* genLABEL();

// Generates code: RETURN x.
void genRETURN(Operand* op1);


InnerCode* genCode(int kind, int argc, ...);

// Generates operand.
Operand* genOp(int kind, void* val);

// Generates code for expressions.
Operand* translateExp(Node* exp, Symbol** retSym);

// Generates code for bool expressions, using backpatch techniques.
void translateBoolExp(Node* exp, CodeList* tlistHead, CodeList* flistHead);

// set lable of jmp or cjmp code in 'list' to 'label'.
void backpatch(CodeList list, Operand* label);


// Generates code: x := y.
void genASSIGN(Operand* result, Operand* op1);


// Generates a operand for variables occurred in src code.
Operand* genVarOp(Symbol* sym);
#endif