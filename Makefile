# REFERENCE: https://stackoverflow.com/questions/52034997/how-to-make-makefile-recompile-when-a-header-file-is-changed

SRC_DIR = src

SROUCE := $(wildcard $(SRC_DIR)/*.c)
OBJECT := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
DEPEND := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.d)\

TARGET = engine

# compile variables
CC = -gcc
CFLAGS = -Wall -Wextra -MMD -MP

# link variables
LDFLAGS = -Wall -Wextra
LDLIBS  = -lm -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lavcodec -lavformat -lavutil -lswscale -lswresample

# .PHONY means these rules get executed even if files of those names exist
.PHONY: all clean

all: $(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

# engine: $(OBJECT)

-include $(DEPEND)

clean:
	rm -f $(OBJECT) $(DEPEND) engine
