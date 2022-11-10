CC = gcc
SRCS = $(shell find ./ -name "*.c" | sed "s/.\/lex.yy.c//g") 
HEADS = $(shell find ./ -name "*.h")
OBJS = $(SRCS:%.c=%.o)
TARGET = compiler

$(TARGET):$(OBJS)
	$(CC) -lfl -ly $(OBJS) -o $(TARGET)
	make clean

%.o: %.c $(HEADS)	
	$(CC) -c -g $< -o $@
	
clean:
	find . -name "*.o"  | xargs rm -f