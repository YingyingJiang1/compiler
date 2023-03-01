### C-- Compiler

A compiler of C-- language, whose syntax is defined in **Appendix_A.pdf**.

Needs two arguments: 

+ arg1: src file.
+ arg2: output file(an assembly file).



### File Structure And Function Description

front-end 
	|\_\_\_\_\_\_ lex-and-syn **（Use flex and bison to complete lexical and syntax analysis.)**
	
	|\_\_\_\_\_\_ ast **（Create abstract syntax tree.)**
	
	|\_\_\_\_\_\_ semantic **(Complete semantic analysis and generate ir during semantic analysis.)**
	
	|\_\_\_\_\_\_ st **(Create the symbol table and implement operations on the symbol table.)**
	
	|\_\_\_\_\_\_ ir **(Define the structure of ir and implement the generation of ir.)**
	
	|\_\_\_\_\_\_ optimizaton **(Complete ir optimization based on framework code.)**

back-end **(Generate assembly code based on ir.)**

output
	|\_\_\_\_\_\_ outputCode.c **(Output ir.)**



# Additional notes

+ All mandatory and optional requirements for the semantic analysis experiment have been completed in the semantic analysis phase.
+ ...
