#############################################################################
# Makefile for building: jcmess
#############################################################################

EXECUTABLE=jcmess
CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lasound  -ljack
SOURCES= \
	jcmess.cpp \
	jcmess_main.cpp





OBJECTS=$(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
        
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

jcmess.o: jcmess.cpp jcmess.h
	$(CC) -c $(CFLAGS) -o "$@" "$<"

jcmess_main.o: jcmess_main.cpp jcmess.h
	$(CC) -c $(CFLAGS) -o "$@" "$<"

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

