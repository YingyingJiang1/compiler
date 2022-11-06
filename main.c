
#include<stdio.h>
#include<stdlib.h>
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

int main(int argc, char** argv)
{

    FILE* f;
    if(argc > 1)
    {
        if(!(f = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }           
    }
    yyrestart(f);
    /*return 1 if input is incorrect and error recovery is impossible
    return 0 if input is valid*/
    int ret = yyparse();
    if(!isSynError && !isLexError)
    {
        //printAST(root);
        checkProgram(root);
    }    
    deleteTree(root);
    return 0;
}


void deleteTree(Node* t)
{
    if(t == NULL)
        return;
    for(int i = 0; i < t->num; ++i)
        deleteTree(t->children[i]);        
    if(t->children)
        free(t->children);  
    if(t)
        free(t);
    
}
