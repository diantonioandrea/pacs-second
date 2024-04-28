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
	@echo "Done!"

run: $(EXEC)
	@echo "Running the script and redirecting the output to $(OUTPUT)"
	@./$(EXEC) > $(OUTPUT)
	@echo "Done!"

$(EXEC): $(OBJECT)
	@if ["$(LDLIBS)" = ""]; then echo "Linking $^ to $@"; else echo "Linking $^ to $@ with the following flags: $(LDFLAGS) $(LDLIBS)"; fi
	@$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJECT): $(SOURCE) $(HEADERS)
	@echo "Compiling $< with the following flags: $(CXXFLAGS)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean.
clean:
	@echo "Cleaning the repo."
	@$(RM) $(OBJECT)
	@$(RM) $(OUTPUT)

distclean: clean
	@$(RM) $(EXEC)