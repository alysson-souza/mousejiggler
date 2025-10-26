# MouseJiggler C Rewrite - Makefile
# Supports MSVC and MinGW-w64 compilation

# Configuration
CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c99 -Os -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables
RCFLAGS ?=
LIBS := user32 gdi32 comctl32 shell32 advapi32
LDFLAGS := $(addprefix -l,$(LIBS)) -Wl,--gc-sections -s

# Directories
SRC_DIR := ../src
INCLUDE_DIR := ../include
RES_DIR := ../res
OBJ_DIR := ../obj
BIN_DIR := ../bin

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
TARGET := $(BIN_DIR)/mousejiggler.exe

# Targets
.PHONY: all clean

all: $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) -mwindows

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# MSVC targets (for Windows development)
msvc: CC := cl
msvc: CFLAGS := /W4 /O2 /std:c11 /D_UNICODE /DUNICODE
msvc: LDFLAGS := user32.lib gdi32.lib comctl32.lib shell32.lib advapi32.lib /SUBSYSTEM:WINDOWS
msvc: all

# Run the application
run: $(TARGET)
	$(TARGET)

# Help
help:
	@echo "MouseJiggler C Rewrite - Build System"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build the application (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  msvc    - Build with MSVC compiler"
	@echo "  run     - Build and run the application"
	@echo "  help    - Show this help message"
