%{
    #include <stdio.h>
    #include "tokenParser.h"

    // root -> used to store pointer to root of CFG
    PRGRM* root;

    int yylex(void);
    int yyerror(char *);
%}

%debug

// Symbols
%union {
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
};

%token LP   
%token RP
%token ASGN
%token SC

%token <op2val> OP2
%token <op3val> OP3
%token <op4val> OP4

%token IF
%token THEN
%token ELSE
%token BEGINKEY
%token END
%token WHILE
%token DO
%token PROGRAM
%token VAR
%token AS
%token INT
%token BOOL

%token WRITEINT
%token READINT

%token <sval>   IDENTIFIER
%token <iValue> NUM
%token <iValue> BOOLLIT

%type <programPtr>      Program
%type <declarationsPtr> Declarations
%type <bValue>          Type
%type <statementSeqPtr> StatementSequence ElseClause
%type <statementPtr>    Statement
%type <asgnStructPtr>   Assignment
%type <ifStatementPtr>  IfStatement
%type <whileStatementPtr> WhileStatement
%type <expressionPtr>   Expression WriteInt
%type <simpleExprPtr>   SimpleExpression
%type <termPtr>         Term
%type <factorPtr>       Factor


%start Program

%%
Program:
    PROGRAM Declarations BEGINKEY StatementSequence END
    {
        $$ = malloc(sizeof(PRGRM));
        $$->decls = $2;
        $$->stmtSeq = $4;
        root = $$;
    }
    ;

Declarations:
    {
        $$ = NULL;
    }
    | VAR IDENTIFIER AS Type SC Declarations
    {
        $$ = malloc(sizeof(DECLS));
        $$->identifier = $2;
        $$->varType = $4;
        $$->decls = $6;
        //printf("VAR %s AS %s ;\n", $$->identifier, $$->varType ? "BOOL" : "INT");
    }
    ;

Type:
    INT     { $$ = 0; /*printf("Type: %d\n", $$);*/ }
    | BOOL  { $$ = 1; /*printf("Type: %d\n", $$);*/ }
    ;

StatementSequence:
    {
        $$ = NULL;
    }
    | Statement SC StatementSequence
    {
        $$ = malloc(sizeof(STMTSEQ));
        $$->stmt = $1;
        $$->stmtSeq = $3;
    }
    ;

Statement:
    Assignment
    {
        $$ = malloc(sizeof(STMT));
        $$->asgnStruct = $1;
        $$->ifStmt = NULL;
        $$->whileStmt = NULL;
        $$->writeIntStruct = NULL;
    }       
    | IfStatement
    {
        $$ = malloc(sizeof(STMT));
        $$->asgnStruct = NULL;
        $$->ifStmt = $1;
        $$->whileStmt = NULL;
        $$->writeIntStruct = NULL;
    }
    | WhileStatement
    {
        $$ = malloc(sizeof(STMT));
        $$->asgnStruct = NULL;
        $$->ifStmt = NULL;
        $$->whileStmt = $1;
        $$->writeIntStruct = NULL;
    }
    | WriteInt
    {
        $$ = malloc(sizeof(STMT));
        $$->asgnStruct = NULL;
        $$->ifStmt = NULL;
        $$->whileStmt = NULL;
        $$->writeIntStruct = $1;
    }
    ;

Assignment:
    IDENTIFIER ASGN Expression  
    { 
        $$ = malloc(sizeof(ASGNSTRUCT));
        $$->identifier = $1;
        $$->expr = $3;
        $$->readInt = INVALID_VALUE;
    }
    | IDENTIFIER ASGN READINT
    {
        $$ = malloc(sizeof(ASGNSTRUCT));
        $$->identifier = $1;
        $$->expr = NULL;
        $$->readInt = 1;

    }
    ;

IfStatement:
    IF Expression THEN StatementSequence ElseClause END
    {
        $$ = malloc(sizeof(IFSTMT));
        $$->expr = $2;
        $$->stmtSeq = $4;
        $$->elseClause = $5;
    }
    ;

ElseClause:
    {
        $$ = NULL;
    }
    | ELSE StatementSequence
    {
        $$ = $2;
    }
    ;

WhileStatement: 
    WHILE Expression DO StatementSequence END
    {
        $$ = malloc(sizeof(WHILESTMT));
        $$->expr = $2;
        $$->stmtSeq = $4;
    }
    ;

WriteInt:
    WRITEINT Expression 
    {
        $$ = malloc(sizeof(EXPR));
        $$ = $2;
    }
    ;

Expression:
    SimpleExpression
    {
        $$ = malloc(sizeof(EXPR));
        $$->simpleExpr1 = $1;
        $$->op4 = INVALID_VALUE;
        $$->simpleExpr2 = NULL;
    }
    | SimpleExpression OP4 SimpleExpression
    {
        $$ = malloc(sizeof(EXPR));
        $$->simpleExpr1 = $1;
        $$->op4 = $2;
        $$->simpleExpr2 = $3;
        //printf("%s, %d, %s\n", $1->term1->factor1->identifier, $2, $3->term1->factor1->identifier);
    }
    ;

SimpleExpression:
    Term    
    {
        $$ = malloc(sizeof(SIMPEXPR));
        $$->term1 = $1;
        $$->op3 = INVALID_VALUE;
        $$->term2 = NULL;
    }
    | Term OP3 Term 
    {
        $$ = malloc(sizeof(SIMPEXPR));
        $$->term1 = $1;
        $$->op3 = $2;
        $$->term2 = $3;
    }
    ;

Term:
    Factor  
    {
        $$ = malloc(sizeof(TERM));
        $$->factor1 = $1;
        $$->op2 = INVALID_VALUE;
        $$->factor2 = NULL;
    }
    | Factor OP2 Factor 
    {
        $$ = malloc(sizeof(TERM));
        $$->factor1 = $1;
        $$->op2 = $2;
        $$->factor2 = $3;
    }
    ;

Factor:
    IDENTIFIER 
    {
        $$ = malloc(sizeof(FACTOR));
        $$->identifier = $1;
        $$->num = INVALID_VALUE;
        $$->boolLit = INVALID_VALUE;
        $$->expr = NULL;
        //printf("Factor $$->identifier = %s\n", $$->identifier);
    }
    | NUM 
    {
        $$ = malloc(sizeof(FACTOR));
        $$->identifier = NULL;
        $$->num = $1;
        $$->boolLit = INVALID_VALUE;
        $$->expr = NULL;
        //printf("Factor $$->num = %u\n", $$->num);
    }
    | BOOLLIT   
    {
        $$ = malloc(sizeof(FACTOR));
        $$->identifier = NULL;
        $$->num = INVALID_VALUE;
        $$->boolLit = $1;
        $$->expr = NULL;
        //printf("Factor $$->boolLit = %u\n", $$->boolLit);
    }
    | LP Expression RP  
    {
        $$ = malloc(sizeof(FACTOR));
        $$->identifier = NULL;
        $$->num = INVALID_VALUE;
        $$->boolLit = INVALID_VALUE ;
        $$->expr = $2;
        //printf("This is being recognized at one point\n");

    }
    ;

%%

int yyerror(char *s) {
    printf("yyerror: %s\n", s);
}

int yydebug = 0;

PRGRM* generateTree() {
    int isFailure = yyparse();
    if (!isFailure) {
        return root;
    }
    return NULL;
}
int yywrap() {}


