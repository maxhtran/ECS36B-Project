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
TESTTMP_DIR			= ./testtmp

# Define the flags for compilation/linking
PKGS				= expat
DEFINES				=
INCLUDE				= -I $(INC_DIR) `pkg-config --cflags $(PKGS)`
ARFLAGS				= rcs
CFLAGS				= -Wall
CPPFLAGS			= --std=c++20
LDFLAGS				= `pkg-config --libs $(PKGS)`

TEST_CFLAGS			= $(CFLAGS) -O0 -g --coverage
TEST_CPPFLAGS		= $(CPPFLAGS) -fno-inline
TEST_LDFLAGS		= $(LDFLAGS) -lgtest -lgtest_main -lpthread

# Define the object files
TEST_SVG_OBJ		= $(TESTOBJ_DIR)/svg.o
TEST_SVG_TEST_OBJ	= $(TESTOBJ_DIR)/SVGTest.o
TEST_OBJ_FILES		= $(TEST_SVG_OBJ) $(TEST_SVG_TEST_OBJ)

TEST_STRSINK_OBJ 		= $(TESTOBJ_DIR)/StringDataSink.o 
TEST_STRSINK_TEST_OBJ	= $(TESTOBJ_DIR)/StringDataSinkTest.o 
TEST_STRSINK_OBJ_FILES 	= $(TEST_STRSINK_OBJ) $(TEST_STRSINK_TEST_OBJ)

TEST_STRSRC_OBJ			= $(TESTOBJ_DIR)/StringDataSource.o 
TEST_STRSRC_TEST_OBJ	= $(TESTOBJ_DIR)/StringDataSourceTest.o
TEST_STRSRC_OBJ_FILES	= $(TEST_STRSRC_OBJ) $(TEST_STRSRC_TEST_OBJ)

TEST_SVGWRITER_OBJ			= $(TESTOBJ_DIR)/SVGWriter.o 
TEST_SVGWRITER_TEST_OBJ		= $(TESTOBJ_DIR)/SVGWriterTest.o
TEST_SVGWRITER_OBJ_FILES	= $(TEST_SVGWRITER_OBJ) $(TEST_SVGWRITER_TEST_OBJ) $(TEST_STRSINK_OBJ)

TEST_XML_OBJ		= $(TESTOBJ_DIR)/XMLReader.o 
TEST_XML_TEST_OBJ	= $(TESTOBJ_DIR)/XMLTest.o 
TEST_XML_OBJ_FILES	= $(TEST_XML_OBJ) $(TEST_XML_TEST_OBJ)

TEST_XMLBS_OBJ			= $(TESTOBJ_DIR)/XMLBusSystem.o 
TEST_XMLBS_TEST_OBJ		= $(TESTOBJ_DIR)/XMLBusSystemTest.o 
TEST_XMLBS_OBJ_FILES	= $(TEST_XMLBS_OBJ) $(TEST_XMLBS_TEST_OBJ) $(TEST_XML_OBJ) $(TEST_STRSRC_OBJ)

TEST_OSM_OBJ		= $(TESTOBJ_DIR)/OpenStreetMap.o 
TEST_OSM_TEST_OBJ	= $(TESTOBJ_DIR)/OpenStreetMapTest.o 
TEST_OSM_OBJ_FILES	= $(TEST_OSM_OBJ) $(TEST_OSM_TEST_OBJ) $(TEST_XML_OBJ) $(TEST_STRSRC_OBJ)

# Define the targets
TEST_SVG_TARGET			= $(TESTBIN_DIR)/testsvg
TEST_STRSINK_TARGET		= $(TESTBIN_DIR)/teststrdatasink
TEST_STRSRC_TARGET		= $(TESTBIN_DIR)/teststrdatasource
TEST_SVGWRITER_TARGET	= $(TESTBIN_DIR)/testsvgwriter
TEST_XML_TARGET			= $(TESTBIN_DIR)/testxml
TEST_XMLBS_TARGET		= $(TESTBIN_DIR)/testxmlbs
TEST_OSM_TARGET			= $(TESTBIN_DIR)/testosm


# Define the non-test object files
SVG_OBJ 			= $(OBJ_DIR)/svg.o 
MAIN_OBJ 			= $(OBJ_DIR)/main.o 
OBJ_FILES 			= $(SVG_OBJ) $(MAIN_OBJ)

# Define the targets
EXECUTABLE 			= $(BIN_DIR)/svg
CHECKMARK_ANSWER	= expected_checkmark.svg
CHECKMARK_OUTPUT	= checkmark.svg

all: directories run_svgtest run_srctest run_sinktest run_xmltest run_svgwritertest run_xmlbstest run_osmtest gen_html

run_svgtest: $(TEST_SVG_TARGET)
	$(TEST_SVG_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_sinktest: $(TEST_STRSINK_TARGET)
	$(TEST_STRSINK_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_srctest: $(TEST_STRSRC_TARGET)
	$(TEST_STRSRC_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_svgwritertest: $(TEST_SVGWRITER_TARGET)
	$(TEST_SVGWRITER_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_xmltest: $(TEST_XML_TARGET)
	$(TEST_XML_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_xmlbstest: $(TEST_XMLBS_TARGET)
	$(TEST_XMLBS_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_osmtest: $(TEST_OSM_TARGET)
	$(TEST_OSM_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@
	
gen_html:
	lcov --capture --directory . --output-file $(TESTCOVER_DIR)/coverage.info --ignore-errors inconsistent,source
	lcov --remove $(TESTCOVER_DIR)/coverage.info '*.h' '/usr/*' '*/testsrc/*' --output-file $(TESTCOVER_DIR)/coverage.info
	genhtml $(TESTCOVER_DIR)/coverage.info --output-directory $(TESTCOVER_DIR)

$(TEST_SVG_TARGET): $(TEST_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SVG_TARGET)

$(TEST_STRSINK_TARGET): $(TEST_STRSINK_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSINK_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSINK_TARGET)

$(TEST_STRSRC_TARGET): $(TEST_STRSRC_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_STRSRC_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_STRSRC_TARGET)

$(TEST_SVGWRITER_TARGET): $(LIB_DIR)/libsvg.a $(TEST_SVGWRITER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_SVGWRITER_OBJ_FILES) $(LIB_DIR)/libsvg.a $(TEST_LDFLAGS) -o $(TEST_SVGWRITER_TARGET)

$(TEST_XML_TARGET): $(TEST_XML_OBJ_FILES) $(TEST_STRSRC_OBJ)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_XML_OBJ_FILES) $(TEST_STRSRC_OBJ) $(TEST_LDFLAGS) -lexpat -o $(TEST_XML_TARGET)

$(TEST_XMLBS_TARGET): $(TEST_XMLBS_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_XMLBS_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_XMLBS_TARGET)

$(TEST_OSM_TARGET): $(TEST_OSM_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_OSM_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_OSM_TARGET)
	
$(TEST_SVG_OBJ): $(SRC_DIR)/svg.c
	$(CC) $(TEST_CFLAGS) $(DEFINES) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(TEST_SVG_OBJ)

$(TEST_SVG_TEST_OBJ): $(TESTSRC_DIR)/SVGTest.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $(TESTSRC_DIR)/SVGTest.cpp -o $(TEST_SVG_TEST_OBJ)

$(TESTOBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

$(TESTOBJ_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@


run: $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(LIB_DIR)/libsvg.a
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIB_DIR)/libsvg.a -o $(EXECUTABLE)

$(SVG_OBJ): $(SRC_DIR)/svg.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC_DIR)/svg.c -o $(SVG_OBJ)

$(MAIN_OBJ): $(SRC_DIR)/main.c 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC_DIR)/main.c -o $(MAIN_OBJ)

$(LIB_DIR)/libsvg.a: $(SVG_OBJ)
	$(AR) $(ARFLAGS) $(LIB_DIR)/libsvg.a $(SVG_OBJ)

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