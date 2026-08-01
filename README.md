# Mini-C Compiler (`.mc`)

A six-phase compiler front-end for a custom mini-language, built with **Flex**, **Bison**, and **C**.

Compiler Construction Lab Project — Department of CSE, Metropolitan University Bangladesh.

---

## Team

| Name | GitHub | Responsibilities |
|------|--------|------------------|
| Mosaddeq (Team Lead) | [@TheBiscuitMann](https://github.com/TheBiscuitMann) | Lexical analyzer, Makefile & build system, three-address code generation, web UI |
| Fariha Rahman | [@student880](https://github.com/student880) | Parser (grammar), AST construction, symbol table, semantic analyzer, project report |
| Puspo Gondha Paul | [puspogondhapaul-cloud](https://github.com/puspogondhapaul-cloud) | Test suite (valid & invalid programs, expected outputs) |

---

## Overview

The compiler reads a source file written in our mini-language (`.mc`) and processes it through six phases:

1. **Lexical Analysis** — Flex scanner converts the character stream into tokens, tracking line numbers and reporting invalid characters.
2. **Syntax Analysis** — Bison LALR(1) parser validates the token stream against the grammar and reports syntax errors with line numbers.
3. **Abstract Syntax Tree** — the parser's semantic actions build an AST; the tree is printed in indented form.
4. **Symbol Table** — scoped symbol table records identifiers, their types, and their scope level; supports nested block scoping.
5. **Semantic Analysis** — walks the AST checking for undeclared variables, redeclaration in the same scope, type mismatches, and invalid assignments.
6. **Intermediate Code Generation** — emits three-address code (TAC) with temporary variables and labels for control flow.

The compiler stops and reports errors at the phase where they are detected.

---

## The Language

**File extension:** `.mc`

**Data types:** `int`, `float`, `bool`

**Statements**
- Variable declaration — `int x;`
- Assignment — `x = 5;`
- Conditional — `if`, `if`/`else`
- Loop — `while`
- Output — `print`
- Nested blocks `{ ... }` with lexical scoping

**Operators**

| Category | Operators |
|----------|-----------|
| Arithmetic | `+` `-` `*` `/` `%` |
| Relational | `<` `>` `<=` `>=` `==` `!=` |
| Logical | `&&` `\|\|` `!` |

### Sample program

```c
int x;
int y;
x = 10;
y = 0;

while (x > 0) {
    y = y + x;
    x = x - 1;
}

if (y > 50) {
    print y;
} else {
    print x;
}
```

The formal context-free grammar, operator precedence table, and a note on the dangling-else conflict are documented in [`docs/GRAMMAR.md`](docs/GRAMMAR.md).

---

## Requirements

- GCC
- Flex
- Bison
- GNU Make
- Python 3 and Flask (only for the optional web UI)

On Ubuntu / WSL:

```bash
sudo apt update
sudo apt install build-essential flex bison
```

For the web UI:

```bash
pip3 install flask
```

---

## Build

From the project root:

```bash
make
```

This runs Bison, then Flex, then compiles all C sources into a single executable named `compiler`.

To remove generated files and the binary:

```bash
make clean
```

> **Note:** Bison reports `1 shift/reduce conflict`. This is the classic *dangling-else* ambiguity. Bison resolves it by shifting, which correctly binds each `else` to the nearest unmatched `if`. This is expected behaviour, not a defect — see `docs/GRAMMAR.md`.

---

## Running the compiler

The compiler reads the source program from standard input:

```bash
./compiler < tests/valid/complete_program.mc
```

Any `.mc` file can be supplied:

```bash
./compiler < examples/valid/sample_program.mc
./compiler < tests/invalid/type_mismatch.mc
```

The output shows each phase in sequence: the token stream, parse result, AST, symbol table, semantic analysis result, and the generated three-address code.

---

## Web Interface (bonus)

A browser-based front-end that runs the actual compiler binary and displays each phase in a tabbed view.

```bash
make                    # the binary must be built first
python3 web/server.py
```

Then open <http://127.0.0.1:5000> in a browser.

Type or paste a program into the editor and run it. The output panel separates AST, semantic analysis, TAC, and raw compiler output into tabs. The Flask backend invokes the real `./compiler` executable — nothing is simulated.

---

## Test Suite

```
tests/
├── valid/      programs that compile cleanly
└── invalid/    programs that must be rejected
```

Each test consists of a `.mc` source file and a `.out` file holding the expected compiler output.

**Valid tests:** arithmetic, assignment, declaration, if_else, while, complete_program

**Invalid tests:** lexical_error, syntax_error, undeclared_variable, redeclaration, type_mismatch, scope_violation, invalid_assignment

Run any test manually:

```bash
./compiler < tests/invalid/undeclared_variable.mc
```

---

## Project Structure

```
CC-Lab-Project-new_mutants/
├── docs/
│   ├── GRAMMAR.md                  formal grammar & precedence
│   └── Compiler Construction Lab Project Manual.pdf
├── src/
│   ├── lexer/lexer.l               Flex specification
│   ├── parser/parser.y             Bison grammar & AST actions
│   ├── ast/                        ast.c, ast.h
│   ├── symbol_table/               symtab.c, symtab.h
│   ├── semantic/                   semantic.c, semantic.h
│   └── codegen/                    tac.c, tac.h
├── tests/
│   ├── valid/
│   └── invalid/
├── examples/
│   ├── valid/
│   └── invalid/
├── web/
│   ├── server.py                   Flask backend
│   └── index.html                  browser front-end
├── Makefile
├── .gitignore
└── README.md
```

Generated files (`lex.yy.c`, `parser.tab.c`, `parser.tab.h`, `*.o`, `compiler`) are produced by the build and are not tracked in version control.

---

## Repository

```bash
git clone git@github.com:TheBiscuitMann/CC-Lab-Project-new_mutants.git
cd CC-Lab-Project-new_mutants
make
```

Forked from the course template at [KhalidBinSelim/Compiler-Construction-Lab-Project](https://github.com/KhalidBinSelim/Compiler-Construction-Lab-Project).