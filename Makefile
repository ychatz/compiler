ECHO=/bin/echo

all:
	cd core && $(MAKE)

test: core/ast.o core/lexer.o core/parser.o core/general.o core/types.o core/error.o tests/main.c tests/helpers.c tests/lexer.c tests/parser.c tests/semantic.c
	@cd core && $(MAKE)
	@$(ECHO) "* Compiling..."
	@$(CC) $(CFLAGS) -o llama-test $^ -lfl
	@./llama-test
	@$(RM) ./llama-test
