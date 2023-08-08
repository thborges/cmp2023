%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"

int yyerror(const char *s);
int yylex(void);
%}

%define parse.error verbose

%union {
	char *str;
	int itg;
	double flt;
	Node *node;
}

%token TOK_PRINT TOK_IDENT TOK_FLOAT TOK_INT 

%type<str> TOK_IDENT
%type<itg> TOK_INT
%type<flt> TOK_FLOAT
%type<node> factor

%start program

%%

program : stmts
        ;


stmts : stmts stmt
      | stmt
	  ;

stmt : TOK_IDENT '=' expr ';'
     | TOK_PRINT expr ';'
	 ;

expr : expr '+' term
     | expr '-' term
     | term
	 ;

term : term '*' factor
     | term '/' factor
     | factor
	 ;


factor : '(' expr ')' {
}

factor : TOK_INT {
}

factor : TOK_FLOAT {
}

factor : TOK_IDENT[id] {
	$$ = new Ident($id);
}

%%

extern int yylineno;

int yyerror(const char *s) {
	printf("erro na linha %d: %s\n", yylineno, s);
	return 1;
}

