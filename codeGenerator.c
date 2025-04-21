#include <stdlib.h>
#include <stdio.h>
#include "tokenParser.h"
#include "tokenParser.tab.h"
#include "uthash.h"


// Declare and initialize hash table according to uthash.h standards
SYMBOL* symbolTable = NULL;
ERRORSTRUCT* errorList = NULL;
int tabAmnt;
int lineNumber;
int errorCount;

int main() {
    PRGRM* root = generateTree();

    if (root == NULL) {
        printf("Error generating tree. Exiting program");
        return 1;
    } else {
        tabAmnt = 0;
        lineNumber = 1;
        errorCount = 0;
        
        printProgramNode(root);
        printf("\n");
        printf("Total line count: %d\n", lineNumber);
        printErrorList();
        printf("\n");
    }

}

// Prints a new line, then indents it appropriately
void indentNewLine(int tabAmnt) {
    // Prints out needed tabs
    printf("\n");   lineNumber++;
    for (int i = 0; i < tabAmnt; i++) {
        printf("\t");
    }
}

// Checks if an identifier is already inside of the symbol table
// No need to check for scope conflicts since declarations can only be
// done at the start of the TL13 program according to the BNF grammar
// returns true if yes, false if no
bool isVarInSymbolTable(char* keyPtr) {
    SYMBOL* foundEntry;
    HASH_FIND_STR(symbolTable, keyPtr, foundEntry);

    if (foundEntry == NULL) {
        return false;
    } else {
        return true;
    }
}

SYMBOL* getVarFromSymbolTable(char* keyPtr) {
    SYMBOL* foundEntry;
    HASH_FIND_STR(symbolTable, keyPtr, foundEntry);
    return foundEntry;
}

// Adds new symbol to symbol table with char* identifier as the key.
void addVarToSymbolTable(DECLS* node) {
    // Declare and initialize a new symbol structure
    SYMBOL* newEntry = malloc(sizeof(SYMBOL));
    newEntry->identifier = node->identifier;
    newEntry->varType = node->varType;

    // HASH_ADD_KEYPTR is used when the struct contains a pointer to the key, rather than the key itself
    // In our case, we use "identifier" as the key, which is stored as a char* in the struct
    HASH_ADD_KEYPTR(hh, symbolTable, newEntry->identifier, strlen(newEntry->identifier), newEntry);

    // printf("New Entry on Symbol Table: \n");
    // printf("Identifier: %s\n", newEntry->identifier);
    // printf("Scope: %d\n", newEntry->scope);
    // printf("Struct Ptr Identifier: %s\n", ((DECLS*) newEntry->structPtr)->identifier);
}

void addErrorToErrorList(int errorNum, char* errorMessage, int lineNumber) {
    ERRORSTRUCT* newErrorEntry = malloc(sizeof(ERRORSTRUCT));
    newErrorEntry->id = errorNum;
    newErrorEntry->errorMessage = errorMessage;
    newErrorEntry->lineNumber = lineNumber;

    HASH_ADD_INT(errorList, id, newErrorEntry);
}

void printErrorList() {
    ERRORSTRUCT *e, *tmp;
    printf("error count: %d", HASH_COUNT(errorList));
    HASH_ITER(hh, errorList, e, tmp) {
        printf("\nError Number: %d", e->id );
        printf("\nError Message: %s", e->errorMessage );
        printf("\nLine Number: %d", e->lineNumber );
        printf("\n");
    }
}

void printProgramNode(PRGRM* node) {
    printf("#include <stdlib.h>\n");    lineNumber++;
    printf("#include <stdio.h>\n");     lineNumber++;
    printf("#include <stdbool.h>\n");   lineNumber++;

    lineNumber++;   // Incremented due to the '\n' at the start of the following printf
    printf("\nint main() {");
    tabAmnt++;
    printDeclarationsNode(node->decls);
    printStmtSequenceNode(node->stmtSeq);
    tabAmnt--;
    printf("\n}");  lineNumber++;

}

void printDeclarationsNode(DECLS* node) {
    while (node != NULL) {
        indentNewLine(tabAmnt);

        if (isVarInSymbolTable(node->identifier)) {
            errorCount++;
            char* errorMessage = "(5) Each variables may only be declared once";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        } else {
            addVarToSymbolTable(node);
        }

        printf("%s %s = %s;", node->varType ? "bool" : "int", node->identifier, node->varType ? "false" : "0");
        node = node->decls;
    }
}

void printStmtSequenceNode(STMTSEQ* node) {
    while(node != NULL) {
        indentNewLine(tabAmnt);
        printStmtNode(node->stmt);
        node = node->stmtSeq;
    }
}

void printStmtNode(STMT* node) {

    if (node->asgnStruct != NULL) {
        printAssignNode(node->asgnStruct);
    } else if (node->ifStmt != NULL) {
        tabAmnt++;
        printIfStmtNode(node->ifStmt);
        tabAmnt--;
    } else if (node->whileStmt != NULL) {
        tabAmnt++;
        printWhileStmtNode(node->whileStmt);
        tabAmnt--;
    } else if (node->writeIntStruct != NULL) {
        //WriteInt Expression
        printf("printf(\"%%d\", ");
        printExprNode(node->writeIntStruct);
        printf(");");
    } else {
        printf("ERROR!\n"); lineNumber++;
    }
    
}

void printAssignNode(ASGNSTRUCT* node) {
    // Assignment:
    //      IDENTIFIER := EXPRESSION
    //      IDENTIFIER := READINT

    // Check if using an undeclared variable
    if (isVarInSymbolTable(node->identifier) == false) {
        errorCount++;
        char* errorMessage = "(6) Trying to assign value to undeclared variable";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
    }

    if (node->readInt != INVALID_VALUE) {

        SYMBOL* leftHandVar = getVarFromSymbolTable(node->identifier);
        if (leftHandVar->varType == TYPE_BOOLEAN) {
            errorCount++;
            char* errorMessage = "(8) Only integer variables may be assigned the result of readInt.";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        }

        printf("scanf(\"%%d\", &%s)", node->identifier);

    } else {
        printf("%s = ", node->identifier);
        printExprNode(node->expr);
    }
    printf("; ");
}

void printIfStmtNode(IFSTMT* node) {
    // if statement:
    // IF <expression> THEN <statementSequence> <elseClause> END
    printf("if ( ");
    printExprNode(node->expr);
    printf(") { ");
    if (node->stmtSeq != NULL) { printStmtSequenceNode(node->stmtSeq); }

    // Needs the -1 because the ending curly needs to be aligned with the "if"
    indentNewLine(tabAmnt - 1);
    printf("}");

    if (node->elseClause != NULL)   
    { 
        printf(" else {");
        printStmtSequenceNode(node->elseClause);
        
        indentNewLine(tabAmnt - 1);
        printf("}");
    }
}

void printWhileStmtNode(WHILESTMT* node) {
    // WHILE <expression> DO <statementSequence> END
    printf("while ( ");
    printExprNode(node->expr);
    printf(") { ");
    if (node->stmtSeq != NULL) { printStmtSequenceNode(node->stmtSeq); }

    // Needs the -1 because the ending curly needs to be aligned with the "while"
    indentNewLine(tabAmnt - 1);
    printf("}");
}

void printExprNode(EXPR* node) {
    // SIMPEXPR op4 SIMPEXPR
    // SIMPEXPR
    if (node->op4 != INVALID_VALUE) {
        printSimpleExprNode(node->simpleExpr1);
        if      (node->op4 == 0) {printf("== ");}
        else if (node->op4 == 1) {printf("!= ");}
        else if (node->op4 == 2) {printf("< ");}
        else if (node->op4 == 3) {printf("> ");}
        else if (node->op4 == 4) {printf("<= ");}
        else if (node->op4 == 5) {printf(">= ");}
        printSimpleExprNode(node->simpleExpr2);
    } else {
        printSimpleExprNode(node->simpleExpr1);
    }
}

void printSimpleExprNode(SIMPEXPR* node) {
    // TERM op3 TERM
    // TERM
    if (node->op3 != INVALID_VALUE) {
        printTermNode(node->term1);
        if      (node->op3 == 0) {printf("+ ");}
        else if (node->op3 == 1) {printf("- ");}
        printTermNode(node->term2);
    } else {
        printTermNode(node->term1);
    }
}

void printTermNode(TERM* node) {
    // FACTOR op2 FACTOR
    // FACTOR
    if (node->op2 != INVALID_VALUE) {
        printFactorNode(node->factor1);
        if      (node->op2 == 0) {printf("* ");}
        else if (node->op2 == 1) {printf("/ ");}
        else if (node->op2 == 2) {printf("%% ");}
        printFactorNode(node->factor2);
    } else {
        printFactorNode(node->factor1);
    }
}

void printFactorNode(FACTOR* node) {
    if (node->identifier != NULL) {
        printf("%s ", node->identifier);
    } else if (node->num != INVALID_VALUE) {
        printf("%d ", node->num);
    } else if (node->boolLit != INVALID_VALUE) {
        printf("%s ", node->boolLit ? "true" : "false");
    } else {
        printf("( ");
        printExprNode(node->expr);
        printf(") ");
    }
}