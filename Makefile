CFLAGS= -g -Wall -Wextra -I./src -coverage
LDFLAGS= -L./build -coverage
LDLIBS= -lm

LIB_SOURCES=$(wildcard ./src/*.c)
LIB_OBJECTS=$(patsubst %.c,%.o,$(LIB_SOURCES))
LIB=./build/libdsa.a

TEST_SOURCES=$(wildcard ./tests/*_tests.c)
TEST=$(patsubst %.c,%,$(TEST_SOURCES))

.PHONY: all clean build test doc

all: clean $(LIB) test

clean:
	rm -rf ./build ./bin ./**/*.o
	rm -rf `find ./tests/ -type f ! -name "*.*"`
	rm -rf `find . -type f -name "*.aux" -o -name "*.log" -o -name "*.gc*" -o -name "*.info" -o -name "log.txt"`
	rm -rf ./cov
	ctags -R .

build:
	mkdir -p ./build

$(LIB): build $(LIB_OBJECTS)
	ar rcs $@ $(LIB_OBJECTS)
	ranlib $@

$(TEST): $(LIB)

# Build, run the tests, and generate coverage info.
test: $(TEST)
	sh ./tests/runtests.sh
	mkdir -p cov
	lcov --quiet --capture --directory . --output-file ./cov/coverage.info
	genhtml ./cov/coverage.info --output-directory ./cov --quiet

# Build and run the bst comparisons.
bst: CFLAGS += -DNDEBUG
bst: $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./build/bst_comparisons ./programs/bst_comparisons.c $(LIB)
	./build/bst_comparisons

# Build and run the sort comparisons.
sort: CFLAGS += -DNDEBUG
sort: $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./build/sort_comparisons ./programs/sort_comparisons.c $(LIB)
	./build/sort_comparisons
