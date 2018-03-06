%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civic.tab.h"
#include "globals.h"
#include "ctinfo.h"


#define FILTER(token) \
  global.col += yyleng;  \
  return( token);

#define LINEBUF_SIZE 256
#define YY_NO_UNPUT
#define YY_NO_INPUT


%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%option noyywrap
%option nounput

/* For MULTI/SINGLE line comments */
%x ML_COMMENT
%x SL_COMMENT


%%

"("                        { FILTER( BRACKET_L); }
")"                        { FILTER( BRACKET_R); }
","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }
"{"                        { FILTER( C_BRACKET_L); }
"}"                        { FILTER( C_BRACKET_R); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( STAR);  } 
"/"                        { FILTER( SLASH);  } 
"%"                        { FILTER( PERCENT);  }

"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    } 
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }

"="                        { FILTER( LET); }
"!"                        { FILTER( EXCL_MARK); }

"true"                     { FILTER( TRUEVAL); }
"false"                    { FILTER( FALSEVAL); }

"void"                     { FILTER( TVOID); }
"int"                      { FILTER( TINT); }
"float"                    { FILTER( TFLOAT); }
"bool"                     { FILTER( TBOOL); }

"if"                       { FILTER( KIF); }
"else"                     { FILTER( KELSE); }
"while"                    { FILTER( KWHILE); }
"do"                       { FILTER( KDO); }
"for"                      { FILTER( KFOR); }
"return"                   { FILTER( KRETURN); }

"export"                   { FILTER( EXPORT); }
"extern"                   { FILTER( VAREXTERN); }

"/*"            { global.col += 2; BEGIN(ML_COMMENT); }
<ML_COMMENT>"*/" { global.col += 2; BEGIN(INITIAL); }
<ML_COMMENT>\n   { global.line += 1; global.col = 0; }
<ML_COMMENT>.    { global.col += 1; }

"//"            { global.col += 2; BEGIN(SL_COMMENT); }
<SL_COMMENT>.    { global.col += 1; }
<SL_COMMENT>\n   { global.line += 1; global.col = 0; BEGIN(INITIAL); }


[A-Za-z][A-Za-z0-9_]*      { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }

[0-9]+                     { yylval.cint=atoi(yytext);
                             FILTER( NUM);
                           }

\n.*                       { global.line += 1;
                             global.col = 0;
                             yyless(1);
                           }

[ \t]                      { global.col += yyleng;
                           }

