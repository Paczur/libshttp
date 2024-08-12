BIN=bin
BUILD=build
TEST_DIR=bin/tests
DIRS=$(BIN) $(BUILD)
SRC=src

WARN=-Wall -Wextra -Wvla
NO_WARN_TESTS=-Wno-unused-parameter -Wno-incompatible-pointer-types -Wno-unused-but-set-parameter
NO_WARN_DEBUG=-Wno-unused-function
MEMORY_DEBUG=-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
DEBUG=$(NO_WARN_DEBUG) $(MEMORY_DEBUG) -Og -ggdb3  -fsanitize=undefined -fsanitize-address-use-after-scope -fstack-check -fno-stack-clash-protection
LIBS=$(shell pkg-config --cflags --libs cmocka)
RELEASE=-Os -s -pipe -flto=4 -fwhole-program -D NDEBUG
CFLAGS=$(WARN) -march=native -std=gnu99 $(LIBS)
TESTS=$(wildcard $(SRC)/*.test.c $(SRC)/**/*.test.c)
BIN_TESTS=$(patsubst $(SRC)/%.test.c, $(TEST_DIR)/%.test,$(TESTS))
SOURCES=$(filter-out $(TESTS), $(wildcard $(SRC)/*.c $(SRC)/**/*.c))
OBJECTS=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))
DEPENDS=$(patsubst $(SRC)/%.c,$(BUILD)/%.d,$(SOURCES))

export CCACHE_DIR := ccache
CC=ccache gcc

all: release

$(shell mkdir -p $(dir $(DEPENDS)))
-include $(DEPENDS)

.PHONY: all install uninstall release debug re re_clean clean
MAKEFLAGS := --jobs=$(shell nproc)
MAKEFLAGS += --output-sync=target
$(VERBOSE).SILENT:

release: CFLAGS += $(RELEASE)
release: binaries

debug: CFLAGS += $(DEBUG)
debug: tests binaries

check: tests
check:
	for bin in $(BIN_TESTS); do \
		./$$bin; \
	done \

clean:
	rm -rf $(BIN) $(BUILD) $(CCACHE_DIR)

binaries: $(BIN)/libshttp

tests: $(BIN_TESTS)

$(BIN)/libshttp: $(OBJECTS) | $(BIN)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(TEST_DIR)/%.test: $(SRC)/%.test.c $(SRC)/%.c | $(TEST_DIR)
	$(CC) $(CFLAGS) $(NO_WARN_TESTS) -o $@ $<
	./$@

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

$(BUILD):
	mkdir -p $(dir $(OBJECTS)) $(dir $(DEPENDS))

$(TEST_DIR):
	mkdir -p $(TEST_DIR)/$(subst $(SRC)/,,$(dir $(TESTS)))
