# Пути
INCLUDE_DIR     := src/include
CORE_DIR        := src/core
PLUGINS_DIR     := src/plugins
BUILD_DIR       := build
BUILD_CORE_DIR  := $(BUILD_DIR)/core
BUILD_PLUGIN_DIR:= $(BUILD_DIR)/plugins
LIB_DIR         := lib

# Имя исполняемого рядом с Makefile
TARGET := lab12dviN3249

# Флаги
CFLAGS  := -Wall -Wextra -I$(INCLUDE_DIR) -g -std=gnu2x
LDFLAGS := -ldl

# Автообнаружение исходников
CORE_SRCS    := $(wildcard $(CORE_DIR)/*.c)
CORE_OBJS    := $(patsubst $(CORE_DIR)/%.c,$(BUILD_CORE_DIR)/%.o,$(CORE_SRCS))

PLUGIN_SRCS  := $(wildcard $(PLUGINS_DIR)/*.c)
PLUGIN_NAMES := $(notdir $(basename $(PLUGIN_SRCS)))
PLUGIN_OBJS  := $(patsubst $(PLUGINS_DIR)/%.c,$(BUILD_PLUGIN_DIR)/%.o,$(PLUGIN_SRCS))
PLUGIN_LIBS  := $(patsubst %,$(LIB_DIR)/lib%.so,$(PLUGIN_NAMES))

.PHONY: all clean

all: $(TARGET) $(PLUGIN_LIBS)

# ------------------------
# 1) Сборка основного приложения
# ------------------------
$(TARGET): $(CORE_OBJS)
	gcc $^ -o $@ $(LDFLAGS)

$(BUILD_CORE_DIR)/%.o: $(CORE_DIR)/%.c | $(BUILD_CORE_DIR)
	gcc $(CFLAGS) -c $< -o $@

# ------------------------
# 2) Сборка плагинов
# ------------------------
# правило для каждой lib/lib<name>.so из build/plugins/<name>.o
$(LIB_DIR)/lib%.so: $(BUILD_PLUGIN_DIR)/%.o | $(LIB_DIR)
	gcc -shared -fPIC $< -o $@

$(BUILD_PLUGIN_DIR)/%.o: $(PLUGINS_DIR)/%.c | $(BUILD_PLUGIN_DIR)
	gcc $(CFLAGS) -fPIC -c $< -o $@

# ------------------------
# 3) Создание директорий
# ------------------------
$(BUILD_CORE_DIR) $(BUILD_PLUGIN_DIR) $(LIB_DIR):
	mkdir -p $@

# ------------------------
# 4) Очистка
# ------------------------
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR) $(TARGET)
