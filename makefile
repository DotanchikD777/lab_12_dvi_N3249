# Указываем компилятор и базовые флаги
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu2x -D_POSIX_C_SOURCE=200809L

# Определяем директории
SRCDIR = src
OBJDIR = obj

# Находим все исходники в папке src
SOURCES := $(wildcard $(SRCDIR)/*.c)

# Соответствующие объектные файлы
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# Имя результирующего исполняемого файла
TARGET = lab11dviN3249

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
