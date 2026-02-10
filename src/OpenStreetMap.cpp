#include "OpenStreetMap.h"

struct COpenStreetMap::SImplementation {
    struct SNode : public CStreetMap::SNode {
        TNodeID DID;
        SLocation DLocation;
        TAttributes DAttributes;

        SNode(TNodeID id, SLocation location, TAttributes attributes) {
            DID = id;
            DLocation = location;
            DAttributes = attributes;
        }

        ~SNode(){};

        TNodeID ID() const noexcept override {

        }

        SLocation Location() const noexcept override {

        }

        std::size_t AttributeCount() const noexcept override {

        }

        std::string GetAttributeKey(std::size_t index) const noexcept override {

        }

        bool HasAttribute(const std::string &key) const noexcept override {

        }

        std::string GetAttribute(const std::string &key) const noexcept override {

        }
    };

    struct SWay : public CStreetMap::SWay {
        TWayID DID;
        std::vector<SNode> DNodes;
        TAttributes DAttributes;

        SWay(TWayID id, std::vector<SNode> nodes, TAttributes attributes) {
            DID = id;
            DNodes = nodes;
            DAttributes = attributes;
        }

        ~SWay(){};

        TWayID ID() const noexcept override {

        }

        std::size_t NodeCount() const noexcept override {

        }

        TNodeID GetNodeID(std::size_t index) const noexcept override {

        }

        std::size_t AttributeCount() const noexcept override {

        }

        std::string GetAttributeKey(std::size_t index) const noexcept override {

        }

        bool HasAttribute(const std::string &key) const noexcept override {

        }

        std::string GetAttribute(const std::string &key) const noexcept override {
            
        }
    };

    SImplementation(std::shared_ptr<CXMLReader> src) {

    }

    std::size_t NodeCount() const noexcept {

    }

    std::size_t WayCount() const noexcept {

    }

    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept {

    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept {

    }

    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept {

    }

    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept {

    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {

}

COpenStreetMap::~COpenStreetMap() {

}

std::size_t COpenStreetMap::NodeCount() const noexcept {
    
}

std::size_t COpenStreetMap::WayCount() const noexcept {

}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {

}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {

}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {

}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {

}