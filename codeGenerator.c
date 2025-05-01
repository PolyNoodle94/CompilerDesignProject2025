#include <stdlib.h>
#include <stdio.h>
#include "tokenParser.h"
#include "tokenParser.tab.h"
#include "uthash.h"


// Declare and initialize hash table according to uthash.h standards
SYMBOL* symbolTable = NULL;
ERRORSTRUCT* errorList = NULL;


int tabAmnt;    // used to indent printed C code
int lineNumber; // keeps track of what line an error occurs in
int errorCount; // keeps track of the current number of errors

int main() {
    // Basically a wrapper call for yyparse() in the tokenParser.y bison file
    PRGRM* root = generateTree();

    if (root == NULL) {
        printf("Error generating tree. Exiting program");
        return 1;
    } else {
        tabAmnt = 0;    // indentation starts at 0 tabs
        lineNumber = 1; // starting on line 1, incremented after every '\n' character
        errorCount = 0; // initialized to zero errors
        
        printProgramNode(root);

        // the following is not part of the translated TL13 program
        printf("\n\n\n");
        printf("Total line count: %d\n", lineNumber);
        printErrorList();   // for each error, prints the number, message and line it occured ons
        printf("\n");
    }

}

// ####################### HASH TABLE METHODS START ####################

// Checks if an identifier is already inside of the symbol table
// No need to check for scope conflicts since declarations can only be
// done at the start of the TL13 program according to the BNF grammar
// returns true if yes, false if no
bool isVarInSymbolTable(char* keyPtr) {
    SYMBOL* foundEntry;
    HASH_FIND_STR(symbolTable, keyPtr, foundEntry); // store symbolTable(keyPtr) inside foundEntry

    if (foundEntry == NULL) {
        return false;   // if NULL, identifier was not in symbol table
    } else {
        return true;    // if not NULL, identifier was in symbol table
    }
}

// returns SYMBOL* found at symbolTable(keyPtr)
SYMBOL* getVarFromSymbolTable(char* keyPtr) {
    SYMBOL* foundEntry;
    HASH_FIND_STR(symbolTable, keyPtr, foundEntry); // has no return value, instead returns the found entry in foundEntry
    return foundEntry;
}

// Adds new symbol to symbol table with char* identifier as the key.
void addVarToSymbolTable(DECLS* node) {
    // Declare and initialize a new symbol structure ptr
    SYMBOL* newEntry = malloc(sizeof(SYMBOL));
    newEntry->identifier = node->identifier;
    newEntry->varType = node->varType;

    // HASH_ADD_KEYPTR is used when the struct contains a pointer to the key, rather than the key itself
    // In our case, we use "identifier" as the key, which is stored as a char* in the struct
    HASH_ADD_KEYPTR(hh, symbolTable, newEntry->identifier, strlen(newEntry->identifier), newEntry);
}

// adds an ERRORSTRUCT* to the errorList hash table
void addErrorToErrorList(int errorNum, char* errorMessage, int lineNumber) {
    // Declare and initialize a new ERRORSTRUCT* structure ptr
    ERRORSTRUCT* newErrorEntry = malloc(sizeof(ERRORSTRUCT));
    newErrorEntry->id = errorNum;
    newErrorEntry->errorMessage = errorMessage;
    newErrorEntry->lineNumber = lineNumber;

    // current number of errors is used as the hash key for an entry in the errorList hash table
    HASH_ADD_INT(errorList, id, newErrorEntry);
}

// Iterates through all entries inside the errorList hash table and prints them out
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

// ####################### HASH TABLE METHODS END ####################

// ####################### AUXILIARY METHDOS START ###################

// Check if a nested expression is an integer
bool isNestedExpressionsAnInteger(EXPR* expression) {
    // Dive into the right-most parantheses
    SIMPEXPR* simpleExpression = (expression->simpleExpr2 != NULL) ? expression->simpleExpr2 : expression->simpleExpr1;

    TERM* term = (simpleExpression->term2 != NULL) ? simpleExpression->term2 : simpleExpression->term1;

    FACTOR* factor = (term->factor2 != NULL) ? term->factor2 : term->factor1;

    // if nested expressions, recurse
    if (factor->expr != NULL) {
        isNestedExpressionsAnInteger(factor->expr);
    }

    return isExpressionAnInteger(expression);
}

// EXPR* uses OP4 operators (boolean operators)
bool isExpressionAnInteger(EXPR* expression) {
    // if expression is 
    if (expression->simpleExpr2 != NULL) {
        if (isSimpleExpressionAnInteger(expression->simpleExpr1) && isSimpleExpressionAnInteger(expression->simpleExpr2)) {
            return true;
        }
        errorCount++;
        char* errorMessage = "(1) OP4 Arguments must be integers";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
        return false;
    }

    if (isSimpleExpressionAnInteger(expression->simpleExpr1)) {
        return true;
    } 
    return false;

}

bool isSimpleExpressionAnInteger(SIMPEXPR* simpleExpression) {
    // if simpleExpression is an operation between two terms
    if (simpleExpression->term2 != NULL) {
        // if both terms are integers
        if (isTermAnInteger(simpleExpression->term1) && isTermAnInteger(simpleExpression->term2)) {
            return true;
        }
        // ERROR: OP3 arguments must be integers
        errorCount++;
        char* errorMessage = "(1) OP3 Arguments must be integers";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
        return false;
    }

    // if simpleExpression is a single term
    if (isTermAnInteger(simpleExpression->term1)) {
        return true;
    }
    return false;
}

bool isTermAnInteger(TERM* term) {
    // if term is an operation between two factors
    if (term->factor2 != NULL) {
        // if both factors are integers
        if (isFactorAnInteger(term->factor1) && isFactorAnInteger(term->factor2)) {
            return true;
        }
        // error if a factor is not an integer in an OP2 operation
        errorCount++;
        char* errorMessage = "(1) OP2 Arguments must be integers";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
        return false;
    } 
    
    // if term is a single factor
    if (isFactorAnInteger(term->factor1)) {
        return true;
    }
    return false;

}

bool isFactorAnInteger(FACTOR* factor) { 
    if (factor->num != INVALID_VALUE) {
        // if factor is a num, return true
        return true;
    }
    else if (factor->identifier != NULL) {
        // if factor is an identifier and not an integer, return false. otherwise, return true
        SYMBOL* fetchedSymbol = getVarFromSymbolTable(factor->identifier);
        if (fetchedSymbol->varType != TYPE_INTEGER) {
            return false;
        }
        return true;
    }
    return false;
}

// ####################### AUXILIARY METHDOS END #####################

// ####################### PRINTING METHODS START ####################

// Prints a new line, then indents it appropriately
void indentNewLine(int tabAmnt) {
    // Prints out needed tabs
    printf("\n");   lineNumber++;
    for (int i = 0; i < tabAmnt; i++) {
        printf("\t");
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
        // WriteInt Expression
        // WriteInt expression must resolve to an integer
        if (isNestedExpressionsAnInteger(node->writeIntStruct) == false) {
            errorCount++;
            char* errorMessage = "(9) writeInt's expression must evaluate to an integer.";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        }
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
        printf("%s = ", node->identifier);
        printExprNode(node->expr);
        printf(";");
        return;
    }

    SYMBOL* leftHandVar = getVarFromSymbolTable(node->identifier);
    if (node->readInt != INVALID_VALUE) {

        if (leftHandVar->varType == TYPE_BOOLEAN) {
            errorCount++;
            char* errorMessage = "(8) Only integer variables may be assigned the result of readInt.";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        }

        printf("scanf(\"%%d\", &%s)", node->identifier);

    } else {
        // check if identifier is integer but expression is not
        if (leftHandVar->varType == TYPE_INTEGER && isNestedExpressionsAnInteger(node->expr) == false) {
            errorCount++;
            char* errorMessage = "(6) Variables of Type INT must be assgined integer values";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        }

        // check if identifier is boolean but expression is not
        if (leftHandVar->varType == TYPE_BOOLEAN && isNestedExpressionsAnInteger(node->expr) == true) {
            errorCount++;
            char* errorMessage = "(6) Variables of Type BOOL must be assgined boolean values";
            addErrorToErrorList(errorCount, errorMessage, lineNumber);
        }
        printf("%s = ", node->identifier);
        printExprNode(node->expr);
    }
    printf("; ");
}

void printIfStmtNode(IFSTMT* node) {
    // if statement:
    // IF <expression> THEN <statementSequence> <elseClause> END
    printf("if ( ");
    if (isNestedExpressionsAnInteger(node->expr) == true) {
        errorCount++;
        char* errorMessage = "(10) The expression guarding if-statements and while-loops must be boolean.";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
    }
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
    if (isNestedExpressionsAnInteger(node->expr) == true) {
        errorCount++;
        char* errorMessage = "(10) The expression guarding if-statements and while-loops must be boolean.";
        addErrorToErrorList(errorCount, errorMessage, lineNumber);
    }
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

// ####################### PRINTING METHODS END ####################