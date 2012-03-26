# OS type: Linux/Win DJGPP
ifdef OS
   EXE=.exe
else
   EXE=
endif

CFLAGS=-g -Wall
CC=gcc

llama$(EXE): lexer.o main.c
	$(CC) $(CFLAGS) -o $@ $^ -lfl

lexer.c: lexer.l
	flex -s -o $@ $<

llama-test$(EXE): lexer.o tests/main.c
	$(CC) $(CFLAGS) -o $@ $^ -lfl

test$(EXE): llama-test
	./llama-test$(EXE)

.PHONY: clean distclean

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) llama$(EXE)
