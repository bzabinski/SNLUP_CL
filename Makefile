CC = gcc

SOURCEDIR = source
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = $(SOURCEDIR)/include

CFLAGS = -std=c11 -g -Wall -I$(INCLUDEDIR)

LIBS = -lm

TARGET = SNLUP

_SRC = actions.c facts.c groups.c  hypothesis.c io.c logic.c objects.c phrases.c respond.c rules.c scripts.c sentences.c snlupmain.c utilities.c
SRC = $(patsubst %,$(SOURCEDIR)/%,$(_SRC))

_DEPS = actions.h externs.h facts.h globals.h groups.h hypothesis.h io.h logic.h objects.h phrases.h respond.h rules.h scripts.h sentences.h snlup.h utilities.h
DEPS = $(patsubst %,$(INCLUDEDIR)/%,$(_DEPS))

_OBJ =  actions.o facts.o groups.o  hypothesis.o io.o logic.o objects.o phrases.o respond.o rules.o scripts.o sentences.o snlupmain.o utilities.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

default: $(BINDIR)/$(TARGET)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLUDEDIR)/*~ $(BINDIR)/$(TARGET) 
