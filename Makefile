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
TEST_LDFLAGS		= $(LDFLAGS) -lgtest -lgmock -lgtest_main -lpthread

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

TEST_MOCK_BS_OBJ	= $(TESTOBJ_DIR)/MockBusSystem.o 
TEST_MOCK_SM_OBJ	= $(TESTOBJ_DIR)/MockStreetMap.o

TEST_FILEDATASINK_OBJ		= $(TESTOBJ_DIR)/FileDataSink.o 
TEST_FILEDATASOURCE_OBJ		= $(TESTOBJ_DIR)/FileDataSource.o 
TEST_FILEDATAFACTORY_OBJ	= $(TESTOBJ_DIR)/FileDataFactory.o
TEST_FILEDATASS_TEST_OBJ	= $(TESTOBJ_DIR)/FileDataSSTest.o 
TEST_FILEDATASS_OBJ_FILES	= $(TEST_FILEDATASINK_OBJ) $(TEST_FILEDATASOURCE_OBJ) $(TEST_FILEDATAFACTORY_OBJ) $(TEST_FILEDATASS_TEST_OBJ)

TEST_BSINDEXER_OBJ			= $(TESTOBJ_DIR)/BusSystemIndexer.o
TEST_BSINDEXER_TEST_OBJ		= $(TESTOBJ_DIR)/BusSystemIndexerTest.o
TEST_BSINDEXER_OBJ_FILES	= $(TEST_BSINDEXER_OBJ) $(TEST_BSINDEXER_TEST_OBJ) $(TEST_MOCK_BS_OBJ) $(TEST_STRSRC_OBJ)

TEST_GEOUTILS_OBJ		= $(TESTOBJ_DIR)/GeographicUtils.o 
TEST_GEOUTILS_TEST_OBJ	= $(TESTOBJ_DIR)/GeographicUtilsTest.o 
TEST_GEOUTILS_OBJ_FILES	= $(TEST_GEOUTILS_OBJ) $(TEST_GEOUTILS_TEST_OBJ)

TEST_SMINDEXER_OBJ			= $(TESTOBJ_DIR)/StreetMapIndexer.o 
TEST_SMINDEXER_TEST_OBJ		= $(TESTOBJ_DIR)/StreetMapIndexerTest.o 
TEST_SMINDEXER_OBJ_FILES	= $(TEST_SMINDEXER_OBJ) $(TEST_SMINDEXER_TEST_OBJ) $(TEST_MOCK_SM_OBJ)

TEST_TP_OBJ			= $(TESTOBJ_DIR)/TripPlanner.o 
TEST_TP_TEST_OBJ 	= $(TESTOBJ_DIR)/TripPlannerTest.o 
TEST_TP_OBJ_FILES 	= $(TEST_TP_OBJ) $(TEST_TP_TEST_OBJ) $(TEST_MOCK_BS_OBJ) $(TEST_BSINDEXER_OBJ)

TEST_TEXTTPW_OBJ		= $(TESTOBJ_DIR)/TextTripPlanWriter.o 
TEST_TEXTTPW_TEST_OBJ	= $(TESTOBJ_DIR)/TextTripPlanWriterTest.o 
TEST_TEXTTPW_OBJ_FILES	= $(TEST_TEXTTPW_OBJ) $(TEST_TEXTTPW_TEST_OBJ) $(TEST_MOCK_BS_OBJ) $(TEST_STRSINK_OBJ)

TEST_SVGTPW_OBJ			= $(TESTOBJ_DIR)/SVGTripPlanWriter.o 
TEST_SVGTPW_TEST_OBJ	= $(TESTOBJ_DIR)/SVGTripPlanWriterTest.o 
TEST_SVGTPW_OBJ_FILES	= $(TEST_SVGTPW_OBJ) $(TEST_SVGTPW_TEST_OBJ) $(TEST_STRSINK_OBJ) $(TEST_XML_OBJ) $(TEST_MOCK_SM_OBJ) \
							$(TEST_MOCK_BS_OBJ) $(TEST_STRSRC_OBJ) $(TEST_SVGWRITER_OBJ) $(TEST_SMINDEXER_OBJ) \
							$(TEST_BSINDEXER_OBJ) $(TEST_GEOUTILS_OBJ)

TEST_HTMLTPW_OBJ		= $(TESTOBJ_DIR)/HTMLTripPlanWriter.o 
TEST_HTMLTPW_TEST_OBJ	= $(TESTOBJ_DIR)/HTMLTripPlanWriterTest.o 
TEST_HTMLTPW_OBJ_FILES	= $(TEST_HTMLTPW_OBJ) $(TEST_HTMLTPW_TEST_OBJ) $(TEST_STRSINK_OBJ) $(TEST_XML_OBJ) $(TEST_MOCK_SM_OBJ) \
							$(TEST_MOCK_BS_OBJ) $(TEST_STRSRC_OBJ) $(TEST_SVGWRITER_OBJ) $(TEST_SVGTPW_OBJ) $(TEST_BSINDEXER_OBJ) \
							$(TEST_SMINDEXER_OBJ) $(TEST_GEOUTILS_OBJ) $(TEST_TEXTTPW_OBJ)

TEST_TPCL_OBJ		= $(TESTOBJ_DIR)/TripPlannerCommandLine.o
TEST_TPCL_TEST_OBJ	= $(TESTOBJ_DIR)/TripPlannerCommandLineTest.o
TEST_TPCL_OBJ_FILES	= $(TEST_STRSRC_OBJ) \
						$(TEST_STRSINK_OBJ) \
						$(TEST_MOCK_BS_OBJ) \
						$(TEST_MOCK_SM_OBJ) \
						$(TEST_BSINDEXER_OBJ) \
						$(TEST_HTMLTPW_OBJ) \
						$(TEST_SVGTPW_OBJ) \
						$(TEST_TEXTTPW_OBJ) \
						$(TEST_TP_OBJ) \
						$(TEST_TPCL_OBJ) \
						$(TEST_TPCL_TEST_OBJ) \
						$(TEST_SVGWRITER_OBJ) \
						$(TEST_SMINDEXER_OBJ) \
						$(TEST_GEOUTILS_OBJ) \
						$(TEST_SVG_OBJ)

# Define the targets
TEST_SVG_TARGET			= $(TESTBIN_DIR)/testsvg
TEST_STRSINK_TARGET		= $(TESTBIN_DIR)/teststrdatasink
TEST_STRSRC_TARGET		= $(TESTBIN_DIR)/teststrdatasource
TEST_SVGWRITER_TARGET	= $(TESTBIN_DIR)/testsvgwriter
TEST_XML_TARGET			= $(TESTBIN_DIR)/testxml
TEST_XMLBS_TARGET		= $(TESTBIN_DIR)/testxmlbs
TEST_OSM_TARGET			= $(TESTBIN_DIR)/testosm
TEST_FILEDATASS_TARGET	= $(TESTBIN_DIR)/testfiledatass
TEST_BSINDEXER_TARGET	= $(TESTBIN_DIR)/testbussystemindexer
TEST_GEOUTILS_TARGET	= $(TESTBIN_DIR)/testgeographicutils
TEST_SMINDEXER_TARGET	= $(TESTBIN_DIR)/teststreetmapindexer
TEST_TP_TARGET 			= $(TESTBIN_DIR)/testtripplanner
TEST_TEXTTPW_TARGET		= $(TESTBIN_DIR)/testtexttripplanwriter
TEST_SVGTPW_TARGET		= $(TESTBIN_DIR)/testsvgtripplanwriter
TEST_HTMLTPW_TARGET		= $(TESTBIN_DIR)/testhtmltripplanwriter
TEST_TPCL_TARGET		= $(TESTBIN_DIR)/testtpcl


# Define the non-test object files
SVG_OBJ 			= $(OBJ_DIR)/svg.o 
MAIN_OBJ 			= $(OBJ_DIR)/main.o 
OBJ_FILES 			= $(SVG_OBJ) $(MAIN_OBJ)

TP_STRSINK_OBJ			= $(OBJ_DIR)/StringDataSink.o 
TP_STRSRC_OBJ			= $(OBJ_DIR)/StringDataSource.o 
TP_SVGWRITER_OBJ		= $(OBJ_DIR)/SVGWriter.o 
TP_XML_OBJ				= $(OBJ_DIR)/XMLReader.o 
TP_XMLBS_OBJ			= $(OBJ_DIR)/XMLBusSystem.o 
TP_OSM_OBJ				= $(OBJ_DIR)/OpenStreetMap.o 
TP_STDDATASINK_OBJ		= $(OBJ_DIR)/StandardDataSink.o 
TP_STDDATASRC_OBJ		= $(OBJ_DIR)/StandardDataSource.o 
TP_STDERRDATASINK_OBJ	= $(OBJ_DIR)/StandardErrorDataSink.o
TP_FILEDATASINK_OBJ		= $(OBJ_DIR)/FileDataSink.o 
TP_FILEDATASRC_OBJ		= $(OBJ_DIR)/FileDataSource.o 
TP_FILEDATAFACTORY_OBJ	= $(OBJ_DIR)/FileDataFactory.o
TP_BSINDEXER_OBJ		= $(OBJ_DIR)/BusSystemIndexer.o 
TP_GEOUTILS_OBJ			= $(OBJ_DIR)/GeographicUtils.o 
TP_SMINDEXER_OBJ		= $(OBJ_DIR)/StreetMapIndexer.o 
TP_OBJ 					= $(OBJ_DIR)/TripPlanner.o 
TP_TEXTTPW_OBJ			= $(OBJ_DIR)/TextTripPlanWriter.o 
TP_SVGTPW_OBJ			= $(OBJ_DIR)/SVGTripPlanWriter.o 
TP_HTMLTPW_OBJ			= $(OBJ_DIR)/HTMLTripPlanWriter.o 
TP_TPCL_OBJ				= $(OBJ_DIR)/TripPlannerCommandLine.o 
TP_MAIN_OBJ				= $(OBJ_DIR)/tripplannermain.o 

TP_OBJ_FILES 			= $(TP_STRSINK_OBJ) \
							$(TP_STRSRC_OBJ) \
							$(TP_SVGWRITER_OBJ) \
							$(TP_XML_OBJ) \
							$(TP_XMLBS_OBJ) \
							$(TP_OSM_OBJ) \
							$(TP_STDDATASINK_OBJ) \
							$(TP_STDDATASRC_OBJ) \
							$(TP_STDERRDATASINK_OBJ) \
							$(TP_FILEDATASINK_OBJ) \
							$(TP_FILEDATASRC_OBJ) \
							$(TP_FILEDATAFACTORY_OBJ) \
							$(TP_BSINDEXER_OBJ) \
							$(TP_GEOUTILS_OBJ) \
							$(TP_SMINDEXER_OBJ) \
							$(TP_OBJ) \
							$(TP_TEXTTPW_OBJ) \
							$(TP_SVGTPW_OBJ) \
							$(TP_HTMLTPW_OBJ) \
							$(TP_TPCL_OBJ) \
							$(TP_MAIN_OBJ)

# Define the targets
EXECUTABLE 			= $(BIN_DIR)/svg
CHECKMARK_ANSWER	= expected_checkmark.svg
CHECKMARK_OUTPUT	= checkmark.svg

TP_EXECUTABLE		= $(BIN_DIR)/tripplanner

all: directories \
		run_svgtest \
		run_srctest \
		run_sinktest \
		run_xmltest \
		run_svgwritertest \
		run_xmlbstest \
		run_osmtest \
		run_filedatasstest \
		run_bsindexertest \
		run_geoutilstest \
		run_smindexertest \
		run_tptest \
		run_texttpwtest \
		run_svgtpwtest \
		run_htmltpwtest \
		run_tpcltest \
		run_tripplanner \
		gen_html

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

run_filedatasstest: $(TEST_FILEDATASS_TARGET)
	$(TEST_FILEDATASS_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_bsindexertest: $(TEST_BSINDEXER_TARGET)
	$(TEST_BSINDEXER_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_geoutilstest: $(TEST_GEOUTILS_TARGET)
	$(TEST_GEOUTILS_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_smindexertest: $(TEST_SMINDEXER_TARGET)
	$(TEST_SMINDEXER_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
	mv $(TESTTMP_DIR)/$@ $@

run_tptest: $(TEST_TP_TARGET)
	$(TEST_TP_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@ --gtest_filter=-TripPlanner.FindOneStopRouteTest
	mv $(TESTTMP_DIR)/$@ $@

run_texttpwtest: $(TEST_TEXTTPW_TARGET)
	$(TEST_TEXTTPW_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@ --gtest_filter=-TextTripPlanWriter.OneStopRoute:TextTripPlanWriter.VerboseOutput
	mv $(TESTTMP_DIR)/$@ $@

run_svgtpwtest: $(TEST_SVGTPW_TARGET)
	$(TEST_SVGTPW_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@ --gtest_filter=-SVGTripPlanWriter.OneStopRoute:SVGTripPlanWriter.UpdatedDescription
	mv $(TESTTMP_DIR)/$@ $@

run_htmltpwtest: $(TEST_HTMLTPW_TARGET)
	$(TEST_HTMLTPW_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@  --gtest_filter=-HTMLTripPlanWriter.OneStopRoute
	mv $(TESTTMP_DIR)/$@ $@

run_tpcltest: $(TEST_TPCL_TARGET)
	$(TEST_TPCL_TARGET) --gtest_output=xml:$(TESTTMP_DIR)/$@
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

$(TEST_FILEDATASS_TARGET): $(TEST_FILEDATASS_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_FILEDATASS_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_FILEDATASS_TARGET)

$(TEST_BSINDEXER_TARGET): $(TEST_BSINDEXER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_BSINDEXER_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_BSINDEXER_TARGET)

$(TEST_GEOUTILS_TARGET): $(TEST_GEOUTILS_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_GEOUTILS_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_GEOUTILS_TARGET)

$(TEST_SMINDEXER_TARGET): $(TEST_SMINDEXER_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_SMINDEXER_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_SMINDEXER_TARGET)

$(TEST_TP_TARGET): $(TEST_TP_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_TP_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_TP_TARGET)

$(TEST_TEXTTPW_TARGET): $(TEST_TEXTTPW_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_TEXTTPW_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_TEXTTPW_TARGET)

$(TEST_SVGTPW_TARGET): $(LIB_DIR)/libsvg.a $(TEST_SVGTPW_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_SVGTPW_OBJ_FILES) $(LIB_DIR)/libsvg.a $(TEST_LDFLAGS) -o $(TEST_SVGTPW_TARGET)

$(TEST_HTMLTPW_TARGET): $(LIB_DIR)/libsvg.a $(TEST_HTMLTPW_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_HTMLTPW_OBJ_FILES) $(LIB_DIR)/libsvg.a $(TEST_LDFLAGS) -o $(TEST_HTMLTPW_TARGET)

$(TEST_TPCL_TARGET): $(TEST_TPCL_OBJ_FILES)
	$(CXX) $(TEST_CFLAGS) $(TEST_CPPFLAGS) $(TEST_TPCL_OBJ_FILES) $(TEST_LDFLAGS) -o $(TEST_TPCL_TARGET)
	
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


run_tripplanner: $(TP_EXECUTABLE)
	$(TP_EXECUTABLE)

$(TP_EXECUTABLE): $(TP_OBJ_FILES) $(LIB_DIR)/libsvg.a 
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(TP_OBJ_FILES) $(LIB_DIR)/libsvg.a $(LDFLAGS) -o $(TP_EXECUTABLE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

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
	mkdir -p $(TESTTMP_DIR)

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(LIB_DIR)
	rm -rf $(TESTOBJ_DIR)
	rm -rf $(TESTBIN_DIR)
	rm -rf $(TESTCOVER_DIR)
	rm -rf $(TESTTMP_DIR)
	rm -f run_*