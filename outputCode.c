#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "innerCode/innerCode.h"

#define BUF_SIZE 0xffff


extern InnerCode* codes[];
extern int codeNum;

char buffer[BUF_SIZE];
int length = 0;

#define outPARAM(code) length = length + sprintf(buffer+length, "PARAM v%d\n", code->_3ops.op1->no)

#define outFUNCTION(code) length = length + sprintf(buffer+length, "FUNCTION %s :\n", code->_3ops.op1->name)

#define outJMP(code) length = length + sprintf(buffer+length, "GOTO %s%d\n", getPrefix(code->_3ops.result), code->_3ops.result->no)

#define outLABEL(code) length = length + sprintf(buffer+length, "LABEL %s%d :\n", getPrefix(code->_3ops.op1), code->_3ops.op1->no)

#define outASSIGN(code) Operand* op1 = code->_3ops.op1, *result = code->_3ops.result; \
length = length + sprintf(buffer+length, "%s%d := %s%d\n", getPrefix(result), result->no, getPrefix(op1), op1->no)

#define outDEC(code) Operand* op1 = code->_3ops.op1, *result = code->_3ops.result; \
length = length + sprintf(buffer+length, "DEC %s%d %d\n", getPrefix(op1), op1->no, result->constInt)

#define outCALL(code) Operand* op1 = code->_3ops.op1, *result = code->_3ops.result; \
length = length + sprintf(buffer+length, "%s%d := CALL %s\n", getPrefix(result), result->no, op1->name)




char* getPrefix(Operand* op)
{
    if(op->kind == VARIABLE || op->kind == ADDRESS_V)
        return "v";
    if(op->kind == TMP_VARIABLE || op->kind == ADDRESS)
        return "t";
    if(op->kind == CONST_INT)
        return "#";
    if(op->kind == LABEL_NO)
        return "L";
    if(op->kind == GET_ADDRESS)
        return "&v";
    if(op->kind == GET_VALUE)
        return "*t";
}

// didn't consider float type
// + = * /, type of operands are VARIABLE or TMP_VARIABLE or CONST_INT
void out3opsCode(char op, InnerCode* code)
{
    Operand* op1, *op2, *op3;
    char* str1, *str2, *str3;
    op1 = code->_3ops.op1;
    op2 = code->_3ops.op2;
    op3 = code->_3ops.result;
    str1 = getPrefix(op1);
    str2 = getPrefix(op2);
    str3 = getPrefix(op3);  
    
    length = length + sprintf(buffer+length, "%s%d := %s%d %c %s%d\n", str3, op3->no, str1, op1->no, op, str2, op2->no);
}


// PARAM, ARG, RETURN, READ, WRITE, 
void out1opCode(char* ctype, InnerCode* code)
{
    Operand* op1 = code->_3ops.op1;
    char* str = getPrefix(op1);
    length = length + sprintf(buffer+length, "%s %s%d\n", ctype, str, op1->no);
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
    char* str1, *str2, *str3;
    str1 = getPrefix(op1);
    str2 = getPrefix(op2);
    str3 = getPrefix(result);    

    length = length + sprintf(buffer+length, "IF %s%d %s %s%d GOTO %s%d\n", str1, op1->no, relop, str2, op2->no, str3, result->no);
}






void outputCode(char* outfile)
{
    printf("106\n");
    memset(buffer, 0, BUF_SIZE);
    FILE* out = fopen(outfile, "w");
    for(int i = 0; i < codeNum; ++i)
    {
        switch (codes[i]->kind)
        {
        case ASSIGN:
        {
            outASSIGN(codes[i]);
            break;
        }
        case LABEL:
        {
            outLABEL(codes[i]);
            break;
        }  
        case JMP:
        {
            outJMP(codes[i]);
            break;
        }  
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
        case DEC:
        {
            outDEC(codes[i]);
            break;
        } 
        case PARAM:
            outPARAM(codes[i]);
            break;
        case ARG:
            out1opCode("ARG", codes[i]);
            break;
        case CALL:
        {
            outCALL(codes[i]);
            break;
        }
        case RETURN:
            out1opCode("RETURN", codes[i]);
            break;
        case READ:
            out1opCode("READ", codes[i]);
            break;
        case WRITE:
            out1opCode("WRITE", codes[i]);
            break;                                     
        case FUNCTION:
        {
            outFUNCTION(codes[i]);
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