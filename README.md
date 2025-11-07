# 🧮 matpreter

**matpreter** is an interpreter — and future JIT compiler — for simple mathematical expressions.  
It’s mainly a learning project focused on **exploring how programming languages work under the hood**:  
tokenizing, parsing, building abstract syntax trees (ASTs), and eventually executing or compiling them.

---


## Compiling
**Dependencies**
- make:
- gcc
- _in the future:_ LLVM
---
**Building the binary**

	# Installing dependencies
	sudo apt install make gcc
---
	make		# Compile
	make run	# Compile/run
	make debug	# Compile/launch gdb session
	make clean  # Clean all object/binary files
