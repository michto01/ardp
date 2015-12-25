NAME := entee
CC := clang
CFLAGS :=# -pedantic \\ 
	 -fPIC \
	 -std=c99 \
	 -O3 \
	 -march=native \
	 -g \
	 -Wall -Werror -Wextra -Winline \
	 -Wno-unused-variable -Wno-unused-parameter

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  EXT = so 
  LDFLAGS := -shared -Wl,-soname,lib$(NAME).$(EXT)
endif
ifeq ($(UNAME_S),Darwin)
  EXT = dylib
  LDFLAGS := -dynamiclib 
endif

SRCDIR = src
EXAMPLE_SOURCES = $(SRCDIR)/example.c
EXAMPLE_OBJECTS = $(EXAMPLE_SOURCES:%.c=%.o)

LIB_PARSERS := $(wildcard $(SRCDIR)/*.c.rl)
LIB_SOURCES := $(filter-out $(EXAMPLE_SOURCES), $(wildcard $(SRCDIR)/*.c) $(LIB_PARSERS:%.c.rl=%.c))
LIB_OBJECTS := $(LIB_SOURCES:%.c=%.o)

all: lib

lib: lib$(NAME).$(EXT)

lib$(NAME).$(EXT): $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.c: %.c.rl
	ragel -C -G2 -o $@ $<

example: lib$(NAME).$(EXT) $(EXAMPLE_OBJECTS)
	$(CC) $^ -L. -l$(NAME) -lz -lbz2 -Wl,-rpath,. -o $@

.PHONEY: clean
clean:
	@$(RM) $(EXAMPLE_OBJECTS) example $(LIB_OBJECTS) lib$(NAME).$(EXT)
