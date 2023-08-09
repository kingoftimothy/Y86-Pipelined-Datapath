# Set up the compiler and flags
CC = gcc
CFLAGS = -g -Wall

# Phony targets are not the name of a file
.PHONY: clean depend

# Set up the suffix rules and relationship between .c and .o files
.SUFFIXES: .c

.c.o: 
	$(CC) $(CFLAGS) -c $<

.c:
	$(CC) $(CFLAGS) $< -o $@

# Set up file dependencies
SRCS = util.c parser.c isa.c instruction.c registers.c sim.c


# Replace .c with .o
OBJS = $(SRCS:.c=.o)

sim: $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) sim

# Figure out header file dependencies
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ -MF ./.depend;

# Include the rule in dependency file that was just created by .depend rule
include .depend

