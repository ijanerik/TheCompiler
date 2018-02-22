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
%token TBOOL TVOID TINT TFLOAT VAREXTERN VAREXPORT
%token KIF KELSE KWHILE KDO KFOR KRETURN

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

//%type <node> stmts stmt assign varlet program

%type <cbinop> binop
%type <cmonop> monop
%type <ccctype> vartype
%type <ccctype> rettype

%type <node> ident intval floatval boolval constant expr
%type <node> stmts stmt assign ifelsestmt block
%type <node> program globaldef declarations declaration

%start program

%%
// ------- GLOBAL DEFINITIONS ---------
program: stmts
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

declaration: globaldef { $$ = $1; };

globaldef: vartype ident LET expr SEMICOLON
         {
            $$ = TBmakeGlobaldef($1, FALSE, $2, $4);
         }
         | VAREXPORT vartype ident LET expr SEMICOLON
         {
            $$ = TBmakeGlobaldef($2, TRUE, $3, $5);
         }
         |
         vartype ident SEMICOLON
         {
            $$ = TBmakeGlobaldef($1, FALSE, $2, NULL);
         }
         | VAREXPORT vartype ident SEMICOLON
         {
            $$ = TBmakeGlobaldef($2, TRUE, $3, NULL);
         };

// ----------- STATEMENT DEFINITIONS --------------
stmts: stmt stmts
        {
          $$ = TBmakeStmts( $1, $2);
        }
      | stmt
        {
          $$ = TBmakeStmts( $1, NULL);
        }
        ;

stmt: assign
       {
         $$ = $1;
       }
       |
       ifelsestmt
       {
         $$ = $1;
       }
       ;


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

block: C_BRACKET_L stmts C_BRACKET_R
        {
            $$ = TBmakeBlock($2);
        }
        |
        stmt
        {
            $$ = TBmakeBlock(TBmakeStmts($1, NULL));
        }
        ;

// ----------- EXPRESSION DEFINITIONS -----------
expr: constant
    {
        $$ = $1;
    }
    | ID
    {
        $$ = TBmakeIdent( STRcpy( $1));
    }
    | BRACKET_L expr BRACKET_R
    {
        $$ = $2;
    }
    | BRACKET_L expr binop expr BRACKET_R
    {
        $$ = TBmakeBinop( $3, $2, $4);
    }
    | BRACKET_L monop expr BRACKET_R
    {
        $$ = TBmakeMonop($2, $3);
    }
    | BRACKET_L BRACKET_L vartype BRACKET_R expr BRACKET_R
    {
        $$ = TBmakeCastexpr($3, $5);
    }
    ;

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

monop: MINUS    { $$ = MO_neg; }
     | EXCL_MARK { $$ = MO_not; }
     ;

vartype: TINT   { $$ = T_int; }
     |   TFLOAT { $$ = T_float; }
     |   TBOOL  { $$ = T_bool; }
     ;

rettype: TINT   { $$ = T_int; }
     |   TFLOAT { $$ = T_float; }
     |   TBOOL  { $$ = T_bool; }
     |   TVOID  { $$ = T_void; }
     ;


ident: ID
        {
          $$ = TBmakeIdent( STRcpy( $1));
        }
        ;





/*
stmts: stmt stmts
    {
        $$ = TBmakeStmts( $1, $2);
    }
    | stmt
    {
        $$ = TBmakeStmts( $1, NULL);
    }
    ;

stmt: assign
    {
        $$ = $1;
    }
    |
    ;

assign: varlet LET expr SEMICOLON
        {
          $$ = TBmakeAssign( $1, $3);
        }
        ;


expr: constant
      {
        $$ = $1;
      }
    | ID
      {
        $$ = TBmakeIdent( STRcpy( $1));
      }
    | BRACKET_L expr binop expr BRACKET_R
      {
        $$ = TBmakeBinop( $3, $2, $4);
      }
    ;

monop: ;
cctype: ;
*/



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

