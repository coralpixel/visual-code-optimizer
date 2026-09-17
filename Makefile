# ============================================================
#  Makefile -- Visual Code Optimizer
#
#  make          build the executable into ./vco
#  make run      build, then run on tests/tc01.txt
#  make clean    remove build artifacts
#
#  New modules need NO edit here: every .c file under src/ is
#  picked up automatically by the wildcard below.
# ============================================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g -Isrc
TARGET  := vco

SRCDIR  := src
OBJDIR  := build

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) tests/tc01.txt

clean:
	rm -rf $(OBJDIR) $(TARGET) out/*.dot out/*.png
