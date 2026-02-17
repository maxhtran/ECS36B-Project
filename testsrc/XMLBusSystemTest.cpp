#include <gtest/gtest.h>
#include "XMLBusSystem.h"
#include "StringDataSource.h"

TEST(XMLBusSystemTest, SimpleTest) {
    // Beginning of professor's code from 2/6/2026 discussion (Test stops)
    auto BusRouteSource = std::make_shared<CStringDataSource>("<bussystem>\n"
                                                                "<stops>\n"
                                                                    "<stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                    "<stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "</stops>\n"
                                                            "</bussystem>");
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths>\n"
                                                                "<path source =\"321\" destination=\"311\"/>\n"
                                                                    "<node id=\"321\"/>\n"
                                                                    "<node id=\"315\"/>\n"
                                                                    "<node id=\"311\"/>\n"
                                                                "</path>\n"
                                                            "</paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    ASSERT_EQ(BusSystem.StopCount(), 2);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_NE(BusSystem.StopByIndex(0), nullptr);
    EXPECT_NE(BusSystem.StopByIndex(1), nullptr);
    
    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->ID(), 1);
    EXPECT_EQ(StopObj->NodeID(), 321);
    EXPECT_EQ(StopObj->Description(), "First");

    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->ID(), 2);
    EXPECT_EQ(StopObj->NodeID(), 311);
    EXPECT_EQ(StopObj->Description(), "second");
    // End of professor's code from 2/6/2026 discussion

    // Test routes (no routes here, see RouteTest)
    EXPECT_EQ(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("placeholder"), nullptr);

    // Test paths
    auto PathObj = BusSystem.PathByStopIDs(1, 2);
    ASSERT_NE(PathObj, nullptr);
    EXPECT_EQ(PathObj->StartNodeID(), 321);
    EXPECT_EQ(PathObj->EndNodeID(), 311);
    EXPECT_EQ(PathObj->NodeCount(), 3);
    EXPECT_EQ(PathObj->GetNodeID(0), 321);
    EXPECT_EQ(PathObj->GetNodeID(1), 315);
    EXPECT_EQ(PathObj->GetNodeID(2), 311);
    EXPECT_EQ(PathObj->GetNodeID(3), CStreetMap::InvalidNodeID);
}

TEST(XMLBusSystemTest, RouteTest) {
    auto BusRouteSource = std::make_shared<CStringDataSource>("<bussystem>\n"
                                                                "<routes>\n"
                                                                    "<route name=\"M\"\n>"
                                                                        "<routestop stop=\"22272\" />\n"
                                                                        "<routestop stop=\"22014\" />\n"
                                                                        "<routestop stop=\"22002\" />\n"
                                                                    "</route>\n"
                                                                "</routes>\n"
                                                            "</bussystem>");
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    // path reader is just a placeholder for the CXMLBusSystem
    auto BusPathSource = std::make_shared<CStringDataSource>("<paths>\n"
                                                                "<path source=\"321\" destination=\"311\"/>\n"
                                                                    "<node id=\"321\"/>\n"
                                                                    "<node id=\"315\"/>\n"
                                                                    "<node id=\"311\"/>\n"
                                                                "</path>\n"
                                                            "</paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    ASSERT_EQ(BusSystem.RouteCount(), 1);
    EXPECT_NE(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(1), nullptr);

    auto RouteObj = BusSystem.RouteByName("M");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->Name(), "M");
    EXPECT_EQ(RouteObj->StopCount(), 3);
    EXPECT_EQ(RouteObj->GetStopID(0), 22272);
    EXPECT_EQ(RouteObj->GetStopID(1), 22014);
    EXPECT_EQ(RouteObj->GetStopID(2), 22002);
    EXPECT_EQ(RouteObj->GetStopID(3), CBusSystem::InvalidStopID);
}

TEST(XMLBusSystemTest, InvalidXMLTest) {
    // Test what happens if error is at the beginning (shouldn't read anything)
    auto BusRouteSource = std::make_shared<CStringDataSource>("<bussystem\n"
                                                                "<stops>\n"
                                                                    "<stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                    "<stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                                "</stops>\n"
                                                            "</bussystem>");
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths>\n"
                                                                "<path source=\"321\" destination=\"311\"/>\n"
                                                                    "<node id=\"321\"/>\n"
                                                                    "<node id=\"315\"/>\n"
                                                                    "<node id=\"311\"/>\n"
                                                            "</paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.StopByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.StopByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.StopByID(1), nullptr);
    EXPECT_EQ(BusSystem.StopByID(2), nullptr);

    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_EQ(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("placeholder1"), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("placeholder2"), nullptr);

    EXPECT_EQ(BusSystem.PathByStopIDs(1, 2), nullptr);
    EXPECT_EQ(BusSystem.PathByStopIDs(2, 3), nullptr);

    // Test what happens if error is at the end (should read everything successfully prior to error)
    BusRouteSource = std::make_shared<CStringDataSource>("<bussystem>\n"
                                                                "<stops>\n"
                                                                    "<stop id=\"1\" node=\"321\" description=\"First\"/>\n"
                                                                    "<stop id=\"2\" node=\"311\" description=\"second\"/>\n"
                                                            "</bussystem>");
    BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    EXPECT_EQ(BusSystem.StopCount(), 2);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_NE(BusSystem.StopByIndex(0), nullptr);
    EXPECT_NE(BusSystem.StopByIndex(1), nullptr);
    
    auto StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->ID(), 1);
    EXPECT_EQ(StopObj->NodeID(), 321);
    EXPECT_EQ(StopObj->Description(), "First");

    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj, nullptr);
    EXPECT_EQ(StopObj->ID(), 2);
    EXPECT_EQ(StopObj->NodeID(), 311);
    EXPECT_EQ(StopObj->Description(), "second");

    auto PathObj = BusSystem.PathByStopIDs(1, 2);
    ASSERT_NE(PathObj, nullptr);
    EXPECT_EQ(PathObj->StartNodeID(), 321);
    EXPECT_EQ(PathObj->EndNodeID(), 311);
    EXPECT_EQ(PathObj->NodeCount(), 3);
    EXPECT_EQ(PathObj->GetNodeID(0), 321);
    EXPECT_EQ(PathObj->GetNodeID(1), 315);
    EXPECT_EQ(PathObj->GetNodeID(2), 311);
    EXPECT_EQ(PathObj->GetNodeID(3), CStreetMap::InvalidNodeID);
}

TEST(XMLBusSystemTest, ZeroTest) {
    auto BusRouteSource = std::make_shared<CStringDataSource>("<bussystem>\n"
                                                                "<stops>\n"
                                                                "</stops>\n"
                                                                "<routes>\n"
                                                                "</routes>\n"
                                                            "</bussystem>");
    auto BusRouteReader = std::make_shared<CXMLReader>(BusRouteSource);

    auto BusPathSource = std::make_shared<CStringDataSource>("<paths>\n"
                                                            "</paths>");
    auto BusPathReader = std::make_shared<CXMLReader>(BusPathSource);

    CXMLBusSystem BusSystem(BusRouteReader, BusPathReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.StopByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.StopByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.StopByID(1), nullptr);
    EXPECT_EQ(BusSystem.StopByID(2), nullptr);

    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_EQ(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(1), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("placeholder1"), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("placeholder2"), nullptr);

    EXPECT_EQ(BusSystem.PathByStopIDs(1, 2), nullptr);
    EXPECT_EQ(BusSystem.PathByStopIDs(2, 3), nullptr);
}