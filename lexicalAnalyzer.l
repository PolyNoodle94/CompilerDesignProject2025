%{
#include <stdio.h>
#include <stdlib.h>

// tokenParser.h has to be included before 
// the automatically created bison header file; 
// otherwise, the structs used in the bison file 
// that get included inside the automatically created 
// header file but defined in the manually created header 
// file won't be recognized during compilation and an 
// error will get thrown.

#include "tokenParser.h"
#include "tokenParser.tab.h"
%}

WS          [ \t\n]+
NUM         (-?[1-9][0-9]*|0)

IDENT       [A-Z][A-Z0-9]*


%%

{WS}    {}  
%[^\n]* {}

writeInt    { return(WRITEINT); }
readInt     { return(READINT); }

if      { return(IF);   }
then    { return(THEN); }
else    { return(ELSE); }
begin   { return(BEGINKEY); }
end     { return(END);  }
while   { return(WHILE);}
do      { return(DO);   }
program { return(PROGRAM); }
var     { return(VAR);  }
as      { return(AS);   }
int     { return(INT);  }
bool    { return(BOOL); }

{NUM}   { 
    yylval.iValue = atoi(yytext);
    return(NUM); 
}

false   { yylval.bValue = 0; return(BOOLLIT); }
true    { yylval.bValue = 1; return(BOOLLIT); }

{IDENT} { 
    yylval.sval = malloc(strlen(yytext));
    strncpy(yylval.sval, yytext, strlen(yytext));
    return(IDENTIFIER); 
}

\*  { yylval.op2val = 0; return(OP2); }
div { yylval.op2val = 1; return(OP2); }
mod { yylval.op2val = 2; return(OP2); }

\+  { yylval.op3val = 0; return(OP3); }
-   { yylval.op3val = 1; return(OP3); }

=   { yylval.op4val = 0; return(OP4); }
!=  { yylval.op4val = 1; return(OP4); }
\<  { yylval.op4val = 2; return(OP4); }
>   { yylval.op4val = 3; return(OP4); }
\<= { yylval.op4val = 4; return(OP4); }
>=  { yylval.op4val = 5; return(OP4); }

\(  { return(LP); }
\)  { return(RP); }
:=  { return(ASGN); }
;   { return(SC); }



%%