#include <gtest/gtest.h>
#include <OpenStreetMap.h>
#include "StringDataSource.h"

TEST(OpenStreetMapTest, SimpleTest) {
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                        "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                        "<node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                            "<way id=\"1000\">\n"
                                                                "<nd ref=\"1\"/>\n"
                                                                "<nd ref=\"2\"/>\n"
                                                            "</way>\n"
                                                        "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.NodeCount(), 2); // Two nodes should be loaded
    auto Node1 = OpenStreetMap.NodeByIndex(0); // Lookup by index preserves order
    ASSERT_NE(Node1, nullptr);
    EXPECT_EQ(Node1->ID(), 1);
    auto Location1 = CStreetMap::SLocation{38.5, -121.7};
    EXPECT_EQ(Node1->Location(), Location1);

    auto Node2 = OpenStreetMap.NodeByID(2); // Lookup by ID via hashmap
    ASSERT_NE(Node2, nullptr);
    EXPECT_EQ(Node2->ID(), 2);
    auto Location2 = CStreetMap::SLocation{38.5, -121.8};
    EXPECT_EQ(Node2->Location(), Location2);

    ASSERT_EQ(OpenStreetMap.WayCount(), 1); // One way parsed
    auto Way = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(Way, nullptr);
    EXPECT_EQ(Way->ID(), 1000);
    ASSERT_EQ(Way->NodeCount(), 2); // Way references two nodes
    EXPECT_EQ(Way->GetNodeID(0), 1);
    EXPECT_EQ(Way->GetNodeID(1), 2);
}

TEST(OpenStreetMapTest, LookupTest) {
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                        "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
	                                                        "<node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                            "<node id=\"3\" lat=\"38.5\" lon=\"-121.9\"/>\n"
                                                            "<node id=\"4\" lat=\"38.4\" lon=\"-121.1\"/>\n"
                                                            "<way id=\"100\">\n"
                                                                "<nd ref=\"1\"/>\n"
                                                                "<nd ref=\"2\"/>\n"
                                                                "<nd ref=\"4\"/>\n"
                                                            "</way>\n"
                                                            "<way id=\"101\">\n"
                                                                "<nd ref=\"1\"/>\n"
                                                                "<nd ref=\"3\"/>\n"
                                                            "</way>\n"
                                                        "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    // Test node lookup
    ASSERT_EQ(OpenStreetMap.NodeCount(), 4); // All nodes parsed
    auto Node1 = OpenStreetMap.NodeByID(4); // Direct ID lookup
    ASSERT_NE(Node1, nullptr);
    EXPECT_EQ(Node1->ID(), 4);
    auto Location1 = CStreetMap::SLocation{38.4, -121.1};
    EXPECT_EQ(Node1->Location(), Location1);

    auto Node2 = OpenStreetMap.NodeByIndex(1); // Ordered access
    ASSERT_NE(Node2, nullptr);
    EXPECT_EQ(Node2->ID(), 2);
    auto Location2 = CStreetMap::SLocation{38.5, -121.8};
    EXPECT_EQ(Node2->Location(), Location2);

    auto BadNode1 = OpenStreetMap.NodeByID(1000); // Nonexistent ID
    EXPECT_EQ(BadNode1, nullptr);
    auto BadNode2 = OpenStreetMap.NodeByIndex(OpenStreetMap.NodeCount()); // Out of range
    EXPECT_EQ(BadNode2, nullptr);

    // Test way lookup
    ASSERT_EQ(OpenStreetMap.WayCount(), 2);
    auto Way1 = OpenStreetMap.WayByID(100);
    ASSERT_NE(Way1, nullptr);
    EXPECT_EQ(Way1->ID(), 100);
    ASSERT_EQ(Way1->NodeCount(), 3);
    EXPECT_EQ(Way1->GetNodeID(0), 1);
    EXPECT_EQ(Way1->GetNodeID(1), 2);
    EXPECT_EQ(Way1->GetNodeID(2), 4);
    EXPECT_EQ(Way1->GetNodeID(3), CStreetMap::InvalidNodeID); // Bounds check

    auto Way2 = OpenStreetMap.WayByIndex(1);
    ASSERT_NE(Way2, nullptr);
    EXPECT_EQ(Way2->ID(), 101);
    ASSERT_EQ(Way2->NodeCount(), 2);
    EXPECT_EQ(Way2->GetNodeID(0), 1);
    EXPECT_EQ(Way2->GetNodeID(1), 3);
    EXPECT_EQ(Way2->GetNodeID(2), CStreetMap::InvalidNodeID);

    auto BadWay1 = OpenStreetMap.WayByID(1000); // Nonexistent ID
    EXPECT_EQ(BadWay1, nullptr);
    auto BadWay2 = OpenStreetMap.WayByIndex(OpenStreetMap.WayCount()); // Out of bounds
    EXPECT_EQ(BadWay2, nullptr);
}

TEST(OpenStreetMapTest, AttributeTest) {
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "<node id=\"10\" lat=\"38.55\" lon=\"-121.75\">\n"
                                                                "<tag k=\"amenity\" v=\"bus_stop\"/>\n"
                                                                "<tag k=\"name\" v=\"Shields Library\"/>\n"
                                                            "</node>\n"
                                                            "<node id=\"11\" lat=\"38.56\" lon=\"-121.76\"/>\n"
                                                            "<way id=\"200\">\n"
                                                                "<nd ref=\"10\"/>\n"
                                                                "<nd ref=\"11\"/>\n"
                                                                "<tag k=\"highway\" v=\"residential\"/>\n"
                                                                "<tag k=\"name\" v=\"Howard Way\"/>\n"
                                                            "</way>\n"
                                                            "<way id=\"201\">\n"
                                                                "<nd ref=\"10\"/>\n"
                                                                "<nd ref=\"11\"/>\n"
                                                            "</way>\n"
                                                        "</osm>\n");

    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);

    ASSERT_EQ(OpenStreetMap.NodeCount(), 2);
    auto Node1 = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(Node1, nullptr);
    EXPECT_EQ(Node1->AttributeCount(), 2); // Two <tag> children captured
    EXPECT_NE(Node1->GetAttributeKey(0), "");
    EXPECT_NE(Node1->GetAttribute(Node1->GetAttributeKey(0)), "");
    EXPECT_TRUE(Node1->HasAttribute("amenity"));
    EXPECT_EQ(Node1->GetAttribute("amenity"), "bus_stop");
    EXPECT_TRUE(Node1->HasAttribute("name"));
    EXPECT_EQ(Node1->GetAttribute("name"), "Shields Library");
    EXPECT_FALSE(Node1->HasAttribute("does_not_exist")); // Missing key
    EXPECT_EQ(Node1->GetAttribute("does_not_exist"), "");
    EXPECT_EQ(Node1->GetAttributeKey(Node1->AttributeCount()), ""); // Out of range

    auto Node2 = OpenStreetMap.NodeByIndex(1); // Self-closing node has no attributes
    ASSERT_NE(Node2, nullptr);
    EXPECT_EQ(Node2->AttributeCount(), 0);

    ASSERT_EQ(OpenStreetMap.WayCount(), 2);
    auto Way1 = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(Way1, nullptr);
    EXPECT_EQ(Way1->AttributeCount(), 2); // Way tags parsed same as node tags
    EXPECT_NE(Way1->GetAttributeKey(0), "");
    EXPECT_NE(Way1->GetAttribute(Way1->GetAttributeKey(0)), "");
    EXPECT_TRUE(Way1->HasAttribute("highway"));
    EXPECT_EQ(Way1->GetAttribute("highway"), "residential");
    EXPECT_TRUE(Way1->HasAttribute("name"));
    EXPECT_EQ(Way1->GetAttribute("name"), "Howard Way");
    EXPECT_FALSE(Way1->HasAttribute("does_not_exist"));
    EXPECT_EQ(Way1->GetAttribute("does_not_exist"), "");
    EXPECT_EQ(Way1->GetAttributeKey(Way1->AttributeCount()), "");

    auto Way2 = OpenStreetMap.WayByIndex(1); // Way without tags
    ASSERT_NE(Way2, nullptr);
    EXPECT_EQ(Way2->AttributeCount(), 0);
}

TEST(OpenStreetMapTest, InvalidXMLTest) {
    { // Missing osm root
    auto OSMSource = std::make_shared<CStringDataSource>("<notosm>\n"
                                                            "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                        "</notosm>"); 
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader); 
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1), nullptr);
    }
    { // Unclosed node
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                            "<node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                        "</osm>"); 
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1), nullptr);
    }
    { // Missing attributes
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "<node lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                        "</osm>"); 
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1), nullptr);
    }
    { // Duplicate node id
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                            "<node id=\"1\" lat=\"38.6\" lon=\"-121.8\"/>\n"
                                                        "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1), nullptr);
    }
    { // Duplicate way id
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
                                                            "<node id=\"1\" lat=\"38.5\" lon=\"-121.7\"/>\n"
                                                            "<node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                            "<way id=\"100\">\n"
                                                                "<nd ref=\"1\"/>\n"
                                                                "<nd ref=\"2\"/>\n"
                                                            "</way>\n"
                                                            "<way id=\"100\">\n"
                                                                "<nd ref=\"2\"/>\n"
                                                                "<nd ref=\"1\"/>\n"
                                                            "</way>\n"
                                                        "</osm>");
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(100), nullptr);
    }
}

TEST(OpenStreetMapTest, ZeroTest) {
    auto OSMSource = std::make_shared<CStringDataSource>("<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n" // Empty
                                                        "</osm>"); 
    auto OSMReader = std::make_shared<CXMLReader>(OSMSource);

    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
    EXPECT_EQ(OpenStreetMap.NodeByID(1), nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(0), nullptr);
    EXPECT_EQ(OpenStreetMap.WayByID(1), nullptr);
}