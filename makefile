CC = gcc
SRCS = $(shell find ./ -name "*.c" | sed "s/.\/\(.*lex.yy.c\|optimization\/.*\|tests\/.*\)//g") 
HEADS = $(shell find ./ -name "*.h" | sed "s/.\/optimization\/.*//g")
OBJS = $(SRCS:%.c=%.o)
TARGET = parser

$(TARGET):$(OBJS)
	@$(CC) -lfl -ly $(OBJS) -o $(TARGET)
	@make clean

%.o: %.c $(HEADS)	
	@$(CC) -c -g $< -o $@
	
clean:
	@find . -name "*.o"  | xargs rm -f