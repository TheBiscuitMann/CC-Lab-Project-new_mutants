#ifndef SYMTAB_H
#define SYMTAB_H

// Represents a single variable in the symbol table
typedef struct Symbol {
    char *name;
    char *type;
    int scope_level;
    int line_declared; 
    struct Symbol *next;
} Symbol;

void enter_scope();
void exit_scope();
int insert_symbol(char *name, char *type, int line);
Symbol *lookup_symbol(char *name);

#endif