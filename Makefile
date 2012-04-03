CFLAGS = -g -Wall
CC = gcc
ECHO = /bin/echo

llama: core/lexer.o core/main.c core/parser.o
	$(CC) $(CFLAGS) -o $@ $^ -lfl

core/parser.o core/parser.h: core/parser.c

core/parser.c: core/parser.y
	bison -d -v -o $@ $<

core/lexer.o: core/lexer.c

core/lexer.c: core/lexer.l core/parser.h
	flex -s --header-file=core/lexer.h -o $@ $<

test: core/lexer.o tests/main.c tests/helpers.c
	@$(ECHO) "* Compiling..."
	@$(CC) $(CFLAGS) -o llama-test $^ -lfl
	@./llama-test
	@$(RM) ./llama-test

.PHONY: test clean distclean

clean:
	$(RM) core/lexer.c core/*.o test/*.o
	$(RM) core/lexer.c core/lexer.h
	$(RM) core/parser.c core/parser.h core/parser.output

distclean: clean
	$(RM) llama
