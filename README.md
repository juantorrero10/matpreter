# 🧮 matpreter

**matpreter** is an interpreter — and future JIT compiler — for simple mathematical expressions.  
It’s mainly a learning project focused on **exploring how programming languages work under the hood**:  
tokenizing, parsing, building abstract syntax trees (ASTs), and eventually executing or compiling them.

---

## 🎯 Project Goals

- Build a tokenizer and parser for a small mathematical language  
- Support arithmetic operations (`+`, `-`, `*`, `/`, `^`, parentheses, variables, constants)  
- Represent parsed expressions as **ASTs** (Abstract Syntax Trees)  
- Evaluate expressions interactively or compile them into executable code (future goal)  
- Learn about **language design**, **compiler frontends**, and **runtime code generation (LLVM)**  

---


# Compile/run/launch gdb session/clean
´´´bash
make
make run
make debug
make clean
´´´
