# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -ggdb3 -O0 -Qunused-arguments -std=c99 -Wall -Werror 

# name for executable
EXE = cmyk

# space-separated list of header files
HDRS = bmp.h lcms2.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS = -L/usr/local/lib -llcms2

# space-separated list of source files
SRCS = cmyk.c #rgbtocmyk.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies 
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
