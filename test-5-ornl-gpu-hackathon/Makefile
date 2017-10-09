CXX = g++
INCLUDE =
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++17 -fno-omit-frame-pointer -DDEBUG $(INCLUDE)
#CXXFLAGS = -mtune=native -Wall -Wextra -O2 -std=c++17 $(INCLUDE)

LN = $(CXX)
LIBS =
LFLAGS =

BINARY = test-5

SOURCES = check.cpp main.cpp prereqs.cpp
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = prereqs.hpp check.hpp config.hpp config.inc constexpr.hpp

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
	-rm -f $(BINARY) *.o .depend config.inc

.PHONY: clang-format
clang-format:
	@for file in $(SOURCES) $(INCLUDES); do \
		echo "clang-format -i $$file"; \
		clang-format -i $$file; \
	done

-include .depend

