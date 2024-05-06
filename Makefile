.PHONY: compile run clean distclean
CXXFLAGS = -Wall -pedantic -std=c++20 -I./include -O3

# Further optimization.
# CXXFLAGS += -DNDEBUG

# Parallel computing.
ifneq ($(mkTbbLib),)
CXXFLAGS += -DPARALLEL_PACS
LDFLAGS += -L$(mkTbbLib)
LDLIBS += -ltbb
endif

EXEC = main
SOURCE = main.cpp
OBJECT = main.o
HEADERS = ./include/* # Recompiling purposes.
OUTPUT = ./output.txt

# Rules.

# Compiling only.
compile: $(EXEC)
	@echo "Done!"

# Compiling and running.
run: $(EXEC)
	@echo "Running ./$(EXEC) and redirecting the output to $(OUTPUT)"
	@./$(EXEC) > $(OUTPUT)
	@echo "Done!"

$(EXEC): $(OBJECT)
	@if [ "$(LDFLAGS) $(LDLIBS)" = " " ]; then echo "Linking $^ to $@"; else echo "Linking $^ to $@ with the following flags: $(LDFLAGS) $(LDLIBS)"; fi
	@$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJECT): $(SOURCE) $(HEADERS)
	@echo "Compiling $< using $(CXX) with the following flags: $(CXXFLAGS)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean.
clean:
	@echo "Cleaning the repo."
	@$(RM) $(OBJECT)
	@$(RM) $(OUTPUT)

distclean: clean
	@$(RM) $(EXEC)