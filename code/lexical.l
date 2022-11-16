%option yylineno

%{
#include <string.h>
#include "syntax.tab.h"
#include "ast/ast.h"
#define YY_USER_ACTION \
yylloc.first_line = yylloc.last_line = yylineno; \
yylloc.first_column = yycolumn; \
yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;

extern int step;
int yycolumn = 1;
extern int isLexError;
Node* genNode(int lineNo,  NodeType type, char* val);
%}

ID          [_a-zA-Z][_0-9a-zA-Z]*
INT         [0-9]|[1-9][0-9]+|0[0-7]+|0[xX][0-9a-fA-F]+
FLOAT       (("."[0-9]+)|([0-9]+"."[0-9]*))([eE][+-]?[0-9]+)?
SEMI        ;
COMMA       ,
ASSIGNOP    =
RELOP       >|<|>=|<=|==|!=
PLUS        "+"
MINUS       "-"
STAR        "*"
DIV         "/"
AND         "&&"
OR          "||"
DOT         "."
NOT         !
TYPE        int|float
LP          "("
RP          ")"
LB          "["
RB          "]"
LC          "{"
RC          "}"
STRUCT      struct
RETURN      return
IF          if
ELSE        else
WHILE       while
WHITE_SPACE " "
FLOAT_ERROR (("."[0-9]+)|([0-9]+"."[0-9]*))([eE][+-]?)
OCTAL_ERROR   0([0-7]*[89]*[0-7]*)+
HEX_ERROR   0[xX]([0-9a-fA-F]*[g-zG-Z]*[0-9a-fA-F]*)*
%%
    /*if no return in action then yylex() doesn't return but continues
    to look for further matches, so no reutrn in action may have impact on
    subsequent syntax parsing(I guess!)*/
    /* Node* genNode(int lineNo,  char* name, NodeType type, char* val) */
{INT}       {
    //printf("%d:INT\n", step++, step++);
    yylval.pNode = genNode(yylineno, INTEGER, yytext);
    return INT;
}
{FLOAT}     {
    //printf("%d:FLOAT\n", step++);
    yylval.pNode = genNode(yylineno, FLOAT_POINT, yytext);
    return FLOAT;
}
{TYPE}      {
    //printf("%d:TYPE: %s\n", step++, yytext);F
    yylval.pNode = genNode(yylineno, BASE_TYPE, yytext);
    return TYPE;
}
{STRUCT}    {
    //printf("%d:STRUCT\n", step++);
    yylval.pNode = genNode(yylineno, STRUCT_TYPE, yytext);
    return STRUCT;
}
{RETURN}    {
    //printf("%d:RETURN\n", step++);
    yylval.pNode = genNode(yylineno, CALL_RETURN, yytext);
    return RETURN;
}
{IF}        {
    //printf("%d:IF\n", step++);
    yylval.pNode = genNode(yylineno, BRANCH, yytext);
    return IF;
}
{ELSE}      {
    //printf("%d:ELSE\n", step++);
    yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return ELSE;
}
{WHILE}     {
    //printf("%d:WHILE\n", step++);
    yylval.pNode = genNode(yylineno, WHILE_LOOP, yytext);
    return WHILE;
}
{ID}        {
    //printf("%d:ID: %s\n", step++, yytext);
    yylval.pNode = genNode(yylineno, IDENTIFIER, yytext);
    return ID;
}
{SEMI}      {
    //printf("%d:SEMI\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return SEMI;
}
{COMMA}     {
    //printf("%d:COMMA\n", step++);
    yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return COMMA;
}
{ASSIGNOP}  {
    //printf("%d:ASSIGNOP\n", step++);
    yylval.pNode = genNode(yylineno, ASSIGN_OP, yytext);
    return ASSIGNOP;
}
{RELOP}     {
    //printf("%d:RELOP\n", step++);
    yylval.pNode = genNode(yylineno, RELOP_OP, yytext);
    return RELOP;
}
{PLUS}      {
    //printf("%d:PLUS\n", step++);
    yylval.pNode = genNode(yylineno, ARITHMATIC_OP, yytext);
    return PLUS;
}
{MINUS}     {
    //printf("%d:MINUS\n", step++);
    yylval.pNode = genNode(yylineno, ARITHMATIC_OP, yytext);
    return MINUS;
}
{STAR}      {
    //printf("%d:STAR\n", step++);
    yylval.pNode = genNode(yylineno, ARITHMATIC_OP, yytext);
    return STAR;
}
{DIV}       {
    //printf("%d:DIV\n", step++);
    yylval.pNode = genNode(yylineno, ARITHMATIC_OP, yytext);
    return DIV;
}
{AND}       {
    //printf("%d:AND\n", step++);
    yylval.pNode = genNode(yylineno, LOGICAL_OP, yytext);
    return AND;
}
{OR}        {
    //printf("%d:OR\n", step++);
    yylval.pNode = genNode(yylineno, LOGICAL_OP, yytext);
    return OR;
}
{DOT}       {
    //printf("%d:DOT\n", step++);
    yylval.pNode = genNode(yylineno, MEMBER_ACCESS_OP, yytext);
    return DOT;
}   
{NOT}       {
    //printf("%d:NOT\n", step++);
    yylval.pNode = genNode(yylineno, LOGICAL_OP, yytext);
    return NOT;
}
{LP}        {
    //printf("%d:LP\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return LP;
}
{RP}        {
    //printf("%d:RP\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return RP;
}
{LB}        {
    //printf("%d:LB\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return LB;
}   
{RB}        {
    //printf("%d:RB\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return RB;
}
{LC}        {
    //printf("%d:LC\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return LC;
}
{RC}        {
    //printf("%d:RC\n", step++);
    //yylval.pNode = genNode(yylineno, TOKEN_OTHER, yytext);
    return RC;
}
{FLOAT_ERROR}   {
    printf("Error type A at line %d: illegal float \"%s\".\n",  yylineno,yytext);
    //printf("%d:Error type A at line %d\n", step++, yylineno);
    isLexError = 1;
    yylval.pNode = NULL;
    return FLOAT;
}
{OCTAL_ERROR}   {
    printf("Error type A at line %d: illegal octal number \"%s\".\n",  yylineno, yytext);
    //printf("%d:Error type A at line %d\n", step++, yylineno);
    isLexError = 1;
    yylval.pNode = NULL;
    return INT;
}
{HEX_ERROR} {
    printf("Error type A at line %d: illegal hexadecimal number \"%s\".\n",  yylineno, yytext);
    //printf("%d:Error type A at line %d\n", step++, yylineno);
    isLexError = 1;
    yylval.pNode = NULL;
    return INT;
}
\t                      {/*ignore*/}
{WHITE_SPACE}+         {/* ignore */}

"\n"        {yycolumn=1;}
"//"        {
    char c = input();
    while(c != '\n')
        c = input();
}
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]     {/* igonre multiple comments*/}
 /*
"/*"      {
    char c;
    while((c = input())!= EOF);
    //printf("%d:Unterminated comment at line %d.\n", step++, yylineno);
}
*/
.           {
    isLexError = 1;
    printf("Error type A at Line %d: mysterious characters \"%s\".\n", yylineno, yytext);
    //printf("%d:Error type A at line %d\n", step++, yylineno);
}
%%


