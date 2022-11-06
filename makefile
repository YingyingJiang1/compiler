objs := main.o syntax.tab.o  ast/ast.o  sematic/sematic.o sematic/st.o
	
parser: $(objs)	
	gcc  -g $(objs) -lfl -ly -o parser
	make clean


ast.o:
	cd ./ast
	gcc -c -g ast.c

sematic.o:
	cd sematic
	gcc -c -g sematic.cd

st.o:
	cd sematic
	gcc -c -g st.c

syntax.tab.c :
	flex lexical.l
	bison -d syntax.y


syn.tab.o: syntax.tab.c 
	gcc -c -g syntax.tab.c 
main.o: 	
	gcc -c -g main.c

clean:
	find . -name "*.o"  | xargs rm -f