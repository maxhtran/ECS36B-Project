#include <gtest/gtest.h>
#include "MockStreetMap.h"
#include "StreetMapIndexer.h"

#include <iostream>
using std::cout;
using std::endl;

class StreepMapIndexer : public ::testing::Test{
    protected:
        std::shared_ptr<CMockStreetMap> DStreetMap;

        void SetUp() override{
            DStreetMap = CMockStreetMap::CreateTestStreetMap();
        }

        void TearDown() override{
            
        }
};

TEST_F(StreepMapIndexer, SimpleTest){
    CStreetMapIndexer Indexer(DStreetMap);

    EXPECT_EQ(Indexer.NodeCount(),DStreetMap->NodeCount());
    EXPECT_EQ(Indexer.WayCount(),DStreetMap->WayCount());
    std::shared_ptr<CStreetMap::SNode> LastNode;
    for(size_t Index = 0; Index < Indexer.NodeCount(); Index++){
        auto Node = Indexer.SortedNodeByIndex(Index);
        if(Node && LastNode){
            EXPECT_LT(LastNode->ID(), Node->ID());
        }
        LastNode = Node;
    }
    EXPECT_EQ(Indexer.SortedNodeByIndex(Indexer.NodeCount()),nullptr);
    std::shared_ptr<CStreetMap::SWay> LastWay;
    for(size_t Index = 0; Index < Indexer.WayCount(); Index++){
        auto Way = Indexer.SortedWayByIndex(Index);
        if(Way && LastWay){
            EXPECT_LT(LastWay->ID(), Way->ID());
        }
        LastWay = Way;
    }
    EXPECT_EQ(Indexer.SortedWayByIndex(Indexer.WayCount()),nullptr);
    CStreetMap::SLocation LowerLeft{38.5 + (-0.25)/69.0, -121.7 +(-0.25)/54.2};
    CStreetMap::SLocation UpperRight{38.5 + (0.26)/69.0, -121.7 +(0.26)/54.2};
    auto Ways = Indexer.WaysInRange(LowerLeft,UpperRight);
    std::unordered_set<CStreetMap::TWayID> WayIDs;
    for(auto Way : Ways){
        WayIDs.insert(Way->ID());
    }
    // 1st, 2nd, A St., B St.
    for(CStreetMap::TWayID WayID : {105, 205, 2407, 3408}){
        EXPECT_TRUE(WayIDs.contains(WayID));
    }

    for(size_t NodeIndex = 0; NodeIndex < DStreetMap->NodeCount(); NodeIndex++){
        auto Node = DStreetMap->NodeByIndex(NodeIndex);
        std::unordered_set<CStreetMap::TWayID> ExpectedWayIDs;
        std::unordered_set<CStreetMap::TWayID> ActualWayIDs;

        for(size_t WayIndex = 0; WayIndex < DStreetMap->WayCount(); WayIndex++){
            auto Way = DStreetMap->WayByIndex(WayIndex);
            for(size_t WayNodeIndex = 0; WayNodeIndex < Way->NodeCount(); WayNodeIndex++){
                if(Way->GetNodeID(WayNodeIndex) == Node->ID()){
                    ExpectedWayIDs.insert(Way->ID());
                }
            }
        }

        auto WaysByNode = Indexer.WaysByNodeID(Node->ID());
        for(auto Way : WaysByNode){
            ActualWayIDs.insert(Way->ID());
        }

        EXPECT_EQ(ExpectedWayIDs, ActualWayIDs);
    }
}

