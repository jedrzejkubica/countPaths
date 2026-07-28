LDFLAGS = -lm -lz -fopenmp
CFLAGS = -g -Wall -Wextra -std=c17 -O2 -fPIC -fvisibility=hidden -fopenmp
CC = gcc

OBJDIR = Objs
DEPDIR = Deps

## all shared objects to make
OBJS = $(patsubst %.h,$(OBJDIR)/%.o,$(wildcard *.h))

# default target for production: binaries and shared lib
default: $(OBJDIR) $(DEPDIR) allBins

# for development: like "default" but also build the LSP compilation database
all: $(OBJDIR) $(DEPDIR) compile_commands.json allBins

# all binaries
allBins: testAdjacency bfwalk.so

testAdjacency: $(OBJDIR)/testAdjacency.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

bfwalk.so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^


# make subdirs if they don't exist yet
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(DEPDIR):
	mkdir -p $(DEPDIR)


# use bear to make a compilation database for LSP (usable by emacs and other IDEs)
# (install bear with "sudo dnf install bear" on RHEL/Alma/Fedora if you don't have it)
compile_commands.json:
	bear -- make allBins


# making each object file
$(OBJDIR)/%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MF $(DEPDIR)/$*.d -c -o $@ $<

# deps made thanks to -MMD above, just include them
-include $(patsubst %.c,$(DEPDIR)/%.d,$(wildcard *.c))


clean:
	rm -f $(OBJDIR)/*.o $(DEPDIR)/*.d compile_commands.json testAdjacency bfwalk.so

.PHONY: default all allBins clean
