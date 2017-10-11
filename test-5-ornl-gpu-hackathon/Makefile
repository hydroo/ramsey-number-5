CXX = g++
INCLUDE =
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -g -fno-omit-frame-pointer $(INCLUDE)
#CXXFLAGS = -std=c++17 -mtune=native -mcpu=native -O2 -Wall -Wextra -g -fno-omit-frame-pointer $(INCLUDE)
CXXFLAGS += -DDEBUG
#CXXFLAGS += -DVERBOSE
#CXXFLAGS += -pg

LN = $(CXX)
LIBS =
LFLAGS =
#LFLAGS = -pg

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

