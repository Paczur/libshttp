BIN_DIR=bin
BUILD_DIR=build
TEST_DIR=$(BIN_DIR)/tests
SRC_DIR=src
EXAMPLE_DIR=$(BIN_DIR)/examples
INCLUDE_DIR=include
DIRS=$(BIN) $(BUILD) $(INCLUDE_DIR)
SO=$(INCLUDE_DIR)/libshttp.so

CFLAGS_BASE=-std=gnu99
CFLAGS=$(CFLAGS_BASE)
SO_FLAGS=-fpic -shared
OPTIMIZE_FLAGS=-march=native -O2 -s -pipe -flto=4 -D NDEBUG
OPTIMIZE_BIN_FLAGS=$(OPTIMIZE_FLAGS) -fwhole-program

NO_WARN_TEST_FLAGS=-Wno-unused-parameter
WARN_FLAGS=-Wall -Wextra -Werror -Wno-error=cpp -Wunused-result -Wvla -Wshadow -Wstrict-prototypes -Wsuggest-attribute=pure -Wsuggest-attribute=const
MEMORY_DEBUG_FLAGS=-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
SANTIIZER_FLAGS=-fsanitize=undefined -fsanitize-address-use-after-scope -fstack-check -fno-stack-clash-protection
DEBUG_FLAGS=$(WARN_FLAGS) $(MEMORY_DEBUG_FLAGS) $(SANITIZER_FLAGS) -Og -ggdb3

PROFILE_MEMORY_FLAGS=-fstack-usage
LIBS_TESTS=$(shell pkg-config --cflags --libs cmocka)

EXAMPLES_SRC=$(wildcard $(SRC_DIR)/examples/*.c)
EXAMPLES_BIN=$(patsubst $(SRC_DIR)/examples/%.c, $(EXAMPLE_DIR)/%, $(EXAMPLES_SRC))

TESTS_SRC=$(wildcard $(SRC_DIR)/*.test.c $(SRC_DIR)/*/*.test.c)
TESTS_BIN=$(patsubst %/,$(TEST_DIR)/%.test,$(dir $(subst $(SRC_DIR)/,,$(TESTS_SRC))))

LIB_SRC=$(filter-out $(EXAMPLES_SRC), $(filter-out $(TESTS_SRC), $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/**/*.c)))
LIB_OBJ=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRC))
LIB_DEP=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.d,$(LIB_SRC))

all: shared release

$(shell mkdir -p $(dir $(LIB_DEP)))
-include $(LIB_DEP)

.PHONY: all shared release debug profile_memory check clean clean_executables so examples tests
$(VERBOSE).SILENT:

release: CFLAGS += $(OPTIMIZE_BIN_FLAGS)
release: examples

debug: CFLAGS += $(DEBUG_FLAGS)
debug: tests examples

shared: CFLAGS += $(OPTIMIZE_FLAGS)
shared: so

profile_memory: clean_executables
profile_memory: CFLAGS += $(PROFILE_MEMORY_FLAGS) $(OPTIMIZE_BIN_FLAGS)
profile_memory: examples
profile_memory: CFLAGS = $(CFLAGS_BASE) $(PROFILE_MEMORY_FLAGS) $(OPTIMIZE_FLAGS)
profile_memory: so

check: tests
check:
	for bin in $(BIN_TESTS); do \
		./$$bin; \
	done \

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR) $(INCLUDE_DIR)

clean_executables:
	rm -rf $(BIN_DIR) $(INCLUDE_DIR)

so: CFLAGS += $(SO_FLAGS)
so: $(SO)

examples: $(EXAMPLES_BIN)

tests: CFLAGS += $(LIBS_TESTS) $(NO_WARN_TEST_FLAGS)
tests: $(TESTS_BIN)

$(SO): $(LIB_SRC) | $(INCLUDE_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(EXAMPLE_DIR)/%:$(SRC_DIR)/examples/%.c $(LIB_OBJ) | $(EXAMPLE_DIR)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(BIN_DIR)/libshttp: $(LIB_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(TEST_DIR)/%.test: $(SRC_DIR)/%/*.test.c $(SRC_DIR)/%/*.c | $(TEST_DIR)
	$(info $(CC) $(CFLAGS) -o $@ $<)
	$(CC) $(CFLAGS) -o $@ $<
	./$@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(dir $(LIB_OBJ)) $(dir $(LIB_DEP))

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

$(EXAMPLE_DIR):
	mkdir -p $(EXAMPLE_DIR)

$(INCLUDE_DIR):
	mkdir -p $(INCLUDE_DIR)
