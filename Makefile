PROG = queue
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
CPP = g++
CXXFLAGS = -std=c++11 -ggdb -Wall -Werror
LDLIBS= -lpmemobj -lpmem

all: $(PROG)

$(PROG): $(OBJ)
	$(CPP) $(CXXFLAGS) $^ $(LDLIBS) -o $@

.PHONY: clean

clean:
	rm -f $(PWD)/*.o *~ $(PROG)
