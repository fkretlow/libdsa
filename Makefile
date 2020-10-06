CFLAGS= -g -Wall -Wextra -I./src
LDFLAGS= -L./build
LDLIBS= -lsort

SOURCES=$(wildcard ./src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
TARGET=./build/libsort.a

TEST_SOURCES=$(wildcard ./tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SOURCES))

.PHONY: all clean build tests

all: clean $(TARGET) tests

clean:
	rm -rf ./build ./**/*.o `find ./tests/ -type f -name "*_tests"` ./tests/sort_comparisons

build:
	mkdir -p ./build

$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(TESTS): $(TARGET)

tests: $(TESTS)
	sh ./tests/runtests.sh

compare: clean $(TARGET) ./tests/sort_comparisons
	./tests/sort_comparisons
