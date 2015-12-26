#
# ARDP - Another RDF document parser Makefile
#

NAME := ardp
CC := clang
CFLAGS := -pedantic \
	 				-fPIC \
	 				-std=c11 \
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

# define any directories containing header files other than /usr/include
#
INCLUDES := -I/usr/local/include/ardp -I$(PWD)/include

# Project directories
SRC_ROOT   = src
BUILD_ROOT = build



ARDP_SOURCES = $(SRC_ROOT)/ardp.c
ARDP_OBJECTS = $(ARDP_SOURCES:$(SRC_ROOT)/%.c=$(BUILD_ROOT)/%.o)

LIB_PARSERS := $(wildcard $(SRC_ROOT)/*.c.rl)
LIB_SOURCES := $(filter-out $(ARDP_SOURCES), $(wildcard $(SRC_ROOT)/*.c) $(LIB_PARSERS:%.c.rl=%.c))
LIB_OBJECTS := $(patsubst $(SRC_ROOT)/%.c,$(BUILD_ROOT)/%.o, $(LIB_SOURCES))

all: dir lib

dir:
	mkdir -p $(BUILD_ROOT)

lib: lib$(NAME).$(EXT)

$(LIB_OBJECTS): $(LIB_SOURCES)
	  @echo "working with:" $<
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

lib$(NAME).$(EXT): $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) $(INCLUDES) $^ -o $@
	mv lib$(NAME).$(EXT) $(BUILD_ROOT)/lib$(NAME).$(EXT)

%.c: %.c.rl
	ragel -C -G2 -o $@ $<

ardp: dir $(BUILD_ROOT)/lib$(NAME).$(EXT) $(ARDP_OBJECTS)
	$(CC) $(INCLUDES) $^ -L. -l$(NAME) -lz -lbz2 -Wl,-rpath,. -o $(BUILD_ROOT)/$@

.PHONEY: clean
clean:
	@$(RM) $(ARDP_OBJECTS) ardp $(LIB_OBJECTS) $(BUILD_ROOT)/lib$(NAME).$(EXT)
