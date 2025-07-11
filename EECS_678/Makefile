STUDENTID = 3050817
PROGNAME = quash

CC = gcc --std=gnu11
CFLAGS = -Wall -g


####################################################################
#                           IMPORTANT                              #
####################################################################
# Add files to their respective line to get this makefile to build #
# them.                                                            #
####################################################################
# NOTE: The submission scripts assume all files in `CFILELIST` end with
# .c and all files in `HFILES` end in .h
CFILELIST = quash.c commands.c execute.c parsing/memory_pool.c parsing/parsing_interface.c parsing/parse.tab.c parsing/lex.yy.c
HFILELIST = quash.h commands.h execute.h parsing/memory_pool.h parsing/parsing_interface.h parsing/parse.tab.h deque.h 

# Add libraries that need linked as needed (e.g. -lm -lpthread)
LIBLIST =

# Include locations
INCLIST = ./src ./src/parsing

# Doxygen configuration file
DOXYGENCONF = quash.doxygen

####################################################################

SRCDIR = ./src/
OBJDIR = ./obj/

EXECNAME = $(patsubst %,./%,$(PROGNAME))

CFILES = $(patsubst %,$(SRCDIR)%,$(CFILELIST))
HFILES = $(patsubst %,$(SRCDIR)%,$(HFILELIST))
OFILES = $(patsubst %.c,$(OBJDIR)%.o,$(CFILELIST))

RAWC = $(patsubst %.c,%,$(addprefix $(SRCDIR), $(CFILELIST)))
RAWH = $(patsubst %.h,%,$(addprefix $(SRCDIR), $(HFILELIST)))

INCDIRS = $(patsubst %,-I%,$(INCLIST))

OBJINNERDIRS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(shell find $(SRCDIR) -type d))
SUBMISSIONDIRS = $(addprefix $(STUDENTID)-project1-quash/,$(shell find $(SRCDIR) -type d))

# Build the the quash executable
all: $(OBJINNERDIRS) $(PROGNAME)

debug: CFLAGS += -DDEBUG -gdwarf-2
debug: all

# Build the object directories
$(OBJINNERDIRS):
	$(foreach dir, $(OBJINNERDIRS), mkdir -p $(dir);)

# Build the quash program
$(PROGNAME): $(OFILES)
	$(CC) $(CFLAGS) $^ -o $(PROGNAME) $(LIBLIST)

# Generic build target for all compilation units. NOTE: Changing a
# header requires you to rebuild the entire project
$(OBJDIR)%.o: $(SRCDIR)%.c $(HFILES)
	$(CC) $(CFLAGS) -c $(INCDIRS) -o $@ $< $(LIBS)

%lex.yy.c: %parse.l
	lex -o $@ $<

%.tab.c %.tab.h: %.y
	bison -t --verbose --defines=$(dir $@)parse.tab.h -o $(dir $@)parse.tab.c $<

# Build and run the program
test: all
	./run_tests.bash -p


# Remove all generated files and directories
clean:
	-rm -rf $(PROGNAME) obj sandbox *~ $(STUDENTID)-project1-quash* src/parsing/parse.output valgrind_report.txt output_report.txt

deep-clean: clean
	-rm -rf doc src/parsing/parse.tab.c src/parsing/parse.tab.h src/parsing/lex.yy.c

%.c: %.y
%.c: %.l

.PHONY: all debug test submit unsubmit testsubmit doc clean deep-clean