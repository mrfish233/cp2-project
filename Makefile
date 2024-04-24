# REFERENCE: https://stackoverflow.com/questions/52034997/how-to-make-makefile-recompile-when-a-header-file-is-changed

SRC_DIR = src
SDL_LOCAL_DIR = ./third-party/SDL2/bin/sdl2-config

SDL_CONFIG = $(SDL_LOCAL_DIR)

SROUCE := $(wildcard $(SRC_DIR)/*.c)
OBJECT := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
DEPEND := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.d)\

TARGET = engine

# compile variables
CC = -gcc
CFLAGS = -Wall -Wextra -MMD -MP -g

# link variables
LDFLAGS = -Wall -Wextra
LDLIBS  = -lm `$(SDL_CONFIG) --cflags --libs`

# .PHONY means these rules get executed even if files of those names exist
.PHONY: all clean

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

# engine: $(OBJECT)

-include $(DEPEND)

clean:
	rm -f $(OBJECT) $(DEPEND) engine
