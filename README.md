### C-- Compiler

A compiler of C-- language, whose syntax is defined in **Appendix_A.pdf**.

Needs two arguments: 

+ arg1: src file.
+ arg2: output file(an assembly file).  



### File Structure And Function Descriptions

front-end   
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ lex-and-syn **（Completing lexical and syntax analysis using flex and bison.)**  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ ast **（Creating abstract syntax tree.)**  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ semantic **(Completing semantic analysis and generating ir during semantic analysis.)**  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ st **(Creating the symbol table and implement operations on the symbol table.)**  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ ir **(Defining the structure of ir and implementing the generation of ir.)**  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ optimizaton **(Completing ir optimization based on framework.)**  

back-end **(Generating assembly code based on ir.)**  

output  
&ensp;&ensp;&ensp;&ensp;|\_\_\_\_\_\_ outputCode.c **(Output ir.)**    



### Additional notes

+ All mandatory and optional requirements for the semantic analysis experiment have been completed in the semantic analysis phase.
+ ...
