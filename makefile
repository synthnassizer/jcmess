#############################################################################
# Makefile for building: jcmess
#############################################################################

EXECUTABLE=jcmess
#CC=g++
CXXFLAGS+=-Wall -Wextra -Wpedantic
LDFLAGS+=-lasound -ljack
SOURCES= \
	jcmess.cpp \
	jcmess_main.cpp





OBJECTS=$(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
        
$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(CXXFLAGS) $(LDFLAGS)

jcmess.o: jcmess.cpp jcmess.h
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

jcmess_main.o: jcmess_main.cpp jcmess.h
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

clean:
	$(RM) $(EXECUTABLE) $(OBJECTS)

