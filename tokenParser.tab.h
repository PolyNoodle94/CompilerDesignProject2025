/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_TOKENPARSER_TAB_H_INCLUDED
# define YY_YY_TOKENPARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LP = 258,
    RP = 259,
    ASGN = 260,
    SC = 261,
    OP2 = 262,
    OP3 = 263,
    OP4 = 264,
    IF = 265,
    THEN = 266,
    ELSE = 267,
    BEGINKEY = 268,
    END = 269,
    WHILE = 270,
    DO = 271,
    PROGRAM = 272,
    VAR = 273,
    AS = 274,
    INT = 275,
    BOOL = 276,
    WRITEINT = 277,
    READINT = 278,
    IDENTIFIER = 279,
    NUM = 280,
    BOOLLIT = 281
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 15 "tokenParser.y" /* yacc.c:1909  */

    char* sval;
    int iValue;
    bool bValue;
    int op2val;
    int op3val; 
    int op4val;

    PRGRM* programPtr;
    DECLS* declarationsPtr;
    STMTSEQ* statementSeqPtr;
    STMT* statementPtr;
    ASGNSTRUCT* asgnStructPtr;
    IFSTMT* ifStatementPtr;
    WHILESTMT*  whileStatementPtr;
    EXPR* expressionPtr;
    SIMPEXPR* simpleExprPtr;
    TERM* termPtr;
    FACTOR* factorPtr;

#line 102 "tokenParser.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOKENPARSER_TAB_H_INCLUDED  */
