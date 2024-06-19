# REFERENCE: https://stackoverflow.com/questions/52034997/how-to-make-makefile-recompile-when-a-header-file-is-changed

SRC_DIR = src

SROUCE := $(wildcard $(SRC_DIR)/*.c)
OBJECT := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
DEPEND := $(SROUCE:$(SRC_DIR)/%.c=$(SRC_DIR)/%.d)

TARGET = engine

LIBS = sdl2 SDL2_image SDL2_mixer SDL2_ttf libavformat libavcodec libavutil libswscale libswresample libcjson

# compile variables
CC = -gcc
CFLAGS = -Wall -Wextra -MMD -MP $(shell pkg-config --cflags $(LIBS))

# link variables
LDFLAGS = -Wall -Wextra
LDLIBS  = -lm $(shell pkg-config --libs $(LIBS))

# .PHONY means these rules get executed even if files of those names exist
.PHONY: all clean

all: $(TARGET)

# debug: CFLAGS += -g -DDEBUG
debug: CFLAGS += -g -DDEBUG -fsanitize=address
debug: LDFLAGS += -fsanitize=address -static-libasan
debug: $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

# engine: $(OBJECT)

-include $(DEPEND)

clean:
	rm -f $(OBJECT) $(DEPEND) engine
