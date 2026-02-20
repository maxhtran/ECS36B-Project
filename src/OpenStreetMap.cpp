#include "OpenStreetMap.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>

struct COpenStreetMap::SImplementation {
    const std::string DOSMTag = "osm";
    const std::string DNodeTag = "node";
    const std::string DWayTag = "way";
    const std::string DAttributeTag = "tag";

    const std::string DNDTag = "nd";
    const std::string DWayIDAttr = "id";
    const std::string DNDRefAttr = "ref";
    const std::string DTagKeyAttr = "k";
    const std::string DTagValAttr = "v";
    

    struct SNode : public CStreetMap::SNode {
        TNodeID DID; 
        SLocation DLocation;

        const std::string DNodeIDAttr = "id";
        const std::string DNodeLatAttr = "lat";
        const std::string DNodeLonAttr = "lon";

        SNode(const SXMLEntity &entity) {
            // Convert XML attributes into strongly typed values
            auto NodeID = std::stoull(entity.AttributeValue(DNodeIDAttr));
            auto NodeLat = std::stod(entity.AttributeValue(DNodeLatAttr));
            auto NodeLon = std::stod(entity.AttributeValue(DNodeLonAttr));

            DID = NodeID;
            DLocation = SLocation{NodeLat, NodeLon};
        }

        ~SNode(){};

        std::vector<std::string> DAttributeKeys;
        std::unordered_map<std::string, std::string> DAttributeMap;

        void AddTag(const SXMLEntity &tagEntity){
            // Extract <tag k="..." v="..."/> values
            const std::string key = tagEntity.AttributeValue("k");
            const std::string val = tagEntity.AttributeValue("v");
            if(key.empty()){
                return;
            }
            // Only push key once to preserve first-seen order
            if(DAttributeMap.find(key) == DAttributeMap.end()){
                DAttributeKeys.push_back(key);
            }
            DAttributeMap[key] = val;
        }

        TNodeID ID() const noexcept override {
            return DID;
        }

        SLocation Location() const noexcept override {
            return DLocation;
        }

        std::size_t AttributeCount() const noexcept override {
            return DAttributeKeys.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override {
            if(index >= DAttributeKeys.size()){
                return "";
            }
            return DAttributeKeys[index];
        }

        bool HasAttribute(const std::string &key) const noexcept override {
            return DAttributeMap.find(key) != DAttributeMap.end();
        }

        std::string GetAttribute(const std::string &key) const noexcept override {
            auto it = DAttributeMap.find(key);
            if(it == DAttributeMap.end()){
                return "";
            }
            return it->second;
        }
    };


    struct SWay : public CStreetMap::SWay {
        TWayID DID;
        std::vector<TNodeID> DNodeIDs;
        std::vector<std::string> DAttributeKeys{};
        std::unordered_map<std::string, std::string> DAttributeMap{};

        SWay(TWayID id, std::vector<TNodeID> nodeids,
                const std::vector<std::pair<std::string,std::string>>& attributes) {
            DID = id;
            DNodeIDs = std::move(nodeids);

            // load attributes, preserving first-seen key order
            for(const auto &kv : attributes){
                const auto &k = kv.first;
                const auto &v = kv.second;
                if(k.empty()){
                    continue;
                }
                if(DAttributeMap.find(k) == DAttributeMap.end()){
                    DAttributeKeys.push_back(k);
                }
                DAttributeMap[k] = v;
            }
        }
    

        ~SWay(){};

        TWayID ID() const noexcept override {
            return DID;
        }

        std::size_t NodeCount() const noexcept override {
            return DNodeIDs.size();
        }

        TNodeID GetNodeID(std::size_t index) const noexcept override {
            if (index >= DNodeIDs.size()) {
                return CStreetMap::InvalidNodeID;
            }
            return DNodeIDs[index];
        }

        std::size_t AttributeCount() const noexcept override {
            return DAttributeKeys.size();
        }

        std::string GetAttributeKey(std::size_t index) const noexcept override {
            return index < DAttributeKeys.size() ? DAttributeKeys[index] : "";
        }

        bool HasAttribute(const std::string &key) const noexcept override {
            return DAttributeMap.find(key) != DAttributeMap.end();
        }

        std::string GetAttribute(const std::string &key) const noexcept override {
            auto it = DAttributeMap.find(key);
            return it == DAttributeMap.end() ? "" : it->second;
        }
    };

    std::vector<std::shared_ptr<SNode>> DNodesByIndex;
    std::unordered_map<TNodeID, std::shared_ptr<SNode>> DNodesByID;

    std::vector<std::shared_ptr<SWay>> DWaysByIndex;
    std::unordered_map<TWayID, std::shared_ptr<SWay>> DWaysByID;

    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        // Scan until we encounter the requested start element
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        // Scan until matching end element appears
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    void ClearAll() {
        // Used when XML is invalid
        DNodesByIndex.clear();
        DNodesByID.clear();
        DWaysByIndex.clear();
        DWaysByID.clear();
    }

    bool ParseOSM(std::shared_ptr<CXMLReader> src) {
    SXMLEntity TempEntity;

    if(!FindStartTag(src, DOSMTag)) {
        ClearAll(); // Invalid if <osm> root missing
        return false;
    }

    while (src->ReadEntity(TempEntity, true)) {
        if (TempEntity.DType == SXMLEntity::EType::EndElement &&
            TempEntity.DNameData == DOSMTag) {
            return true; // finished </osm> cleanly
        }

        if (TempEntity.DType == SXMLEntity::EType::StartElement &&
            TempEntity.DNameData == DNodeTag) {

            std::shared_ptr<SNode> node;
            try {
                node = std::make_shared<SNode>(TempEntity); // Validate attributes
            } catch(...) {
                ClearAll();
                return false;
            }

            bool sawEndNode = false;

            // Consume children until we see </node>
            while (src->ReadEntity(TempEntity, true)) {
                if (TempEntity.DType == SXMLEntity::EType::EndElement &&
                    TempEntity.DNameData == DNodeTag) {
                    sawEndNode = true;
                    break;
                }

                if (TempEntity.DType == SXMLEntity::EType::StartElement &&
                    TempEntity.DNameData == DAttributeTag) {
                    node->AddTag(TempEntity); // Collect node attributes
                }
            }

            // If ReadEntity stopped and we never saw </node>, XML is invalid
            if (!sawEndNode) {
                ClearAll();
                return false;
            }

            // Reject duplicate node IDs (OSM requires IDs to be unique)
            if (DNodesByID.find(node->ID()) != DNodesByID.end()) {
                ClearAll();
                return false;
            }

            DNodesByIndex.push_back(node);
            DNodesByID[node->ID()] = node;
        }

        else if (TempEntity.DType == SXMLEntity::EType::StartElement &&
                 TempEntity.DNameData == DWayTag) {

            TWayID wayid{};
            std::vector<TNodeID> nodeids;
            std::vector<std::pair<std::string,std::string>> wayattrs;

            try {
                wayid = std::stoull(TempEntity.AttributeValue(DWayIDAttr));
            } catch(...) {
                ClearAll();
                return false;
            }

            bool sawEndWay = false;

            while (src->ReadEntity(TempEntity, true)) {
                if (TempEntity.DType == SXMLEntity::EType::EndElement &&
                    TempEntity.DNameData == DWayTag) {
                    sawEndWay = true;
                    break;
                }

                if (TempEntity.DType == SXMLEntity::EType::StartElement &&
                    TempEntity.DNameData == DNDTag) {
                    try {
                        auto nid = std::stoull(TempEntity.AttributeValue(DNDRefAttr));
                        nodeids.push_back(nid);
                    } catch(...) {
                        ClearAll();
                        return false;
                    }
                }
                else if (TempEntity.DType == SXMLEntity::EType::StartElement &&
                         TempEntity.DNameData == DAttributeTag) {
                    const std::string k = TempEntity.AttributeValue(DTagKeyAttr);
                    const std::string v = TempEntity.AttributeValue(DTagValAttr);
                    if (!k.empty()) {
                        wayattrs.push_back({k, v});
                    }
                }
            }

            if (!sawEndWay) {
                ClearAll();
                return false;
            }

            // Reject duplicate way IDs
            if (DWaysByID.find(wayid) != DWaysByID.end()) {
                ClearAll();
                return false;
            }

            auto way = std::make_shared<SWay>(wayid, std::move(nodeids), wayattrs);
            DWaysByIndex.push_back(way);
            DWaysByID[way->ID()] = way;
        }
    }

    // If we ran out of entities without seeing </osm>, treat as invalid
    ClearAll();
    return false;
}


    SImplementation(std::shared_ptr<CXMLReader> src) {
        if(!src) {
            return;
        }
        ParseOSM(src);
    }

    std::size_t NodeCount() const noexcept {
        return DNodesByIndex.size();
    }

    std::size_t WayCount() const noexcept {
        return DWaysByIndex.size();
    }

    std::shared_ptr<CStreetMap::SNode> NodeByIndex(std::size_t index) const noexcept {
        if (index < DNodesByIndex.size()) {
            return DNodesByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SNode> NodeByID(TNodeID id) const noexcept {
        auto Search = DNodesByID.find(id);

        if (Search != DNodesByID.end()) {
            return Search->second;
        }

        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByIndex(std::size_t index) const noexcept {
        if(index < DWaysByIndex.size()) {
            return DWaysByIndex[index];
        }
        return nullptr;
    }

    std::shared_ptr<CStreetMap::SWay> WayByID(TWayID id) const noexcept {
        auto it = DWaysByID.find(id);
        if(it != DWaysByID.end()) {
            return it->second;
        }
        return nullptr;
    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> src) {
    DImplementation = std::make_unique<SImplementation>(src);
}

COpenStreetMap::~COpenStreetMap() {

}

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeCount();
}

std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->WayCount();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    return DImplementation->NodeByIndex(index);
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    return DImplementation->NodeByID(id);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    return DImplementation->WayByIndex(index);
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    return DImplementation->WayByID(id);
}