# Define the tools
AR=ar
CC=gcc
CXX=g++

# Define the directories
INC_DIR				= ./include
SRC_DIR				= ./src
TESTSRC_DIR			= ./testsrc
BIN_DIR				= ./bin
OBJ_DIR				= ./obj
LIB_DIR				= ./lib
TESTOBJ_DIR			= ./testobj
TESTBIN_DIR			= ./testbin
TESTCOVER_DIR		= ./htmlconv

# Define the flags for compilation/linking
DEFINES				=
INCLUDE				= -I $(INC_DIR)
ARFLAGS				= rcs
CFLAGS				= -Wall
CPPFLAGS			= --std=c++17
LDFLAGS				=

TEST_CFLAGS			= $(CFLAGS) -O0 -g --coverage
TEST_CPPFLAGS		= $(CPPFLAGS) -fno-inline
TEST_LDFLAGS		= $(LDFLAGS) -lgtest -lgtest_main -lpthread

# Define the object files
TEST_SVG_OBJ		= $(TESTOBJ_DIR)/svg.o
TEST_SVG_TEST_OBJ	= $(TESTOBJ_DIR)/SVGTest.o
TEST_OBJ_FILES		= $(TEST_SVG_OBJ) $(TEST_SVG_TEST_OBJ)

# Define the targets
TEST_TARGET			= $(TESTBIN_DIR)/testsvg


# Define the non-test object files
SVG_OBJ 			= $(OBJ_DIR)/svg.o 
MAIN_OBJ 			= $(OBJ_DIR)/main.o 
OBJ_FILES 			= $(SVG_OBJ) $(MAIN_OBJ)

# Define the targets
EXECUTABLE 			= $(BIN_DIR)/svg
CHECKMARK_ANSWER	= expected_checkmark.svg
CHECKMARK_OUTPUT	= checkmark.svg

all: directories runtests run runcheckmark

runtests: $(TEST_TARGET)
	$(TEST_TARGET)
	lcov --capture --directory . --output-file $(TESTCOVER_DIR)/coverage.info --ignore-errors inconsistent,source
	lcov --remove $(TESTCOVER_DIR)/coverage.info '/usr/*' '*/testsrc/*' --output-file $(TESTCOVER_DIR)/coverage.info
	genhtml $(TESTCOVER_DIR)/coverage.info --output-directory $(TESTCOVER_DIR)

$(TEST_TARGET): $(TEST_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_TARGET)

$(TEST_SVG_OBJ): $(SRC_DIR)/svg.c
	$(CC) $(TEST_CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(TEST_SVG_OBJ)

$(TEST_SVG_TEST_OBJ): $(TESTSRC_DIR)/SVGTest.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $(TESTSRC_DIR)/SVGTest.cpp -o $(TEST_SVG_TEST_OBJ)


run: $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(LIB_DIR)/libsvg.a
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIB_DIR)/libsvg.a -o $(EXECUTABLE)

$(SVG_OBJ): $(SRC_DIR)/svg.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(SVG_OBJ)

$(MAIN_OBJ): $(SRC_DIR)/main.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC_DIR)/main.c -o $(MAIN_OBJ)

$(LIB_DIR)/libsvg.a: $(OBJ_DIR)/svg.o
	$(AR) $(ARFLAGS) $(LIB_DIR)/libsvg.a $(OBJ_DIR)/svg.o


runcheckmark: $(EXECUTABLE)
	$(EXECUTABLE) 
	@xmldiff --check $(CHECKMARK_OUTPUT) $(CHECKMARK_ANSWER) > /dev/null 2>&1 && echo "SVGs are identical ✅" || (echo "SVGs differ ❌"; exit 1)

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(TESTOBJ_DIR)
	mkdir -p $(TESTBIN_DIR)
	mkdir -p $(TESTCOVER_DIR)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(TESTOBJ_DIR)
	rm -rf $(TESTBIN_DIR)
	rm -rf $(TESTCOVER_DIR)