CXX = g++
INCLUDE =
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++17 -fno-omit-frame-pointer -DDEBUG $(INCLUDE)
#CXXFLAGS = -mtune=native -march=native -Wall -Wextra -O2 -std=c++17 $(INCLUDE)

LN = $(CXX)
LIBS =
LFLAGS =

BINARY = test-3

SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = prereqs.hpp config.hpp

.PHONY: all
all: .depend $(BINARY)

.PHONY:
$(BINARY): $(OBJECTS)
	$(LN) $(CXXFLAGS) -o $@ $^ $(LFLAGS) $(LIBS)
	@echo ""

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

.depend: $(SOURCES)
	$(CXX) $(CXXFLAGS) -MM $^ > .depend

.PHONY: clean
clean:
	-rm -f $(BINARY) *.o

.PHONY: clang-format
clang-format:
	@for file in $(SOURCES) $(INCLUDES); do \
		echo "clang-format-4.0 -i $$file"; \
		clang-format-4.0 -i $$file; \
	done

-include .depend
