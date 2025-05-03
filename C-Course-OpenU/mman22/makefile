CC = gcc
CFLAGS = -Wall -pedantic -ansi  -g 

SRCS = mainmat.c mymat.c function_handler.c
OBJS = $(SRCS:.c=.o)
EXEC = mainmat

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

