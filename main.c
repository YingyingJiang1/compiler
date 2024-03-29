
#include<stdio.h>
#include<stdlib.h>
#include <string.h>


extern void yyrestart(FILE*);
extern int yyparse();
extern FILE* yyin;
typedef struct Node Node;
typedef enum NodeType NodeType;

int isLexError = 0;
int isSynError = 0;
Node* root = NULL;

void deleteTree(Node* t);
void semanticAnalysis(Node* ast);
void outputCode(char* outfile);
void outTargetCode(char* outfile);

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
        if(strlen(argv[2]) > 31)
        {
            printf("Filename of output file is too long.\n");
            return 0;
        }
        strcpy(outfile, argv[2]);   
    }
    else
    {
        printf("Arguments error: main needs two arguments.\n");
        return 0;
    }
    yyrestart(f);
    yyparse();
    if(!isSynError && !isLexError)
    {
        semanticAnalysis(root);
        outTargetCode(outfile);
    }    
    deleteTree(root);
    return 0;
}






