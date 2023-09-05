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
%type<node> factor expr stmts stmt term

%start program

%%

program : stmts {
	Program *p = new Program();
	p->addChild($stmts);

	// analise semântica
	PrintTree pt;
	pt.print(p);

	CheckUndeclaredVar ck;
	ck.check(p);

}

stmts : stmts[ss] stmt {
	$ss->addChild($stmt);
	$$ = $ss;	
}

stmts : stmt {
	Node *stmts = new Node();
	stmts->addChild($stmt);
	$$ = stmts;
}

stmt : TOK_IDENT[id] '=' expr ';' {
	$$ = new Attr($id, $expr);
}

stmt : TOK_PRINT expr ';' {
	$$ = new Print($expr);
}

expr : expr[ex] '+' term  {
	$$ = new BinaryOp(
		$ex, $term, '+');
}

expr : expr[ex] '-' term {
	$$ = new BinaryOp(
		$ex, $term, '-');
}

expr : term {
	$$ = $term;
}

term : term[te] '*' factor {
	$$ = new BinaryOp(
		$te, $factor, '*');
}

term : term[te] '/' factor {
	$$ = new BinaryOp(
		$te, $factor, '/');
}

term : factor {
	$$ = $factor;
}

factor : '(' expr ')' {
	$$ = $expr;
}

factor : TOK_INT[itg] {
	$$ = new Int($itg);
}

factor : TOK_FLOAT[flt] {
	$$ = new Float($flt);
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

