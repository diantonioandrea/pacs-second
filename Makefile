.PHONY: all clean distclean
CXXFLAGS = -Wall -pedantic -std=c++20 -I./include -O3

# Optimizations.
# CXXFLAGS += -DNDEBUG

# Parallel computing.
# CXXFLAGS += -DPARALLEL_PACS
# LDFLAGS += -L$(mkTbbLib)
# LDLIBS += -ltbb

EXEC = main
SOURCE = main.cpp
OBJECT = main.o
HEADERS = ./include/* # Recompiling purposes.
OUTPUT = ./output.txt

# Rules.
compile: $(EXEC)
run:
	@echo "Running the script and redirecting the output to $(OUTPUT)..."
	@./$(EXEC) > $(OUTPUT)
	@echo "Done!"

$(EXEC): $(OBJECT)
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJECT): $(SOURCE) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Clean.
clean:
	$(RM) $(OBJECT)
	$(RM) $(OUTPUT)

distclean: clean
	$(RM) $(EXEC)