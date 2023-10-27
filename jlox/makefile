# Makefile for compiling the Crafting Interpreters project with IntelliJ IDEA-like directory structure

# Define source directory
SRC_DIR = src

# Define the output directory structure to match the IntelliJ IDEA structure
OUT_DIR = out/production/lox

# Find all .java files in the source directory
JAVA_FILES = $(wildcard $(SRC_DIR)/com/craftinginterpreters/lox/*.java) \
            $(wildcard $(SRC_DIR)/com/craftinginterpreters/lox/test/*.java) \
            $(SRC_DIR)/com/craftinginterpreters/tool/GenerateAst.java

# Define the classpath (if needed)
CLASSPATH =

# Define java interpreter
JAVA = java

# Define the Java compiler
JAVAC = javac

# Define the Java compiler flags
JAVAC_FLAGS = -d $(OUT_DIR) -cp $(CLASSPATH)

# Define the target to compile all .java files
all: $(OUT_DIR) $(JAVA_FILES)
	$(JAVAC) $(JAVAC_FLAGS) $^

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Define a clean target to remove the output directory
clean:
	rm -rf $(OUT_DIR)

# Define a target to run the Lox interpreter
run:
	java -cp $(OUT_DIR) com.craftinginterpreters.lox.Lox

# Define a target to generate the AST classes
generate-ast:
	java -cp $(OUT_DIR) com.craftinginterpreters.tool.GenerateAst $(SRC_DIR)/com/craftinginterpreters/lox

# Define a target to compile and run the Lox interpreter
run-lox: all run

# Define the main class for the Lox interpreter
MAIN_CLASS = com.craftinginterpreters.lox.Lox

# Define the target to run the Lox interpreter with arguments
run-with-args: all
	@$(JAVA) -cp $(OUT_DIR) $(MAIN_CLASS) $(ARGS)

.PHONY: all clean run generate-ast run-lox

