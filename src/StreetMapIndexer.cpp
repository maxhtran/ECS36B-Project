#include "StreetMapIndexer.h"

struct CStreetMapIndexer::SImplementation {
    std::shared_ptr<CStreetMap> DStreetMap; // Pointer to the street map being indexed
    std::vector<std::shared_ptr<CStreetMap::SNode>> DSortedNodes; // All nodes sorted in ascending order by node ID
    std::vector<std::shared_ptr<CStreetMap::SWay>> DSortedWays; // All ways sorted in ascending order by way ID
    std::unordered_map<CStreetMap::TNodeID, std::shared_ptr<CStreetMap::SNode>> DNodeByID; // Maps each node ID to its corresponding node pointer
    std::unordered_map<CStreetMap::TNodeID, std::unordered_set<std::shared_ptr<CStreetMap::SWay>>> DWaysByNode; // Maps each node ID to the set of ways that contain that node

    bool NodeInRange(std::shared_ptr<CStreetMap::SNode> node, const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept{ // Checks if node is within rectangle
        CStreetMap::SLocation loc = {node->Location().DLatitude, node->Location().DLongitude};
        return (loc.DLatitude >= lowerleft.DLatitude && loc.DLatitude <= upperright.DLatitude
                && loc.DLongitude >= lowerleft.DLongitude && loc.DLongitude <= upperright.DLongitude);
    }

    SImplementation(std::shared_ptr<CStreetMap> streetmap) {
        DStreetMap = streetmap;
        auto CompareByID = [](const auto &A, const auto &B){ return A->ID() < B->ID(); };
        for(size_t i = 0; i < DStreetMap->NodeCount(); i++) { 
            auto nodeptr = DStreetMap->NodeByIndex(i);
            DSortedNodes.push_back(nodeptr); // Populating vector with node pointers
            DNodeByID.insert({nodeptr->ID(), nodeptr}); // Populating map with node ID key mapped to node ptr
        }
        std::sort(DSortedNodes.begin(), DSortedNodes.end(), CompareByID); // Sorts pointers in ascending node ID order

        for(size_t i = 0; i < DStreetMap->WayCount(); i++) { 
            auto wayptr = DStreetMap->WayByIndex(i);
            DSortedWays.push_back(wayptr); // Populating vector with way pointers
            for (size_t j = 0; j < wayptr->NodeCount(); j++) { // Adds the current way pointer to the set of ways associated with this node ID
                DWaysByNode[wayptr->GetNodeID(j)].insert(wayptr);
            }
        }
        std::sort(DSortedWays.begin(), DSortedWays.end(), CompareByID); // Sorts pointers in ascending way ID order
        
    }


    
    std::size_t NodeCount() const noexcept {
        return DSortedNodes.size();
    }

    std::size_t WayCount() const noexcept {
        return DSortedWays.size();
    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept {
        return index < DSortedNodes.size() ? DSortedNodes.at(index) : nullptr;    
    }

    std::shared_ptr<CStreetMap::SWay> SortedWayByIndex(std::size_t index) const noexcept {
        return index < DSortedWays.size() ? DSortedWays.at(index) : nullptr;
    }

    std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept {
        std::unordered_set<std::shared_ptr<CStreetMap::SWay>> ways;
        
        for(size_t i = 0; i < DSortedWays.size(); i++) {
            auto way = DSortedWays.at(i);
            for (size_t j = 0; j < way->NodeCount(); j++) {
                auto node = DNodeByID.at(way->GetNodeID(j));
                if (NodeInRange(node, lowerleft, upperright)) { // Checks if node is in range
                    ways.insert(way);
                    break;
                }
                
            }
        }

        return ways;
    }

    std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysByNodeID(CStreetMap::TNodeID node) const noexcept {
        auto ways = DWaysByNode.find(node);
        if(ways == DWaysByNode.end()) {
            return {};
        }
        return ways->second;
    }
};

CStreetMapIndexer::CStreetMapIndexer(std::shared_ptr<CStreetMap> streetmap) {
    DImplementation = std::make_unique<SImplementation>(streetmap);
}

CStreetMapIndexer::~CStreetMapIndexer(){

}

std::size_t CStreetMapIndexer::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}

std::size_t CStreetMapIndexer::WayCount() const noexcept {
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> CStreetMapIndexer::SortedNodeByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedNodeByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> CStreetMapIndexer::SortedWayByIndex(std::size_t index) const noexcept {
    return DImplementation->SortedWayByIndex(index);
}

std::unordered_set<std::shared_ptr<CStreetMap::SWay>> CStreetMapIndexer::WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept {
    return DImplementation->WaysInRange(lowerleft, upperright);
}

std::unordered_set<std::shared_ptr<CStreetMap::SWay>> CStreetMapIndexer::WaysByNodeID(CStreetMap::TNodeID node) const noexcept {
    return DImplementation->WaysByNodeID(node);
}