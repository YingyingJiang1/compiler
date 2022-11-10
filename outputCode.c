#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "innerCode/innerCode.h"

#define BUF_SIZE 0xffff


extern InnerCode* codes[];
extern int codeNum;

char buffer[BUF_SIZE];
int length = 0;

/*
#define printRead(op)   printf("READ %c%d\n", getPrefix(op), op->no)
#define printWrite(op)  printf("WRITE %c%d\n", getPrefix(op), op->no)
#define printPARAM(op)  printf("PARAM %c%d\n", getPrefix(op), op->no)
#define printARG(OP)    printf("ARG %c%d\n", getPrefix(op), op->no)
#define printRETURN(op) printf("RETURN %c%d\n", getPrefix(op), op->no)
#define printDEC(op1, op2)    printf("DEC %c%d %d\n", getPrefix(op1), op1->no, op2->no)
*/

#define outFUNCTION(code) length = length + sprintf(buffer+length, "FUNCTION %s :\n", code->_3ops.op1->name)
#define outJMP(code) length = length + sprintf(buffer+length, "GOTO %c%d\n", getPrefix(code->_3ops.result), code->_3ops.result->no)
#define outLABEL(code) length = length + sprintf(buffer+length, "LABEL %c%d :\n", getPrefix(code->_3ops.op1), code->_3ops.op1->no)
#define outASSIGN(code) Operand* op1 = code->_3ops.op1, *result = code->_3ops.result; \
length = length + sprintf(buffer+length, "%c%d := %c%d\n", getPrefix(result), result->no, getPrefix(op1), op1->no)
#define outDEC(code) Operand* op1 = code->_3ops.op1, *op2 = code->_3ops.op2; \
length = length + sprintf(buffer+length, "DEC %c%d %d\n", getPrefix(op1), op1->no, op2->constInt);



char getPrefix(Operand* op)
{
    if(op->kind == CONST_INT)
        return '#';
    else if(op->kind == VARIABLE)
        return 'v';
    else if(op->kind == TMP_VARIABLE)
        return 't';
    else if(op->kind == LABEL_NO)
        return 'L';
    else if(op->kind == GET_ADDRESS)
        return '&';
    else if(op->kind == ADDRESS)
        return '*';
}

// didn't consider float type
// + = * /, type of operands are VARIABLE or TMP_VARIABLE or CONST_INT
void out3opsCode(char op, InnerCode* code)
{
    Operand* op1, *op2, *op3;
    char ch1, ch2, ch3;
    op1 = code->_3ops.op1;
    op2 = code->_3ops.op2;
    op3 = code->_3ops.result;
    ch1 = getPrefix(op1);
    ch2 = getPrefix(op2);
    ch3 = getPrefix(op3);    
    length = length + sprintf(buffer+length, "%c%d := %c%d %c %c%d\n", ch3, op3->no, ch1, op1->no, op, ch2, op2->no);
}

// PARAM, ARG, RETURN, READ, WRITE, 
void out1opCode(char* ctype, InnerCode* code)
{
    Operand* op1 = code->_3ops.op1;
    char ch = getPrefix(op1);
    length = length + sprintf(buffer+length, "%s %c%d\n", ctype, ch, op1->no);
}

void outCJMP(InnerCode* code)
{
    char relop[3];
    if(code->kind == JG)
        strcpy(relop, ">");
    else if(code->kind == JGE)
        strcpy(relop, ">=");
    else if(code->kind == JL)
        strcpy(relop, "<");
    else if(code->kind == JLE)
        strcpy(relop, "<=");
    else if(code->kind == JE)
        strcpy(relop, "==");
    else 
        strcpy(relop, "!=");

    Operand* op1 = code->_3ops.op1;
    Operand* op2 = code->_3ops.op2;
    Operand* result = code->_3ops.result;
    char ch1, ch2,ch3;
    ch1 = getPrefix(op1);
    ch2 = getPrefix(op2);
    ch3 = getPrefix(result);    

    length = length + sprintf(buffer+length, "IF %c%d %s %c%d GOTO %c%d\n", ch1, op1->no, relop, ch2, op2->no, ch3, result->no);
}






void outputCode(char* outfile)
{
    memset(buffer, 0, BUF_SIZE);
    FILE* out = fopen(outfile, "w");
    for(int i = 0; i < codeNum; ++i)
    {
        switch (codes[i]->kind)
        {
        case PLUS:
            out3opsCode('+', codes[i]);
            break;
        case MINUS:
            out3opsCode('-', codes[i]);
            break;
        case MUL:
            out3opsCode('*', codes[i]);
            break;
        case DIV:
            out3opsCode('/', codes[i]);
            break;
        case PARAM:
            out1opCode("PARAM", codes[i]);
            break;
        case ARG:
            out1opCode("ARG", codes[i]);
            break;
        case RETURN:
            out1opCode("RETURN", codes[i]);
            break;
        case READ:
            out1opCode("READ", codes[i]);
            break;
        case WRITE:
            out1opCode("WRITE", codes[i]);
            break;
        case DEC:
        {
            outDEC(codes[i]);
            break;
        }            
        case LABEL:
        {
            outLABEL(codes[i]);
            break;
        }            
        case FUNCTION:
        {
            outFUNCTION(codes[i]);
            break; 
        }
        case JMP:
        {
            outJMP(codes[i]);
            break;
        }
        case ASSIGN:
        {
            outASSIGN(codes[i]);
            break;
        }
        default:
            outCJMP(codes[i]);
            break;
        }
        if(length > BUF_SIZE - 64)
        {
            buffer[length++] = 0;
            fputs(buffer, out);
            memset(buffer, 0, length);
            length = 0;
        }
    }
    printf("%s", buffer);
    buffer[length++] = 0;
    fputs(buffer, out);
    fclose(out);
}