# Visual Code Optimizer

DAG-Based Basic Block Optimization and Peephole Optimization for a C-like Language Subset.

Compiler Design Laboratory Project (BCSE307P) — Coral Jain, 24BCB0028

## Build

```bash
make
```

## Run

```bash
./vco tests/tc01.txt
./vco tests/tc02.txt
./vco tests/tc06.txt
```

The executable prints the source, token stream, AST, unoptimized TAC, CFG, DAG, optimized TAC, peephole output, interpreter verification, and instruction-count reduction. DOT files are written to `out/`.

## Pipeline

```text
Source
  -> Lexer
  -> Recursive-Descent Parser / AST
  -> Three-Address Code
  -> Basic Blocks / CFG
  -> Per-block DAG Optimization
  -> TAC Regeneration
  -> Peephole Optimization
  -> TAC Interpreter Verification
  -> Graphviz DOT
```

## Implemented modules

| Module | File | Purpose |
|---|---|---|
| Lexer | `lexer.c/h` | Tokenization and lexical diagnostics |
| Parser | `parser.c/h`, `ast.h` | Expressions, assignments, if/else, while, blocks |
| TAC | `tacgen.c/h`, `quad.h` | Quadruple generation and labels |
| CFG | `block.c/h`, `cfg.h` | Leader algorithm and CFG edges |
| DAG | `dag.c/h` | Value numbering, CSE and constant folding |
| Optimizer | `optimize.c/h` | DAG-based local optimization and dead-code elimination |
| Code generator | `codegen.c/h` | Linear optimized TAC |
| Peephole | `peephole.c/h` | Local rewrite rules to a fixpoint |
| Interpreter | `interp.c/h` | Before/after semantic verification |
| Visualizer | `viz.c/h` | CFG and DAG Graphviz DOT output |

## Graphviz

If Graphviz is installed:

```bash
dot -Tpng out/cfg_before.dot -o out/cfg_before.png
dot -Tpng out/dag_before.dot -o out/dag_before.png
```

## Included tests

`tc01` — common subexpression elimination; `tc02` — constant folding, identities and dead store; `tc06` — if/else CFG; `tc07` — while loop; `tc08` — stale DAG-node invalidation; `tc09` — division-by-zero guard; `tc10` — no-redundancy control; `tc14_bad` — lexical error; and additional parser/optimization tests.
