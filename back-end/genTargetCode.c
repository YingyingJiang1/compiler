// Naive register allocation algor


#include "../front-end/ir/ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
                                                            
#define emit3opsCode(code) \
    codeOff += sprintf(targetCodes+codeOff, code, regr, reg1, reg2)      

#define emit1opCode(code) \
    codeOff += sprintf(targetCodes+codeOff, code, regr)

#define emitCjmpCode(code) \
    codeOff += sprintf(targetCodes+codeOff, code, reg1, reg2, result->no)      

#define push() \
    stack[++top] = (void*)offToFp 

#define pop() \
    offToFp = (int)stack[top--]                                            

extern IR* codes[];
extern int codeNum;
extern char* buffer;

struct Register
{
    Operand* curOp;    
    int off;    // offset relative to the current fp, useful when callee restoring registers.
    
}regs[32];// regsSaved are registers that callee is responsible for saving.
struct Register* regsSaved;

char* targetCodes;
static int codeOff = 0;
static int offToFp = 0;
static void* stack[100];
static int top = -1;
char curFunction[100];
static int saved = 0;
static int curCode = 0;
static int regr = -1, reg1 = -1, reg2 = -1;

void allocateReg(IR*);
void genTargetCode();
void calleeRestoreRegs();
void callerSaveRegs();
void emitAssignCode(int i);
void loadToReg(Operand* op, int regNo);
void outputCode();
void saveReg(int regNo);
Operand* helper(int kind1, int kind2,Operand* op);

void printCode()
{
    outputCode();
    //genTargetCode();
    //printf("%s", targetCodes);
}

void outTargetCode(char* outfile)
{
    //printCode();
    genTargetCode();
    FILE* out = fopen(outfile, "w");
    if(out)
    {
        fputs(targetCodes, out);
        fclose(out);
    }    
}

void emitRead()
{
    codeOff += sprintf(targetCodes+codeOff, "read:\n"
    "li $v0, 4\n"  
    "la $a0, _prompt\n"  
    "syscall\n"
    "li $v0, 5\n"
    "syscall\n"
    "jr $ra\n");  
    
}


void emitWrite()
{
    codeOff += sprintf(targetCodes+codeOff, "write:\n"
    "li $v0, 1\n"
    "syscall\n"
    "li $v0, 4\n"
    "la $a0, _ret\n"
    "syscall\n"
    "move $v0, $0\n"
    "jr $ra\n");      
}

void initRegs()
{
    for(int i = 0; i < 32; ++i)
    {
        regs[i].curOp = NULL;
        regs[i].off = 1;
    }
}

void genTargetCode()
{
    targetCodes = (char*)malloc(100000*sizeof(char));
    initRegs();
    codeOff += sprintf(targetCodes+codeOff, ".data\n"
    "_prompt: .asciiz \"Enter an integer:\"\n"
    "_ret: .asciiz \"\\n\"\n"
    ".globl main\n"
    ".text\n");    
    //push();
    emitRead();
    emitWrite();
    for(; curCode < codeNum; ++curCode)
    {
        IR* ir = codes[curCode];
        Operand* op1 = ir->_3ops.op1, *op2 = ir->_3ops.op2, *result = ir->_3ops.result;
        switch(ir->kind)
        {               
            case ASSIGN:
            {
                emitAssignCode(curCode);
                initRegs();
                continue;
            }     
            case PLUS:
            {                                          
                if(op2->kind == CONST_INT)
                {
                    ir->_3ops.op2 = NULL;
                    allocateReg(ir);                    
                    codeOff += sprintf(targetCodes+codeOff, "addi $%d, $%d, %d\n", regr, reg1, op2->constInt);
                }
                else
                {
                    allocateReg(ir);                   
                    emit3opsCode("add $%d, $%d, $%d\n");
                }
                break;
            }
            case MINUS:
            {                 
                if(op2->kind == CONST_INT)
                {
                    ir->_3ops.op2 = NULL;
                    allocateReg(ir);
                    codeOff += sprintf(targetCodes+codeOff, "addi $%d, $%d, -%d\n", regr, reg1, op2->constInt);
                }
                else
                {
                    allocateReg(ir);
                    emit3opsCode("sub $%d, $%d, $%d\n");
                }                    
                break;
            }
            case MUL:
            {
                allocateReg(ir);  
                emit3opsCode("mul $%d, $%d, $%d\n");
                break;
            }
            case DIV:
            {
                allocateReg(ir);  
                emit3opsCode("div $%d, $%d, $%d\n");
                emit1opCode("mflo $%d\n");
                break;
            }
            case LABEL:
            {                
                codeOff += sprintf(targetCodes+codeOff, "L%d:\n", op1->no);
                break;
            }
            case JMP:
            {
                codeOff += sprintf(targetCodes+codeOff, "j L%d\n", result->no);
                break;
            }
            case JG:
            {
                allocateReg(ir);  
                emitCjmpCode("bgt $%d, $%d, L%d\n");
                break;
            }
            case JGE:
            {
                allocateReg(ir);  
                emitCjmpCode("bge $%d, $%d, L%d\n");
                break;
            }
            case JL:
            {
                allocateReg(ir);  
                emitCjmpCode("blt $%d, $%d, L%d\n");
                break;
            }
            case JLE:
            {
                allocateReg(ir);  
                emitCjmpCode("ble $%d, $%d, L%d\n");
                break;
            }
            case JE:
            {
                allocateReg(ir);  
                emitCjmpCode("beq $%d, $%d, L%d\n");
                break;
            }
            case JNE:
            {
                allocateReg(ir);  
                emitCjmpCode("bne $%d, $%d, L%d\n");
                break;
            }
            case PARAM:
            {                 
                int off = 8, j = curCode;
                while(codes[j]->kind == PARAM)
                {
                    codes[j]->_3ops.op1->offset = off;                    
                    // allocateReg(ir); 
                    // codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", reg1, off);   
                    // saveReg(reg1); 
                    off += 4;     
                    ++j;  
                }   
                curCode = j-1;           
                break;
            }
            case ARG:
            {      
                saved = 1;                                                           
                int count = 0;
                int j = curCode;
                while(codes[j]->kind == ARG)
                {
                    IR* ir = codes[j];
                    Operand* op1 = ir->_3ops.op1;
                    allocateReg(ir); 
                    ++count;
                    codeOff += sprintf(targetCodes+codeOff, "sw $%d, -%d($sp)\n", reg1, 4*count); 
                    ++j;                    
                }
                offToFp -= 4 * count;
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, -%d\n", 4*count);                                                                              
                curCode = j-1;          
                break;
            }
            case CALL:   
            {                                                                                                               
                // save $ra
                offToFp -= 4;
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, -4\n");    
                codeOff += sprintf(targetCodes+codeOff, "sw $ra, 0($sp)\n"); 
                codeOff += sprintf(targetCodes+codeOff, "jal %s\n", op1->name); 
                // restore $ra
                codeOff += sprintf(targetCodes+codeOff, "lw $ra, 0($sp)\n");         
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, 4\n");   
                offToFp += 4; 
                //restoreAll();     
                allocateReg(ir);                   
                codeOff += sprintf(targetCodes+codeOff, "move $%d, $v0\n", regr);                   
                break;
            }      
            case READ:
            {                                    
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, -4\n");    
                codeOff += sprintf(targetCodes+codeOff, "sw $ra, 0($sp)\n"); 
                codeOff += sprintf(targetCodes+codeOff, "jal read\n");   
                codeOff += sprintf(targetCodes+codeOff, "lw $ra, 0($sp)\n");         
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, 4\n");   
                allocateReg(ir);   
                codeOff += sprintf(targetCodes+codeOff, "move $%d, $v0\n", reg1);  
                saveReg(reg1);                
                break;   
            }      
            case WRITE:
            {                
                allocateReg(ir);                    
                codeOff += sprintf(targetCodes+codeOff, "move $a0, $%d\n", reg1); 
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, -4\n");    
                codeOff += sprintf(targetCodes+codeOff, "sw $ra, 0($sp)\n"); 
                codeOff += sprintf(targetCodes+codeOff, "jal write\n");  
                codeOff += sprintf(targetCodes+codeOff, "lw $ra, 0($sp)\n");         
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, 4\n");   
                break; 
            }
            case RETURN:
            {                     
                allocateReg(ir);          
                codeOff += sprintf(targetCodes+codeOff, "move $v0, $%d\n", reg1);                   
                codeOff += sprintf(targetCodes+codeOff, "move $sp, $fp\n");  
                codeOff += sprintf(targetCodes+codeOff, "lw $fp, 0($sp)\n");  
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, 4\n");  
                codeOff += sprintf(targetCodes+codeOff, "jr $ra\n");                  
                break;
            }
            case DEC:
            {
                offToFp -= result->constInt;
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $fp, %d\n", offToFp); 
                op1->offset =  offToFp;
                break;
            }
            case FUNCTION:
            {
                offToFp = 0;
                saved = 0;
                if(top >= 0)
                    pop();
                push();                
                initRegs();
                codeOff += sprintf(targetCodes+codeOff, "%s:\n", op1->name);    
                codeOff += sprintf(targetCodes+codeOff, "addi $sp, $sp, -4\n");    
                codeOff += sprintf(targetCodes+codeOff, "sw $fp, 0($sp)\n");   
                codeOff += sprintf(targetCodes+codeOff, "move $fp, $sp\n");    
                break;
            }
        }
        if(regr > 0)
            saveReg(regr);
        initRegs();
        
        //printf("%s\n", targetCodes);
        // if(result && (result->kind == GET_VALUE || result->kind == GET_VALUE_V))
        // {
        //     Operand* ret = helper(GET_VALUE, ADDRESS, result);
        //     if(!ret)
        //         ret = helper(GET_VALUE_V, ADDRESS_V, result);
        //     assert(ret != NULL);
        //     if(ret->reg > 0)
        //         codeOff += sprintf(targetCodes+codeOff, "sw $%d, 0($%d)\n", regr, ret->reg);
        //     else
        //         codeOff += sprintf(targetCodes+codeOff, "sw $%d, %d($fp)\n", regr, ret->offset);
        // }
        // else if(result && result->reg > 0)
        //     saveReg(result->reg);
    }
}


void restoreAll()
{
    for(int i = 8; i <= 25; ++i)
    {
        if(regsSaved[i].curOp)
        {
            regs[i].curOp = regsSaved[i].curOp;
            regs[i].curOp->reg = i;
            if(regs[i].curOp->offset == 1)
                regs[i].curOp->offset = regsSaved[i].off;
            codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", i, regs[i].curOp->offset);
        }
    }
}


void saveReg(int regNo)
{
    if(regs[regNo].curOp == NULL)
        return;
    if(regs[regNo].curOp->offset == 1)
    {        
        offToFp -= 4;
        codeOff += sprintf(targetCodes+codeOff, "sw $%d, %d($fp)\n", regNo, offToFp);
        codeOff += sprintf(targetCodes+codeOff, "addi $sp, $fp, %d\n", offToFp);          
        regs[regNo].curOp->offset = offToFp;
    }
    else
        codeOff += sprintf(targetCodes+codeOff, "sw $%d, %d($fp)\n", regNo, regs[regNo].curOp->offset);
}


void calleeRestoreRegs()
{
    for(int i = 8; i <= 25; ++i)
    {
        if(regsSaved[i].off != 1 && regsSaved->off != 0 && regsSaved[i].curOp)
        {
            regs[i].curOp = regsSaved[i].curOp;
            regs[i].curOp->reg = i;
            codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", i, regsSaved[i].off);
            regsSaved[i].curOp = NULL;
        }
    }
}


int same(Operand* op1, Operand* op2)
{
    if(op1->no == op2->no)
    {
        // t1 and *t1
        if(op1->kind == ADDRESS || op1->kind == GET_VALUE || op1->kind == TMP_VARIABLE)
        {
            if(op2->kind == GET_VALUE || op2->kind == ADDRESS || op2->kind == TMP_VARIABLE)
                return 1;
            else
                return 0;
        }
        // v1 and &v1 and *v1
        if(op1->kind == ADDRESS_V || op1->kind == GET_VALUE_V || op1->kind == GET_ADDRESS || op1->kind == VARIABLE)
        {
            if(op2->kind == GET_VALUE_V || op2->kind == VARIABLE || op2->kind == ADDRESS_V || op2->kind == ADDRESS_V)
                return 1;
            else
                return 0;
        }
    }
    else
        return 0;
}

// the same Operand should be allocated to the same register all the time.
// int getReg(Operand* op, int no, int no1)
// {
//     if(op && op->reg > 0)
//         return op->reg;
//     for(int i = 8; i <= 25; ++i)
//     {
//         if(regs[i].curOp == NULL && i != no && i != no1)
//         {
//             if(op)
//                 op->reg = i;
//             regs[i].curOp = op;            
//             return i;
//         }
//     }    
//     int regNo = -1;
//     if(op && op->preReg > 0 && op->preReg != no && op->preReg != no1)
//         regNo = op->preReg;
//     else
//     {
//         regNo = rand() % 18 + 8;
//         while(regNo == no || regNo == no1)
//         {            
//             regNo = rand() % 18 + 8;
//         } 
//     }                     
//     saveReg(regNo);
//     regs[regNo].curOp->reg = -1;
//     if(op)
//         op->reg = regNo;    
//     regs[regNo].curOp = op;
//     return regNo;
// }


int getReg(Operand* op, int no1, int no2)
{
    while(1)
    {
        int i = (rand() % 17) + 8;
        if(regs[i].curOp == NULL)
        {
            regs[i].curOp = op;
            if(op->preReg == -1)
                op->preReg = i;
            op->reg = i;
            break;
        }
    }
    return op->reg;
}


Operand* helper(int kind1, int kind2,Operand* op)
{
    if(op->kind == kind1)
    {
        for(int j = curCode-1; j >= 0; --j)
        {
            IR* ir = codes[j];
            Operand* op1 = ir->_3ops.op1, *op2 = ir->_3ops.op2, *result = ir->_3ops.result;            
            if(op1 && op1->kind == kind2 && op1->no == op->no)
                return op1;
            else if(op2 && op2->kind == kind2 && op2->no == op->no)
                return op2;
            else if(result && result->kind == kind2 && result->no == op->no)
                return result;
        }
    }
    return NULL;
}


void emitAssignCode(int i)
{
    IR* ir = codes[i];
    Operand* op1 = ir->_3ops.op1, *result = ir->_3ops.result;
    Operand* ret = NULL;
    int regNo = getReg(result, -1, -1);
    if(result->kind == GET_VALUE || result->kind == GET_VALUE_V)
    {
        // t1 and *t1 are not the same Operand.
        ret = helper(GET_VALUE, ADDRESS, result);
        if(!ret)
            ret = helper(GET_VALUE_V, ADDRESS_V, result);
        assert(ret != NULL);        
    }   
    if(op1->kind == CONST_INT)
    {
        codeOff += sprintf(targetCodes+codeOff, "li $%d, %d\n", regNo, op1->constInt);  
    }
    else if(op1->kind == GET_ADDRESS)
    {
        Operand* ret1 = helper(GET_ADDRESS, VARIABLE, op1);
        codeOff += sprintf(targetCodes+codeOff, "addi $%d, $fp, %d\n", regNo, ret1->offset);              
    }
    else if(op1->kind == GET_VALUE || op1->kind == GET_VALUE_V)
    {
        Operand* ret1 = helper(GET_VALUE, ADDRESS, op1);
        if(!ret1)
            ret1 = helper(GET_VALUE_V, ADDRESS_V, op1);
        assert(ret1 != NULL); 
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", regNo, ret1->offset);     
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, 0($%d)\n", regNo, regNo);        
    }
    else
    {        
        getReg(op1, regNo, -1);
        loadToReg(op1, op1->reg);
        if(op1->offset == 1)
            saveReg(op1->reg);
        if(ret == NULL)
        {
            codeOff += sprintf(targetCodes+codeOff, "move $%d, $%d\n", regNo, op1->reg);  
        }
        else
            regNo = op1->reg;        
    }
    if(ret)
    {
        int regNo1 = 25;            
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", regNo1, ret->offset); 
        codeOff += sprintf(targetCodes+codeOff, "sw $%d, 0($%d)\n", regNo, regNo1);          
    }  
    else 
        saveReg(result->reg);  
}


void loadToReg(Operand* op, int regNo)
{
    // v1(address) and *v1; t1(address) and *t1
    if(op->kind == GET_VALUE || op->kind == GET_VALUE_V)
    {             
        Operand* ret = NULL;
        ret = helper(GET_VALUE, ADDRESS, op); 
        if(!ret)
            ret = helper(GET_VALUE_V, ADDRESS_V, op); 
        assert(ret != NULL);
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", regNo, ret->offset);              
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, 0($%d)\n", regNo, regNo);            
    }    
    else if(op->kind == GET_ADDRESS && op->offset != 1)
    {
        Operand* ret = helper(GET_ADDRESS, VARIABLE, op);
        assert(ret != NULL);        
        codeOff += sprintf(targetCodes+codeOff, "addi $%d, $fp, %d\n", regNo, ret->offset);              
    }
    else if(op->kind == CONST_INT)
        codeOff += sprintf(targetCodes+codeOff, "li $%d, %d\n", regNo, op->constInt);    
    else if(op->offset != 1)
        codeOff += sprintf(targetCodes+codeOff, "lw $%d, %d($fp)\n", regNo, op->offset);   
}


// void allocateReg(IR* code)
// {
//     Operand* op1 = code->_3ops.op1, *op2 = code->_3ops.op2, *result = code->_3ops.result;    
//     reg1 = reg2 = regr = -1;  
//     if(op1)
//     {
//         if(op1->reg < 0)
//         {
//             reg1 = getReg(op1, -1, -1);
//             loadToReg(op1, reg1);
//             if(op1->reg > 0 && op1->offset == 1)
//                 saveReg(reg1); 
//         }
//         else
//             reg1 = op1->reg;
//     }
//     if(op2)
//     {
//         if(op2->reg < 0)
//         {
//             reg2 = getReg(op2, reg1, -1);
//             loadToReg(op2, reg2);  
//             if(op2->reg > 0 && op2->offset == 1)
//                 saveReg(reg2); 
//         }
//         else
//             reg2 = op2->reg;
//     }
//     if(result && result->reg < 0 && result->kind != LABEL_NO)
//     {
//         int no = -1;
//         if(op2)
//             no = op2->reg;
//         regr = getReg(result, op1->reg, no);
        
//     } 
//     if(result && result->reg > 0)
//         regr = result->reg;         
// }


void allocateReg(IR* code)
{
    Operand* op1 = code->_3ops.op1, *op2 = code->_3ops.op2, *result = code->_3ops.result;
    reg1 = reg2 = regr = -1;
    if(result && result->kind == LABEL_NO)
    {
        if(op1->preReg == -1)
            reg1 = getReg(op1, -1, -1);
        else
        {
            reg1 = op1->preReg;
            regs[reg1].curOp = op1;            
        }    
        if(op2->preReg == -1 || op2->preReg == reg1)
            reg2 = getReg(op2, reg1, -1);
        else
        {
            reg2 = op2->preReg;
            regs[reg2].curOp = op2;
        }                 
        loadToReg(op1, reg1);
        loadToReg(op2, reg2);
        return;
    }
    if(op1 && code->kind != PARAM)
    {
        reg1 = getReg(op1, -1, -1);
        loadToReg(op1, reg1);
        if(op1->offset == 1 && code->kind != ARG
            && (op1->kind == VARIABLE || op1->kind == TMP_VARIABLE || op1->kind == ADDRESS || op1->kind == ADDRESS_V))
            saveReg(reg1);
        
    }
    if(op2)
    {
        reg2 = getReg(op2, reg1, -1);
        loadToReg(op2, reg2);
        if(op2->offset == 1 && code->kind != ARG 
            && (op2->kind == VARIABLE || op2->kind == TMP_VARIABLE || op2->kind == ADDRESS || op2->kind == ADDRESS_V))
            saveReg(reg2);
    }
    if(result && result->kind != LABEL_NO)
    {
        regr = getReg(result, reg1, reg2);
    }
}