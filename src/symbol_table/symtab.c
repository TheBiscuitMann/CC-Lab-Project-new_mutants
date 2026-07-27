#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

Symbol *symbol_table = NULL;
int current_scope = 0;

// Increases the scope level when entering a { block }
void enter_scope() {
    current_scope++;
}

// Decreases the scope level and removes variables inside the block we just left
void exit_scope() {
    Symbol *curr = symbol_table;
    Symbol *prev = NULL;

    while (curr != NULL) {
        if (curr->scope_level == current_scope) {
            // Remove this symbol
            if (prev == NULL) {
                symbol_table = curr->next;
            } else {
                prev->next = curr->next;
            }
            Symbol *temp = curr;
            curr = curr->next;
            free(temp->name);
            free(temp->type);
            free(temp);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    current_scope--;
}

// Adds a new variable to the table. Returns 0 if already declared in THIS scope.
int insert_symbol(char *name, char *type) {
    // Check if it already exists in the CURRENT scope
    Symbol *curr = symbol_table;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0 && curr->scope_level == current_scope) {
            return 0; // Error: variable already declared
        }
        curr = curr->next;
    }

    // Insert new symbol at the head of the list
    Symbol *new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    new_sym->type = strdup(type);
    new_sym->scope_level = current_scope;
    new_sym->next = symbol_table;
    symbol_table = new_sym;
    
    return 1; // Success
}

// Looks up a variable to see if it was declared in any active scope
Symbol* lookup_symbol(char *name) {
    Symbol *curr = symbol_table;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            return curr; // Found it
        }
        curr = curr->next;
    }
    return NULL; // Not found
}