# OS type: Linux/Win DJGPP
ifdef OS
   EXE=.exe
else
   EXE=
endif

CFLAGS=-g -Wall
CC=gcc

llama$(EXE): lexer.o
	$(CC) $(CFLAGS) -o $@ $^ -lfl

lexer.c: lexer.l
	flex -s -o $@ $<

.PHONY: clean distclean

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) llama$(EXE)
