.PHONY: all clean
CXXFLAGS = -Wall -pedantic -std=c++20  -I./include

# Extras.
CPPFLAGS = # -I$(PACS_ROOT)/include
LDFLAGS = # -L$(PACS_ROOT)/lib

# Optimization.
# CXXFLAGS += -O2 -DNDEBUG

EXEC = main
SOURCE = main.cpp
OBJECT = main.o
# OBJECTS = 

# Rules.
all: $(EXEC)

$(EXEC): $(OBJECT) # $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $(EXEC)

$(OBJECT): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# $(OBJECTS): %.o: ./src/%.cpp
# 	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Clean.
clean:
	$(RM) *.o

distclean: clean
	$(RM) $(EXEC)