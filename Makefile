CFLAGS= -g -Wall -Wextra -I./src
LDFLAGS= -L./build
LDLIBS= -lm

LIB_SOURCES=$(wildcard ./src/*.c)
LIB_OBJECTS=$(patsubst %.c,%.o,$(LIB_SOURCES))
LIB=./build/libdsa.a

TEST_SOURCES=$(wildcard ./tests/*_tests.c)
TEST=$(patsubst %.c,%,$(TEST_SOURCES))

BIN_SOURCES=$(wildcard ./programs/*.c)
BIN=$(patsubst ./programs/%.c,./build/%,$(BIN_SOURCES))

.PHONY: all clean build test

all: clean $(LIB) test

clean:
	rm -rf ./build ./bin ./**/*.o
	rm -rf `find ./tests/ -type f ! -name "*.*"`
	ctags -R .

build:
	mkdir -p ./build

$(LIB): build $(LIB_OBJECTS)
	ar rcs $@ $(LIB_OBJECTS)
	ranlib $@

$(TEST): $(LIB)

test: $(TEST)
	sh ./tests/runtests.sh

$(BIN): $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(BIN_SOURCES) $(LIB)

bin: CFLAGS += -DNDEBUG
bin: $(BIN)
	./build/sort_comparisons
