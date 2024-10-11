BIN=bin
BUILD=build
TESTB=bin/tests
DIRS=$(BIN) $(BUILD)
SRC=src
EX=$(BIN)/examples

NO_WARN=-Wno-packed-bitfield-compat
WARN=-Wall -Wextra -Werror -Wno-error=cpp -Wunused-result -Wvla -Wshadow -Wstrict-prototypes -Wsuggest-attribute=pure -Wsuggest-attribute=const $(NO_WARN)
NO_WARN_TESTS=-Wno-unused-parameter
MEMORY_DEBUG=-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
DEBUG=$(MEMORY_DEBUG) -Og -ggdb3  -fsanitize=undefined -fsanitize-address-use-after-scope -fstack-check -fno-stack-clash-protection
LIBS=$(shell pkg-config --cflags --libs cmocka)
RELEASE=-O3 -s -pipe -flto=4 -fwhole-program -D NDEBUG
CFLAGS=$(WARN) -march=native -std=gnu99 $(LIBS)

EXAMPLES=$(wildcard $(SRC)/examples/*.c)
BIN_EXAMPLES=$(patsubst $(SRC)/examples/%.c, $(EX)/%, $(EXAMPLES))
TESTS=$(wildcard $(SRC)/*.test.c $(SRC)/*/*.test.c)
BIN_TESTS=$(patsubst %/,$(TESTB)/%.test,$(dir $(subst $(SRC)/,,$(TESTS))))
SOURCES=$(filter-out $(EXAMPLES), $(filter-out $(TESTS), $(wildcard $(SRC)/*.c $(SRC)/**/*.c)))
OBJECTS=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))
DEPENDS=$(patsubst $(SRC)/%.c,$(BUILD)/%.d,$(SOURCES))

all: release

$(shell mkdir -p $(dir $(DEPENDS)))
-include $(DEPENDS)

.PHONY: all install uninstall release debug clean check tests
MAKEFLAGS := --jobs=$(shell nproc)
MAKEFLAGS += --output-sync=target
$(VERBOSE).SILENT:

release: CFLAGS += $(RELEASE)
release: examples

debug: CFLAGS += $(DEBUG)
debug: tests examples

check: tests
check:
	for bin in $(BIN_TESTS); do \
		./$$bin; \
	done \

clean:
	rm -rf $(BIN) $(BUILD) $(CCACHE_DIR)

examples: $(BIN_EXAMPLES)

tests: $(BIN_TESTS)

$(EX)/%:$(SRC)/examples/%.c $(OBJECTS) | $(EX)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(BIN)/libshttp: $(OBJECTS) | $(BIN)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(TESTB)/%.test: $(SRC)/%/*.test.c $(SRC)/%/*.c | $(TESTB)
	$(CC) $(CFLAGS) -MMD -MP $(NO_WARN_TESTS) -o $@ $<
	./$@

$(TESTB)/%.test: $(SRC)/%.test.c | $(TESTB)
	$(CC) $(CFLAGS) -MMD -MP $(NO_WARN_TESTS) -o $@ $<
	./$@

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

$(BUILD):
	mkdir -p $(dir $(OBJECTS)) $(dir $(DEPENDS))

$(TESTB):
	mkdir -p $(TESTB)

$(EX):
	mkdir -p $(EX)
