CFLAGS = -g -Wall
CC = gcc
ECHO = /bin/echo

llama: core/lexer.o core/main.c
	$(CC) $(CFLAGS) -o $@ $^ -lfl

core/lexer.c: core/lexer.l
	flex -s --header-file=core/lexer.h -o $@ $<

test: core/lexer.o tests/main.c tests/helpers.c
	@$(ECHO) "* Compiling..."
	@$(CC) $(CFLAGS) -o llama-test $^ -lfl
	@./llama-test
	@$(RM) ./llama-test

.PHONY: test clean distclean

clean:
	$(RM) lexer.c *.o *~

distclean: clean
	$(RM) llama
