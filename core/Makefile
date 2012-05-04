.SUFFIXES:
.SUFFIXES: .l .y .c .o
.PHONY: clean distclean

# OS type: Linux/Win DJGPP
ifdef OS
   EXE=.exe
else
   EXE=
endif

# programs
CC=gcc
#CC=gcc-3.0
LEX=flex
YACC=bison

# options
CFLAGS=-Wall -ansi -g -DCHECK_ASSERT
LDFLAGS=-lfl
ifndef LFLAGS
   LFLAGS=-s
endif
ifndef YFLAGS
   YFLAGS=-dtv
endif

# files
CFILES   = error.c general.c symbol.c types.c ast.c pretty.c
HFILES   = error.h general.h symbol.h types.h ast.h pretty.h
OBJFILES = $(patsubst %.c,%.o,$(CFILES)) parser.o lexer.o main.o
TMPFILES = parser.h parser.c lexer.c parser.output
EXEFILES = llama$(EXE)

SRCFILES = $(HFILES) $(CFILES)

#
# Generic rules
#

%.c %.h: %.l
	$(LEX) $(LFLAGS) -o$@ $<

%.c %.h: %.y
	$(YACC) $(YFLAGS) -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c $<

#
# Specific rules and dependencies
#

all: $(EXEFILES)

llama$(EXE): $(OBJFILES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

lexer.o: lexer.c parser.h
parser.o: parser.c parser.h lexer.h ast.h

general.o  : general.c general.h error.h
error.o    : error.c general.h error.h
symbol.o   : symbol.c symbol.h general.h error.h types.h
types.o    : types.c types.h general.h error.h
ast.o      : ast.c ast.h general.h types.h
pretty.o   : pretty.c pretty.h general.h types.h ast.h
main.o     : main.c llama.h parser.h ast.h

clean:
	$(RM) $(OBJFILES) $(TMPFILES) *~

distclean: clean
	$(RM) $(EXEFILES)

count:
	wc -l -c Makefile $(SRCFILES)
