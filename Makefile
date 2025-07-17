LDFLAGS = -lm
CFLAGS = -g -Wall -Wextra -std=c17
CC = gcc

OBJDIR = Objs
DEPDIR = Deps

## all objects to make
OBJS = $(patsubst %.h,$(OBJDIR)/%.o,$(wildcard *.h))

# binaries
all: mkdirs testAdjacency countPaths


testAdjacency: $(OBJDIR)/testAdjacency.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

countPaths: $(OBJDIR)/countPaths.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# make subdirs if they don't exist yet
mkdirs:
	mkdir -p $(OBJDIR) $(DEPDIR)

# making each object file
$(OBJDIR)/%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MF $(DEPDIR)/$*.d -c -o $@ $<

# deps made thanks to -MMD above, just include them
-include $(patsubst %.c,$(DEPDIR)/%.d,$(wildcard *.c))


clean:
	rm -f $(OBJDIR)/*.o $(DEPDIR)/*.d *~

.PHONY: mkdirs all clean
