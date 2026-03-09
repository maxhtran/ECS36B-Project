#include "StreetMapIndexer.h"

struct CStreetMapIndexer::SImplementation {
    SImplementation(std::shared_ptr<CStreetMap> streetmap) {
        
    }
    
    std::size_t NodeCount() const noexcept {

    }

    std::size_t WayCount() const noexcept {

    }

    std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept {

    }

    std::shared_ptr<CStreetMap::SWay> SortedWayByIndex(std::size_t index) const noexcept {

    }

    std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept {

    }

    std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysByNodeID(CStreetMap::TNodeID node) const noexcept {

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