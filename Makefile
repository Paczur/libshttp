BIN=bin
BUILD=build
DIRS=$(BIN) $(BUILD)
SRC=src

WARN=-Wall -Wextra
MEMORY_DEBUG=-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract
DEBUG=$(MEMORY_DEBUG) -Og -ggdb3  -fsanitize=undefined \
			-fsanitize-address-use-after-scope -fstack-check -fno-stack-clash-protection
RELEASE=-O2 -s -pipe -flto=4 -fwhole-program -D NDEBUG
CFLAGS=$(WARN) -march=native -std=gnu99 $(LIBS)
SOURCES=$(wildcard $(SRC)/*.c $(SRC)/**/*.c)
OBJECTS=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SOURCES))
DEPENDS=$(patsubst $(SRC)/%.c,$(BUILD)/%.d,$(SOURCES))

all: release

$(shell mkdir -p $(dir $(DEPENDS)))
-include $(DEPENDS)

.PHONY: all install uninstall release debug re re_clean clean
$(VERBOSE).SILENT:

re: re_clean debug
re_clean:
	rm -rf $(BIN)/out

release: CFLAGS += $(RELEASE)
release: binaries

debug: CFLAGS += $(DEBUG)
debug: binaries

clean:
	rm -rf $(BIN) $(BUILD)

clean_all:
	rm -rf $(BIN) $(BUILD) $(CCACHE_DIR)

binaries: $(BIN)/libshttp

$(BIN)/libshttp: $(OBJECTS) | $(BIN)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD):
	mkdir -p $(dir $(OBJECTS)) $(dir $(DEPENDS))

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)
