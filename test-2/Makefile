CXX = g++
INCLUDE =
CXXFLAGS = -Wall -Wextra -g -O2 -std=c++17 -fno-omit-frame-pointer -DDEBUG $(INCLUDE)
#CXXFLAGS = -mtune=native -march=native -Wall -Wextra -O2 -std=c++17 $(INCLUDE)

T2_COLOR1=$$(if [ "${T2_COLOR1}" != "" ]; then echo $$T2_COLOR1; else echo $$T2_COLOR0; fi) # if T2_COLOR1 is not set, set it to T2_COLOR0

CXXFLAGS+=                 \
   -DT2_COLOR0=$$T2_COLOR0 \
   -DT2_COLOR1=$$T2_COLOR1 \
   -DT2_NODES=$$T2_NODES

LN = $(CXX)
LIBS =
LFLAGS =

BINARY_PREFIX = test-2-
BINARY        = "${BINARY_PREFIX}R($$T2_COLOR0,$$T2_COLOR1)<=$$T2_NODES"

SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
INCLUDES = prereqs.hpp

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
	-rm -f $(BINARY_PREFIX)* *.o

.PHONY: clang-format
clang-format:
	@for file in $(SOURCES) $(INCLUDES); do \
		echo "clang-format-4.0 -i $$file"; \
		clang-format-4.0 -i $$file; \
	done

-include .depend
