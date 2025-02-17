SRC_DIR=src
EXAMPLE_SRC_DIR=$(SRC_DIR)/examples
INCLUDE_SRC_DIR=$(SRC_DIR)/include

BUILD_DIR=build
TEST_OBJ_DIR=$(BUILD_DIR)/tests
EXAMPLE_OBJ_DIR=$(BUILD_DIR)/examples
INCLUDE_OBJ_DIR=$(BUILD_DIR)/include

BIN_DIR=bin
TEST_BIN_DIR=$(BIN_DIR)/tests
EXAMPLE_BIN_DIR=$(BIN_DIR)/examples
INCLUDE_DIR=include

DIRS=$(BIN_DIR) $(BUILD_DIR) $(INCLUDE_DIR)

CFLAGS_BASE=-std=gnu99
CFLAGS=$(CFLAGS_BASE)
SO_FLAGS=-fpic -shared
OPTIMIZE_FLAGS=-march=native -s -Os -pipe -flto=4 -D NDEBUG -fwhole-program -fno-stack-protector

NO_WARN_TEST_FLAGS=-Wno-unused-parameter
WARN_FLAGS=-Wall -Wextra -Werror -Wno-error=cpp -Wunused-result -Wvla -Wshadow -Wstrict-prototypes -Wsuggest-attribute=pure -Wsuggest-attribute=const
MEMORY_DEBUG_FLAGS=-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
SANTIIZER_FLAGS=-fsanitize=undefined -fsanitize-address-use-after-scope -fstack-check -fno-stack-clash-protection
DEBUG_FLAGS=$(WARN_FLAGS) $(MEMORY_DEBUG_FLAGS) $(SANITIZER_FLAGS) -Og -ggdb3

PROFILE_MEMORY_FLAGS=-fstack-usage -fcallgraph-info=su
LIBS_TESTS=$(shell pkg-config --cflags --libs cmocka)

EXAMPLES_SRC=$(wildcard $(SRC_DIR)/examples/*.c)
EXAMPLES_OBJ=$(patsubst $(EXAMPLE_SRC_DIR)/%.c, $(EXAMPLE_OBJ_DIR)/%.o, $(EXAMPLES_SRC))
EXAMPLES_BIN=$(patsubst $(EXAMPLE_SRC_DIR)/%.c, $(EXAMPLE_BIN_DIR)/%, $(EXAMPLES_SRC))

TESTS_SRC=$(wildcard $(SRC_DIR)/*.test.c $(SRC_DIR)/*/*.test.c)
TESTS_OBJ=$(patsubst %/,$(TEST_OBJ_DIR)/%.test,$(dir $(subst $(SRC_DIR)/,,$(TESTS_SRC))).o)
TESTS_BIN=$(patsubst %/,$(TEST_BIN_DIR)/%.test,$(dir $(subst $(SRC_DIR)/,,$(TESTS_SRC))))

LIB_SRC=$(filter-out $(EXAMPLES_SRC), $(filter-out $(TESTS_SRC), $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/**/*.c)))
LIB_OBJ=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRC))
LIB_DEP=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.d,$(LIB_SRC))

SO=$(INCLUDE_DIR)/libshttp.so

all: shared release

$(shell mkdir -p $(dir $(LIB_DEP)))
-include $(LIB_DEP)

.PHONY: all shared release debug profile_memory check clean clean_executables so examples
$(VERBOSE).SILENT:

release: CFLAGS += $(OPTIMIZE_FLAGS)
release: examples

debug: CFLAGS += $(DEBUG_FLAGS)
debug: examples

shared: CFLAGS += $(OPTIMIZE_FLAGS)
shared: so

profile_memory: clean_executables
profile_memory: CFLAGS = $(CFLAGS_BASE) $(PROFILE_MEMORY_FLAGS) $(OPTIMIZE_FLAGS)
profile_memory: examples
profile_memory: so

check: CFLAGS += $(LIBS_TESTS) $(NO_WARN_TEST_FLAGS) $(OPTIMIZE_FLAGS)
check: $(TESTS_BIN)

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR) $(INCLUDE_DIR)

clean_executables:
	rm -rf $(BIN_DIR) $(INCLUDE_DIR)

so: CFLAGS += $(SO_FLAGS)
so: $(SO)

examples: $(EXAMPLES_BIN)

$(SO): $(LIB_SRC) | $(INCLUDE_DIR)
	$(info CC  $@)
	$(CC) $(CFLAGS) -o $@ $^

$(EXAMPLE_BIN_DIR)/%: $(EXAMPLE_OBJ_DIR)/%.o $(LIB_OBJ) | $(EXAMPLE_BIN_DIR)
	$(info LNK $@)
	$(CC) $(CFLAGS) -MMD -MP -o $@ $^

$(EXAMPLE_OBJ_DIR)/%.o: $(EXAMPLE_SRC_DIR)/%.c | $(EXAMPLE_OBJ_DIR)
	$(info CC  $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $^

$(TEST_BIN_DIR)/%.test: $(SRC_DIR)/%/*.test.c $(SRC_DIR)/%/*.c | $(TEST_BIN_DIR)
	$(info CC  $@)
	$(CC) $(CFLAGS) -o $@ $<
	$(info RUN $@)
	./$@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(info CC  $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(dir $(LIB_OBJ)) $(dir $(LIB_DEP))

$(TEST_BIN_DIR):
	mkdir -p $(TEST_BIN_DIR)

$(EXAMPLE_BIN_DIR):
	mkdir -p $(EXAMPLE_BIN_DIR)

$(EXAMPLE_OBJ_DIR):
	mkdir -p $(EXAMPLE_OBJ_DIR)

$(INCLUDE_DIR):
	mkdir -p $(INCLUDE_DIR)
