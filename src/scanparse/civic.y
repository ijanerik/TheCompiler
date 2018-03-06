%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

%}

%union {
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cflt;
 binop               cbinop;
 monop               cmonop;
 cctype             ccctype;
 node               *node;
}

%token BRACKET_L BRACKET_R COMMA SEMICOLON C_BRACKET_L C_BRACKET_R
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND EXCL_MARK
%token TRUEVAL FALSEVAL LET
%token TBOOL TVOID TINT TFLOAT VAREXTERN EXPORT
%token KIF KELSE KWHILE KDO KFOR KRETURN

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

//%type <node> stmts stmt assign varlet program

//%type <cbinop> binop
%type <cbinop> binop1
%type <cbinop> binop2
%type <cbinop> binop3
%type <cbinop> binop4
%type <cbinop> binop5
%type <cbinop> binop6
%type <cmonop> monop
%type <ccctype> rettype
%type <ccctype> vartype

%type <node> ident intval floatval boolval constant exprs
%type <node> expr expr2 expr3 expr4 expr5 expr6 expr7 expr8
%type <node> stmts stmt assign ifelsestmt whilestmt dowhilestmt forstmt returnstmt block funcall
%type <node> vardec vardecs
%type <node> program declarations declaration
%type <node> globaldef fundef funheader funbody params param

%start program

%%
// ------- PROGRAM DEFINITIONS ---------
program: declarations
         {
           parseresult = $1;
         }
         ;

declarations: declaration declarations
        {
          $$ = TBmakeDeclarations($1, $2);
        }
      | declaration
        {
          $$ = TBmakeDeclarations($1, NULL);
        }
        ;

declaration: globaldef { $$ = $1; }
         |   fundef { $$ = $1; }
         |   stmt { $$ = $1; };
         // @todo remove stmts (Kept for backward compatibility)

// ----------- GLOBAL VARIABLE DEFINITIONS -----------
globaldef: vartype ident LET expr SEMICOLON
         {
            $$ = TBmakeGlobaldef($1, FALSE, $2, $4);
         }
         | EXPORT vartype ident LET expr SEMICOLON
         {
            $$ = TBmakeGlobaldef($2, TRUE, $3, $5);
         }
         |
         vartype ident SEMICOLON
         {
            $$ = TBmakeGlobaldef($1, FALSE, $2, NULL);
         }
         | EXPORT vartype ident SEMICOLON
         {
            $$ = TBmakeGlobaldef($2, TRUE, $3, NULL);
         };

// ----------- FUNCTION DEFINITIONS ---------------
fundef: funheader funbody
    {
        $$ = TBmakeFundef(FALSE, $1, $2);
    }
    | EXPORT funheader funbody
    {
        $$ = TBmakeFundef(TRUE, $2, $3);
    }
    |
    VAREXTERN funheader SEMICOLON
    {
        $$ = TBmakeFundef(FALSE, $2, NULL);
    }
    ;

// @todo How to nest vartypes en rettypes
funheader: vartype ident BRACKET_L params BRACKET_R
    {
        $$ = TBmakeFunheader($1, $2, $4);
    }
    | vartype ident BRACKET_L BRACKET_R
    {
        $$ = TBmakeFunheader($1, $2, NULL);
    }
    |
    rettype ident BRACKET_L params BRACKET_R
    {
        $$ = TBmakeFunheader($1, $2, $4);
    }
    | rettype ident BRACKET_L BRACKET_R
    {
        $$ = TBmakeFunheader($1, $2, NULL);
    };

params: param COMMA params
        {
          $$ = TBmakeParams($1, $3);
        }
      | param
        {
          $$ = TBmakeParams($1, NULL);
        }
        ;

param: vartype ident
    {
        $$ = TBmakeParam($1, $2);
    }

funbody:
    C_BRACKET_L vardecs stmts C_BRACKET_R
    {
        $$ = TBmakeFunbody(NULL, NULL, $2);
    }
    | C_BRACKET_L vardecs C_BRACKET_R
    {
        $$ = TBmakeFunbody(NULL, NULL, $2);
    }
    | C_BRACKET_L stmts C_BRACKET_R
    {
        $$ = TBmakeFunbody(NULL, NULL, $2);
    }
    | C_BRACKET_L C_BRACKET_R
      {
          $$ = TBmakeFunbody(NULL, NULL, NULL);
      }
    ;

// ----------- STATEMENT DEFINITIONS --------------
vardecs: vardec vardecs {
            $$ = TBmakeVardecs($1, $2);
        }
        |
        vardec {
            $$ = TBmakeVardecs($1, NULL);
        }
        ;

vardec: vartype ident SEMICOLON {
            $$ = TBmakeVardec($1, $2, NULL);
        }
        |
        vartype ident LET expr SEMICOLON {
            $$ = TBmakeVardec($1, $2, $4);
        };

stmts: stmt stmts
        {
          $$ = TBmakeStmts( $1, $2);
        }
      | stmt
        {
          $$ = TBmakeStmts( $1, NULL);
        }
        ;

stmt:    assign { $$ = $1; }
       | ifelsestmt { $$ = $1; }
       | funcall SEMICOLON { $$ = $1; }
       | whilestmt { $$ = $1; }
       | dowhilestmt { $$ = $1; }
       | forstmt { $$ = $1; }
       | returnstmt { $$ = $1; }
       ;

// ------------ STATEMENT FUNCTIONS ---------------
assign: ident LET expr SEMICOLON
        {
          $$ = TBmakeAssign( $1, $3);
        }
        ;

ifelsestmt: KIF BRACKET_L expr BRACKET_R block
        {
            $$ = TBmakeIfelsestmt($3, $5, NULL);
        }
        | KIF BRACKET_L expr BRACKET_R block KELSE block
        {
            $$ = TBmakeIfelsestmt($3, $5, $7);
        }
        ;

whilestmt: KWHILE BRACKET_L expr BRACKET_R block
        {
            $$ = TBmakeWhilestmt($3, $5);
        }

dowhilestmt: KDO block KWHILE BRACKET_L expr BRACKET_R SEMICOLON
        {
            $$ = TBmakeDowhilestmt($5, $2);
        }

forstmt: KFOR BRACKET_L TINT ident LET expr COMMA expr BRACKET_R block
        {
            $$ = TBmakeForstmt($4, $6, $8, NULL, $10);
        }
        |
        KFOR BRACKET_L TINT ident LET expr COMMA expr COMMA expr BRACKET_R block
        {
            $$ = TBmakeForstmt($4, $6, $8, $10, $12);
        }
        ;

returnstmt: KRETURN expr SEMICOLON {
            $$ = TBmakeReturnstmt($2);
        }
        | KRETURN SEMICOLON
        {
            $$ = TBmakeReturnstmt(NULL);
        };

block: C_BRACKET_L stmts C_BRACKET_R
        {
            $$ = TBmakeBlock($2);
        }
        | C_BRACKET_L C_BRACKET_R
        {
           $$ = TBmakeBlock(NULL);
        }
        |
        stmt
        {
            $$ = TBmakeBlock(TBmakeStmts($1, NULL));
        }
        ;

funcall: ident BRACKET_L exprs BRACKET_R {
            $$ = TBmakeFuncall($1, $3);
        }
        | ident BRACKET_L BRACKET_R {
            $$ = TBmakeFuncall($1, NULL);
        }

// ----------- EXPRESSION DEFINITIONS -----------
exprs: expr COMMA exprs {
            $$ = TBmakeExprs($1, $3);
        }
        | expr {
            $$ = TBmakeExprs($1, NULL);
        }

/*
expr: constant { $$ = $1; }
    | funcall { $$ = $1; }
    | ID { $$ = TBmakeIdent( STRcpy( $1)); }
    | BRACKET_L expr BRACKET_R { $$ = $2; }
    | BRACKET_L expr binop expr BRACKET_R { $$ = TBmakeBinop( $3, $2, $4); }
    | BRACKET_L monop expr BRACKET_R { $$ = TBmakeMonop($2, $3); }
    | BRACKET_L BRACKET_L vartype BRACKET_R expr BRACKET_R { $$ = TBmakeCastexpr($3, $5); }
    ;
*/

expr: expr binop1 expr2 { $$ = TBmakeBinop( $2, $1, $3); }
     | expr2;

expr2: expr2 binop2 expr3 { $$ = TBmakeBinop( $2, $1, $3); }
     | expr3;

expr3: expr3 binop3 expr4 { $$ = TBmakeBinop( $2, $1, $3); }
     | expr4;

expr4: expr4 binop4 expr5 { $$ = TBmakeBinop( $2, $1, $3); }
     | expr5;

expr5: expr5 binop5 expr6 { $$ = TBmakeBinop( $2, $1, $3); }
    | expr6
    ;
expr6: expr6 binop6 expr7 { $$ = TBmakeBinop( $2, $1, $3); }
     | expr7;
expr7: monop expr7 { $$ = TBmakeMonop($1, $2); }
     | BRACKET_L vartype BRACKET_R expr7 { $$ = TBmakeCastexpr($2, $4); }
     | expr8;
expr8: BRACKET_L expr BRACKET_R { $$ = $2; }
     | constant { $$ = $1; }
     | funcall { $$ = $1; }
     | ID { $$ = TBmakeIdent( STRcpy( $1)); }

constant: floatval
          {
            $$ = $1;
          }
        | intval
          {
            $$ = $1;
          }
        | boolval
          {
            $$ = $1;
          }
        ;

floatval: FLOAT
           {
             $$ = TBmakeFloat( $1);
           }
         ;

intval: NUM
        {
          $$ = TBmakeNum( $1);
        }
      ;

boolval: TRUEVAL
         {
           $$ = TBmakeBool( TRUE);
         }
       | FALSEVAL
         {
           $$ = TBmakeBool( FALSE);
         }
       ;

/*
binop: PLUS      { $$ = BO_add; }
     | MINUS     { $$ = BO_sub; }
     | STAR      { $$ = BO_mul; }
     | SLASH     { $$ = BO_div; }
     | PERCENT   { $$ = BO_mod; }
     | LE        { $$ = BO_le; }
     | LT        { $$ = BO_lt; }
     | GE        { $$ = BO_ge; }
     | GT        { $$ = BO_gt; }
     | EQ        { $$ = BO_eq; }
     | OR        { $$ = BO_or; }
     | AND       { $$ = BO_and; }
     ;
*/

binop1: OR        { $$ = BO_or; };

binop2: AND       { $$ = BO_and; };

binop3: EQ        { $$ = BO_eq; };

binop4: LE        { $$ = BO_le; }
      | LT        { $$ = BO_lt; }
      | GE        { $$ = BO_ge; }
      | GT        { $$ = BO_gt; };

binop5: PLUS      { $$ = BO_add; }
      | MINUS     { $$ = BO_sub; };

binop6: STAR      { $$ = BO_mul; }
      | SLASH     { $$ = BO_div; }
      | PERCENT   { $$ = BO_mod; };


monop: MINUS    { $$ = MO_neg; }
     | EXCL_MARK { $$ = MO_not; }
     ;

vartype: TINT   { $$ = T_int; }
     |   TFLOAT { $$ = T_float; }
     |   TBOOL  { $$ = T_bool; };

rettype: TVOID  { $$ = T_void; };

ident: ID
        {
          $$ = TBmakeIdent( STRcpy( $1));
        }
        ;



%%

static int yyerror( char *error)
{
  CTIabort( "line %d, col %d\nError parsing source code: %s\n", 
            global.line, global.col, error);

  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

