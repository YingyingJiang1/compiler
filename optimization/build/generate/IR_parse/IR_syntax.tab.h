/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_IR_YY_BUILD_GENERATE_IR_PARSE_IR_SYNTAX_TAB_H_INCLUDED
# define YY_IR_YY_BUILD_GENERATE_IR_PARSE_IR_SYNTAX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef IR_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define IR_YYDEBUG 1
#  else
#   define IR_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define IR_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined IR_YYDEBUG */
#if IR_YYDEBUG
extern int IR_yydebug;
#endif

/* Token type.  */
#ifndef IR_YYTOKENTYPE
# define IR_YYTOKENTYPE
  enum IR_yytokentype
  {
    IR_TOKEN_EOL = 258,
    IR_TOKEN_COLON = 259,
    IR_TOKEN_FUNCTION = 260,
    IR_TOKEN_LABEL = 261,
    IR_TOKEN_SHARP = 262,
    IR_TOKEN_INT = 263,
    IR_TOKEN_ASSIGN = 264,
    IR_TOKEN_STAR = 265,
    IR_TOKEN_OP = 266,
    IR_TOKEN_IF = 267,
    IR_TOKEN_RELOP = 268,
    IR_TOKEN_ADDR_OF = 269,
    IR_TOKEN_GOTO = 270,
    IR_TOKEN_RETURN = 271,
    IR_TOKEN_DEC = 272,
    IR_TOKEN_ARG = 273,
    IR_TOKEN_CALL = 274,
    IR_TOKEN_PARAM = 275,
    IR_TOKEN_READ = 276,
    IR_TOKEN_WRITE = 277,
    IR_TOKEN_ID = 278
  };
#endif

/* Value type.  */
#if ! defined IR_YYSTYPE && ! defined IR_YYSTYPE_IS_DECLARED
union IR_YYSTYPE
{
#line 18 "src/IR_parse/IR_syntax.y"

    int INT;
    char *id;
    IR_OP_TYPE IR_op_type;
    IR_RELOP_TYPE IR_relop_type;
    IR_program *IR_program_ptr_node;
    IR_function *IR_function_ptr_node;
    IR_stmt *IR_stmt_ptr_node;
    IR_val IR_val_node;
    IR_label IR_label_node;
    IR_var IR_var_node;

#line 102 "build/generate/IR_parse/IR_syntax.tab.h"

};
typedef union IR_YYSTYPE IR_YYSTYPE;
# define IR_YYSTYPE_IS_TRIVIAL 1
# define IR_YYSTYPE_IS_DECLARED 1
#endif


extern IR_YYSTYPE IR_yylval;

int IR_yyparse (void);

#endif /* !YY_IR_YY_BUILD_GENERATE_IR_PARSE_IR_SYNTAX_TAB_H_INCLUDED  */
