#include <stdio.h>
#include <stdlib.h>

int main() {

    int bison_result = system("bison -d -v tokenParser.y");
    if (bison_result != 0) {
        printf("Error running <bison -d -v tokenParser.y>\n");
        return 1;
    }

    int flex_result = system("flex lexicalAnalyzer.l");
    if (flex_result != 0) {
        printf("Error running <flex lexicalAnalyzer.l>\n");
        return 1;
    }

    int gcc_result = system("gcc -DYYDEBUG tokenParser.tab.c lex.yy.c codeGenerator.c -lfl -o codeGenerator");
    if (gcc_result != 0) {
        printf("Error running <gcc -DYYDEBUG tokenParser.tab.c lex.yy.c codeGenerator.c -lfl -o codeGenerator>\n");
        return 1;
    }

    int run_result = system("./codeGenerator < inputFile.txt");
    if (run_result != 0) {
        printf("\nError running <./codeGenerator < inputFile.txt>\n");
        return 1;
    }

    return 0;
}