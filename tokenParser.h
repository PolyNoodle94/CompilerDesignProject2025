/*
* Author: Emilio Ortiz Gonzalez
* Date: April 16th 2025
* Purpose: This file is used to declare all the structs
*   needed in order for the bison file (tokenParser.y) to
*   
*
*
*/



#ifndef TOKENPARSER_H
#define TOKENPARSER_H

#include <stdbool.h>
#include <uthash.h>

// essentially a NULL for non-pointer types
// not a problem since negative values aren't valid token lexeemes in the first place
#define INVALID_VALUE -1 

// typedef 
typedef struct Symbol Symbol;
typedef struct PRGRM PRGRM;
typedef struct DECLS DECLS;         // DECLS refers to struct DECLS
typedef struct STMTSEQ STMTSEQ;
typedef struct STMT STMT;
typedef struct ASGNSTRUCT ASGNSTRUCT;
typedef struct IFSTMT IFSTMT;
typedef struct WHILESTMT WHILESTMT;
typedef struct EXPR EXPR;
typedef struct SIMPEXPR SIMPEXPR;
typedef struct TERM TERM;
typedef struct FACTOR FACTOR;

// forward declaration of structs
struct PRGRM;
struct DECLS;
struct STMTSEQ;
struct STMT;
struct ASGNSTRUCT;
struct IFSTMT;
struct WHILESTMT;
struct EXPR;
struct SIMPEXPR;
struct TERM;
struct FACTOR;

// prototype of functions
PRGRM* generateTree();
void printProgramNode(PRGRM*);
void printDeclarationsNode(DECLS*);
void printStmtSequenceNode(STMTSEQ*);
void printStmtNode(STMT*);
void printAssignNode(ASGNSTRUCT*);
void printIfStmtNode(IFSTMT*);
void printWhileStmtNode(WHILESTMT*);
void printExprNode(EXPR*);
void printSimpleExprNode(SIMPEXPR*);
void printTermNode(TERM*);
void printFactorNode(FACTOR*);

/*
OP2(0) -> *
OP2(1) -> div
OP2(2) -> mod

OP3(0) -> +
OP3(1) -> -

OP4(0) -> =
OP4(1) -> !=
OP4(2) -> <
OP4(3) -> >
OP4(4) -> <=
OP4(5) -> >=
*/

//Symbol table structs
struct Symbol{
    
}

// Structs for each nonterminal (where needed; for example, WriteInt does not need its own struct)
struct PRGRM{
    DECLS * decls;
    STMTSEQ * stmtSeq;
    UT_hash_handle hh;  // makes this struct hashable
};

struct DECLS {
    char* identifier;
    bool varType;
    DECLS* decls;
    UT_hash_handle hh;  // makes this struct hashable
};

struct STMTSEQ{
    STMT* stmt;
    STMTSEQ* stmtSeq;
    UT_hash_handle hh;  // makes this struct hashable
};

struct STMT{
    ASGNSTRUCT* asgnStruct;
    IFSTMT* ifStmt;
    WHILESTMT* whileStmt;
    EXPR*  writeIntStruct;    // <WriteInt> is really just an Expression wrapper, so i just reused the EXPR struct
    UT_hash_handle hh;  // makes this struct hashable
};

struct ASGNSTRUCT {
    char* identifier;
    EXPR* expr;
    int readInt; // "read" as in "the integer has already been read. This is the read int"
    UT_hash_handle hh;  // makes this struct hashable
};

struct IFSTMT {
    EXPR* expr;
    STMTSEQ* stmtSeq;
    STMTSEQ* elseClause;
    UT_hash_handle hh;  // makes this struct hashable
};

struct WHILESTMT {
    EXPR* expr;
    STMTSEQ* stmtSeq;
    UT_hash_handle hh;  // makes this struct hashable
};

struct EXPR {
    SIMPEXPR* simpleExpr1;
    int op4;
    SIMPEXPR* simpleExpr2;
    UT_hash_handle hh;  // makes this struct hashable
};

struct SIMPEXPR {
    TERM* term1;
    int op3;
    TERM* term2;
    UT_hash_handle hh;  // makes this struct hashable
};

// struct for Term
struct TERM {
    FACTOR* factor1;
    int op2;
    FACTOR* factor2;
    UT_hash_handle hh;  // makes this struct hashable
};

// struct for Factor
struct FACTOR {
    char* identifier;
    int num;
    int boolLit;
    EXPR* expr;
    UT_hash_handle hh;  // makes this struct hashable
};

#endif //tokenParser.h

