%locations
%{
#include <stdio.h>
#include "lex.yy.c"
#include "../ast/ast.h"
extern int isSynError;
extern int isLexError;
extern Node* root;
void yyerror(char* msg);
Node* createAST(Node* root, int argc,...);

int structNo = 1;
%}
/* declared types*/
%union
{
    struct Node* pNode;
}
/* tokens */
%token <pNode> INT
%token <pNode> FLOAT
%token <pNode> SEMI COMMA DOT ASSIGNOP PLUS MINUS STAR DIV NOT LP RP LB RB LC RC
%token <pNode> TYPE STRUCT
%token <pNode> ID
%token <pNode> RETURN IF ELSE WHILE
%token <pNode> RELOP  AND OR

/* precedence and associativity */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* non-terminals*/
%type <pNode> Program ExtDefList ExtDef ExtDecList
%type <pNode> Specifier StructSpecifier OptTag Tag
%type <pNode> VarDec FunDec VarList ParamDec
%type <pNode> CompSt StmtList Stmt
%type <pNode> DefList Def DecList Dec 
%type <pNode> Exp Args



%%
/* For the purpose of the subsequent semantic analysis, many high-level nodes are preserved
when generating abstract syntax tree, such as,  "ExtDefList", "funcDef"...*/

/* High-level definitions */
Program: ExtDefList                             {$$ = $1;root = $$;mergeNodes($1, EXT_DEF_LIST);}
    ;
ExtDefList: ExtDef ExtDefList                   {$$ = genNode(1, EXT_DEF_LIST, "ExtDefList");createAST($$, 2, $1, $2);}
    | /* empty */                               {$$ = NULL;}
    ;
ExtDef: Specifier ExtDecList SEMI               {$$ = genNode(1, GLOBAL_VAR_DEF, "gVarDef"); createAST($$, 2, $1, $2); mergeNodes($$, TOKEN_OTHER);}
    | Specifier SEMI                            {$$ = genNode(1, GLOBAL_VAR_DEF, "gVarDef"); createAST($$, 1, $1);}
    | Specifier FunDec CompSt                   {$$ = genNode(1, FUNC_DEF, "funcDef"); createAST($$, 3, $1, $2, $3);}
    | Specifier FunDec SEMI                      {$$ = genNode(1, FUNC_DEC, "funcDec"); createAST($$, 2, $1, $2);}
    | error SEMI                                {}
    | Specifier error                           //{yyerror("syntax error");printf("%d:specifier error\n", step++);}
    ;
ExtDecList: VarDec                              {$$ = $1;}
    | VarDec COMMA ExtDecList                   {$$ = createAST($2, 2, $1, $3);}
    ;       

/* Specifiers */
Specifier: TYPE                                 {$$ = $1;}
    | StructSpecifier                           {$$ = $1;}
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    {mergeNodes($4, LOCAL_DEF_LIST);$$ = createAST($1, 2, $2, $4);}
    | STRUCT Tag                                {$$ = createAST($1, 1, $2);}
    | STRUCT error  SEMI                            //{yyerror("syntax error");printf("%d:struct error semi\n", step++);}
    | STRUCT error RC
    ;
OptTag: ID                                      {$$ = $1;}
    | /* empty */                               {char str[30]; sprintf(str, "%d", structNo); ++structNo;$$ = genNode(1, IDENTIFIER, str);}
    ;
Tag: ID                                         {$$ = $1;}
    ;

/*declarators*/
VarDec: ID                                      {$$ = $1;}
    | VarDec LB INT RB                          {$$ = createAST($1,1, $3);}
    | LB error RB
    ;   
FunDec: ID LP VarList RP                        {createAST($1, 1, $3);mergeNodes($1, TOKEN_OTHER);$$ = $1;}
    | ID LP RP                                  {$$ = $1;}
    | error RP                                  {}
    ;
VarList: ParamDec COMMA VarList                 {$$ = createAST($2, 2, $1, $3);}
    | ParamDec                                  {$$ = $1;}
    ;
ParamDec: Specifier VarDec                      {$$ = createAST($1, 1, $2);}
    ;

/* Statements */
CompSt: LC DefList StmtList RC                  {$$ = genNode(1, COMP_ST, "CompSt");mergeNodes($2, LOCAL_DEF_LIST);mergeNodes($3, STMT_LIST);$$ = createAST($$, 2, $2, $3);}
    | error RC                                  {}
    ;
StmtList: Stmt StmtList                         {$$ = genNode(1, STMT_LIST, "stmtList");createAST($$, 2, $1, $2);}
    | /* empty */                               {$$ = NULL;}
    ;           
Stmt: Exp SEMI                                  {$$ = $1;}
    | CompSt                                    {$$ = $1;}
    | RETURN Exp SEMI                           {$$ = createAST($1, 1, $2);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = createAST($1, 2, $3, $5);}
    | IF LP Exp RP Stmt ELSE Stmt               {$$ = createAST($1, 3, $3, $5, $7);}
    | WHILE LP Exp RP Stmt                      {$$ = createAST($1, 2, $3, $5);}
    | error SEMI                                {}
    | error Stmt %prec LOWER_THAN_ELSE          {}
    | error Stmt ELSE Stmt                      {}
    ;

/* local definitions */
DefList: Def DefList                            {$$ = genNode(1, LOCAL_DEF_LIST, "lDefList");createAST($$, 2, $1, $2);}     
    | /* empty */                               {$$ = NULL;}
    ;
Def: Specifier DecList SEMI                     {$$ = genNode(1, LOCAL_DEF, "localDef");createAST($$, 2, $1, $2); mergeNodes($$, TOKEN_OTHER);}
    | error SEMI
    ;
DecList: Dec                                    {$$ = $1;}
    | Dec COMMA DecList                         {$$ = createAST($2, 2, $1, $3);}
    ;
Dec: VarDec                                     {$$ = $1;}
    | VarDec ASSIGNOP Exp                       {$$ = createAST($2, 2, $1, $3);}
    ;

/*expressions*/
Exp: Exp ASSIGNOP Exp                           {$$ = createAST($2, 2, $1, $3);}
    | Exp AND Exp                               {$$ = createAST($2, 2, $1, $3);}
    | Exp OR Exp                                {$$ = createAST($2, 2, $1, $3);}
    | Exp RELOP Exp                             {$$ = createAST($2, 2, $1, $3);}
    | Exp PLUS Exp                              {$$ = createAST($2, 2, $1, $3);}
    | Exp MINUS Exp                             {$$ = createAST($2, 2, $1, $3);}
    | Exp STAR Exp                              {$$ = createAST($2, 2, $1, $3);}
    | Exp DIV Exp                               {$$ = createAST($2, 2, $1, $3);}
    | LP Exp RP                                 {$$ = $2;}
    | MINUS Exp                                 {$$ = createAST($1, 1, $2);}    
    | NOT Exp                                   {$$ = createAST($1, 1, $2);}    
    | ID LP Args RP                             {$$ = genNode(1, FUNC_CALL, "funcCall"); createAST($1, 1, $3); mergeNodes($1, TOKEN_OTHER);createAST($$, 1, $1);}
    | ID LP RP                                  {$$ = genNode(1, FUNC_CALL, ""); createAST($$, 1, $1);}
    | Exp LB Exp RB                             {$$ = genNode(1, ARRAY_REFERENCE, "ArrRef");createAST($$, 2, $1, $3);mergeNodes($$, ARRAY_REFERENCE);}
    | Exp DOT ID                                {$$ = createAST($2, 2, $1, $3);}
    | ID                                        {$$ = $1;}
    | INT                                       {$$ = $1;}
    | FLOAT                                     {$$ = $1;}
    ;
Args: Exp COMMA Args                            {$$ = createAST($2, 2, $1, $3); } 
    | Exp                                       {$$ = $1;}
    | error COMMA Args                          {}
    ;


%%

void yyerror(char* msg)
{
    isSynError = 1;
    printf("Error type B at Line %d:syntax error\n", yylloc.first_line);

}
