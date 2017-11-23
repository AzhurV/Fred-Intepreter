#
# Created by gmakemake (Ubuntu Jul 25 2014) on Wed Nov 22 20:54:04 2017
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

CFLAGS = -std=c99 -ggdb -Wall -Wextra -pedantic
CLIBFLAGS = -lm

########## End of flags from header.mak


CPP_FILES =	
C_FILES =	evaluate.c fred.c processor.c stack.c symbolTable.c
PS_FILES =	
S_FILES =	
H_FILES =	evaluate.h processor.h stack.h symbolTable.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	evaluate.o processor.o stack.o symbolTable.o 

#
# Main targets
#

all:	fred 

fred:	fred.o $(OBJFILES)
	$(CC) $(CFLAGS) -o fred fred.o $(OBJFILES) $(CLIBFLAGS)

#
# Dependencies
#

evaluate.o:	evaluate.h stack.h symbolTable.h
fred.o:	evaluate.h processor.h stack.h symbolTable.h
processor.o:	evaluate.h processor.h stack.h symbolTable.h
stack.o:	stack.h
symbolTable.o:	symbolTable.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) fred.o core

realclean:        clean
	-/bin/rm -f fred 
