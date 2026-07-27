#ifndef SYMTAB_H
#define SYMTAB_H

// Represents a single variable in the symbol table
typedef struct Symbol {
    char *name;
    char *type; // "int", "float", or "bool"
    int scope_level;
    struct Symbol *next;
} Symbol;


void enter_scope();
void exit_scope();
int insert_symbol(char *name, char *type);
Symbol* lookup_symbol(char *name);

#endif