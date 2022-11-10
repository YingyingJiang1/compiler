
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include"ast/ast.h"

extern void yyrestart(FILE*);
extern int yyparse();
extern FILE* yyin;


int isLexError = 0;
int isSynError = 0;
Node* root = NULL;

//void printAST(Node* t);
void deleteTree(Node* t);
NodeType checkProgram(Node* ast);
void outputCode(char* outfile);

int main(int argc, char** argv)
{
    char outfile[32];
    FILE* f;
    if(argc == 3)
    {
        if(!(f = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }  
        strncpy(outfile, argv[2], strlen(argv[2]));         
    }
    yyrestart(f);
    /*return 1 if input is incorrect and error recovery is impossible
    return 0 if input is valid*/
    int ret = yyparse();
    if(!isSynError && !isLexError)
    {
        //printAST(root);
        checkProgram(root);
        outputCode(outfile);
    }    
    deleteTree(root);
    return 0;
}






