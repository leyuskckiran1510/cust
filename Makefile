CC = gcc
CFLAGS = -ggdb3 -Wall -Wextra -pedantic -I./src  -lpthread -fsanitize=address -O3

SRCDIR = src
INCDIR = include
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)

OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

EXECUTABLE = $(BUILDDIR)/exe.exe



all:$(EXECUTABLE)


$(EXECUTABLE):$(OBJS)
	$(CC)  $^ -o $@ $(CFLAGS)


$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC)  -c $< -o $@ $(CFLAGS)

run:
	./$(EXECUTABLE)

r:run
dg:
	gf2 $(EXECUTABLE)

clean:
	@rm -rf $(BUILDDIR)/*

d: clean all run

.PHONY: all clean d