%{
#include <stdio.h>
#include <stdlib.h>
#include "src/ast/ast.h"
#include "src/semantic/semantic.h"
#include "src/symbol_table/symtab.h"
#include "src/codegen/tac.h"

// External functions and variables provided by your teammate's lexer
extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char *s);

// The root of our Abstract Syntax Tree
ASTNode* root_node = NULL;
%}

%union {
    int ival;
    float fval;
    char *sval;
    struct ASTNode *node; // Allows grammar rules to pass AST nodes up the tree
}

/* Tokens with values from the lexer */
%token <ival> INT_LIT
%token <fval> FLOAT_LIT
%token <sval> IDENTIFIER

/* Keywords and operators (no semantic value needed here, just the token) */
%token INT FLOAT BOOL IF ELSE WHILE PRINT TRUE FALSE
%token PLUS MINUS TIMES DIVIDE MOD
%token LT GT LE GE EQ NE AND OR NOT
%token ASSIGN
%token LBRACE RBRACE LPAREN RPAREN SEMICOLON

/* Non-terminal types mapped to our AST node struct */
%type <node> program statement_list statement declaration type assignment 
%type <node> if_statement while_statement print_statement block expression

/* Operator Precedence and Associativity (Lowest to Highest) */
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right NOT UMINUS

%%

/* GRAMMAR RULES AND AST ACTIONS */

program: statement_list { root_node = create_node(NODE_PROGRAM, $1, NULL, NULL); $$ = root_node; }
       ;

statement_list: statement_list statement { $$ = create_node(NODE_STATEMENT_LIST, $1, NULL, $2); }
              | /* empty */ { $$ = NULL; }
              ;

statement: declaration { $$ = $1; }
         | assignment { $$ = $1; }
         | if_statement { $$ = $1; }
         | while_statement { $$ = $1; }
         | print_statement { $$ = $1; }
         | block { $$ = $1; }
         ;

declaration: type IDENTIFIER SEMICOLON { 
                 // 1. First, check if the variable ALREADY exists in the Symbol Table
                 if (lookup_symbol($2) != NULL) {
                     printf("Semantic Error: Redeclaration of variable '%s'.\n", $2);
                     exit(1);
                 }
                 
                 // 2. If it is safe, officially register the variable and its type
                 insert_symbol($2, $1->sval); 
                 
                 // 3. Build the AST node as usual
                 $$ = create_node(NODE_DECLARATION, $1, create_leaf_str(NODE_IDENTIFIER, $2), NULL); 
             }
           ;

type: INT { $$ = create_leaf_str(NODE_TYPE, "int"); }
    | FLOAT { $$ = create_leaf_str(NODE_TYPE, "float"); }
    | BOOL { $$ = create_leaf_str(NODE_TYPE, "bool"); }
    ;

assignment: IDENTIFIER ASSIGN expression SEMICOLON { $$ = create_node(NODE_ASSIGNMENT, create_leaf_str(NODE_IDENTIFIER, $1), NULL, $3); }
          ;

if_statement: IF LPAREN expression RPAREN statement { $$ = create_node(NODE_IF, $3, $5, NULL); }
            | IF LPAREN expression RPAREN statement ELSE statement { $$ = create_node(NODE_IF_ELSE, $3, $5, $7); }
            ;

while_statement: WHILE LPAREN expression RPAREN statement { $$ = create_node(NODE_WHILE, $3, $5, NULL); }
               ;

print_statement: PRINT expression SEMICOLON { $$ = create_node(NODE_PRINT, $2, NULL, NULL); }
               ;

block: LBRACE { enter_scope(); } statement_list RBRACE { 
           exit_scope();
           $$ = create_node(NODE_BLOCK, $3, NULL, NULL); 
       }
     ;

expression: expression PLUS expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = PLUS; }
          | expression MINUS expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = MINUS; }
          | expression TIMES expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = TIMES; }
          | expression DIVIDE expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = DIVIDE; }
          | expression MOD expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = MOD; }
          | expression LT expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = LT; }
          | expression GT expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = GT; }
          | expression LE expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = LE; }
          | expression GE expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = GE; }
          | expression EQ expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = EQ; }
          | expression NE expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = NE; }
          | expression AND expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = AND; }
          | expression OR expression { $$ = create_node(NODE_BINOP, $1, NULL, $3); $$->op = OR; }
          | NOT expression { $$ = create_node(NODE_UNOP, $2, NULL, NULL); $$->op = NOT; }
          | MINUS expression %prec UMINUS { $$ = create_node(NODE_UNOP, $2, NULL, NULL); $$->op = MINUS; }
          | LPAREN expression RPAREN { $$ = $2; }
          | IDENTIFIER { $$ = create_leaf_str(NODE_IDENTIFIER, $1); }
          | INT_LIT { $$ = create_leaf_int($1); }
          | FLOAT_LIT { $$ = create_leaf_float($1); }
          | TRUE { $$ = create_leaf_str(NODE_BOOL_LIT, "true"); }
          | FALSE { $$ = create_leaf_str(NODE_BOOL_LIT, "false"); }
          ;

%%

/* C CODE FUNCTIONS */

// Called by yyparse on error
void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}

int main(void) {
    // yyparse returns 0 on successful parsing
    if (yyparse() == 0) {
        printf("Parsing completed successfully!\n");
        
        if (root_node != NULL) {
            printf("\n--- Abstract Syntax Tree ---\n");
            print_ast(root_node, 0);
            
            printf("\n--- Semantic Analysis ---\n");
            check_semantics(root_node);
            printf("Semantic Analysis completed with 0 errors!\n");
            generate_tac(root_node);
        }
    } else {
        printf("Parsing failed.\n");
    }
    return 0;
}