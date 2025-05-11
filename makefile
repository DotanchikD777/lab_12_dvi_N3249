# Makefile для проекта C
# Собирает все .c из src/ в object/, а итоговый lab12dvi кладёт в корень (рядом с Makefile)

# Компилятор и флаги
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2

# Директории
SRC_DIR := src
OBJ_DIR := obj

# Исходники и объектники
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Имя исполняемого файла (в корне проекта)
TARGET  := lab12dviN3249

.PHONY: all clean dirs

all: dirs $(TARGET)

# Создать папку object/, если её нет
dirs:
	mkdir -p $(OBJ_DIR)

# Линковка
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Компиляция каждого .c в свой .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Удалить объектники и исполняемый
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
