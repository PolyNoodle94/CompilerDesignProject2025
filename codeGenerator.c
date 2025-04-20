#include <stdlib.h>
#include <stdio.h>
#include "tokenParser.h"
#include "tokenParser.tab.h"
#include "uthash.h"

int tabAmnt;

int main() {
    PRGRM* root = generateTree();

    if (root == NULL) {
        printf("Error generating tree. Exiting program");
        return 1;
    } else {
        tabAmnt = 0;


        
        printProgramNode(root);
        printf("\n");
    }

}

void printProgramNode(PRGRM* node) {
    printf("#include <stdlib.h>\n");
    printf("#include <stdio.h>\n");
    printf("#include <stdbool.h>\n");
    printf("\nint main() {");
    tabAmnt++;
    printDeclarationsNode(node->decls);
    printStmtSequenceNode(node->stmtSeq);
    tabAmnt--;
    printf("\n}");

}

void printDeclarationsNode(DECLS* node) {
    while (node != NULL) {

        printf("\n");
        for (int i = 0; i < tabAmnt; i++) {
            printf("\t");
        }
        
        printf("%s %s;", node->varType ? "bool" : "int", node->identifier);
        node = node->decls;
    }
}

void printStmtSequenceNode(STMTSEQ* node) {
    while(node != NULL) {
        //Puts the statement in a new line, and indents it appropriately.
        printf("\n");
        for (int i = 0; i < tabAmnt; i++) {
            printf("\t");
        }

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
        printf("printf(\"");
        printf("%%d\", ");
        printExprNode(node->writeIntStruct);
        printf(");");
    } else {
        printf("ERROR!\n");
    }
    
}

void printAssignNode(ASGNSTRUCT* node) {
    // Assignment:
    //      IDENTIFIER := EXPRESSION
    //      IDENTIFIER := READINT
    if (node->readInt != INVALID_VALUE) {
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
    if (node->stmtSeq != NULL)      { printStmtSequenceNode(node->stmtSeq);}

    //Puts the curly in a new line, and indents it appropriately.
    // Needs the -1 because the ending curly needs to be aligned with the "if"
    printf("\n");
    for (int i = 0; i < tabAmnt - 1; i++) {
        printf("\t");
    }
    printf("}");

    if (node->elseClause != NULL)   
    { 
        printf(" else {");

        printStmtSequenceNode(node->elseClause);
        
        printf("\n");
        for (int i = 0; i < tabAmnt - 1; i++) {
            printf("\t");
        }
        printf("}");
    }
}

void printWhileStmtNode(WHILESTMT* node) {
    // WHILE <expression> DO <statementSequence> END
    printf("while ( ");
    printExprNode(node->expr);
    printf(") { ");
    if (node->stmtSeq != NULL)      { printStmtSequenceNode(node->stmtSeq);}

    printf("\n");
    for (int i = 0; i < tabAmnt - 1; i++) {
        printf("\t");
    }
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