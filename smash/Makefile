# Makefile for the smash program
CC = gcc
CFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o jobs.o
RM = rm -f
	
# Creating the  executable
all: $(OBJS)
	$(CCLINK) -o smash $(OBJS)

# Creating the object files
commands.o: commands.c commands.h
smash.o: smash.c commands.h
signals.o: signals.c signals.h
jobs.o: jobs.c jobs.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash

