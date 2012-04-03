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
	$(RM) core/lexer.c core/*.o test/*.o

distclean: clean
	$(RM) llama
