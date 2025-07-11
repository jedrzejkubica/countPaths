LDFLAGS = -lm
CFLAGS = -g -Wall -Wextra -std=c99
CC = gcc


## all objects to make
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

# no main() yet but we want to compile each source

all: $(OBJS)


# aliDnaLinear: aliDnaLinear.o $(OBJCOMMON) $(OBJSW)
#	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

# deps made thanks to -MMD above, just include them
-include $(patsubst %.c,%.d,$(wildcard *.c))

clean:
	rm -f *.o *.d *~
