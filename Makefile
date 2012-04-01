CFLAGS = -g -Wall
CC = gcc
ECHO = /bin/echo

llama: lexer.o main.c
	$(CC) $(CFLAGS) -o $@ $^ -lfl

lexer.c: lexer.l
	flex -s -o $@ $<

test: lexer.o tests/main.c tests/helpers.c
	@$(ECHO) "* Compiling..."
	@$(CC) $(CFLAGS) -o llama-test $^ -lfl
	@./llama-test
	@$(RM) ./llama-test

.PHONY: test clean distclean

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) llama
