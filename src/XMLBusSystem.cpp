#include "XMLBusSystem.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>

struct CXMLBusSystem::SImplementation{
    const std::string DBusSystemTag = "bussystem";

    const std::string DStopsTag = "stops";
    const std::string DStopTag = "stop";
    const std::string DStopIDAttr = "id";
    const std::string DStopNodeAttr = "node";
    const std::string DStopDescAttr = "description";

    const std::string DRoutesTag = "routes";
    const std::string DRouteTag = "route";
    const std::string DRouteStopTag = "routestop";
    const std::string DRouteNameAttr = "name";
    const std::string DRouteScheduleAttr = "schedule";
    const std::string DRouteStopStopAttr = "stop";
    const std::string DRouteStopDeltaAttr = "delta";

    const std::string DPathsTag = "paths";
    const std::string DPathTag = "path";
    const std::string DPathSourceAttr = "source";
    const std::string DPathDestAttr = "destination";
    const std::string DPathNodeTag = "node";
    const std::string DPathNodeIDAttr = "id";

    

    struct SStop : public CBusSystem::SStop{
        TStopID DID;
        CStreetMap::TNodeID DNodeID;
        std::string DDescription;

        SStop(TStopID id, CStreetMap::TNodeID nodeid, const std::string &description){
            DID = id;
            DNodeID = nodeid;
            DDescription = description;
        }
        ~SStop(){};
        TStopID ID() const noexcept override{
            return DID;
        }

        CStreetMap::TNodeID NodeID() const noexcept override{
            return DNodeID;
        }

        std::string Description() const noexcept override{
            return DDescription;
        }

        std::string Description(const std::string &description) noexcept override{
            DDescription = description;
            return DDescription;
        }
    };

    struct SRoute : public CBusSystem::SRoute {
        std::string DName;
        std::vector<TStopTime> DSchedule;
        std::vector<std::pair<std::shared_ptr<SStop>, int>> DStops;

        SRoute(std::string name, std::vector<TStopTime> schedule, std::vector<std::pair<std::shared_ptr<SStop>, int>> stops) {
            DName = name;
            DSchedule = schedule;
            DStops = stops;
        }
        
        ~SRoute(){};

        std::string Name() const noexcept override {
            return DName;
        }

        std::size_t StopCount() const noexcept override {
            return DStops.size();
        }

        TStopID GetStopID(std::size_t index) const noexcept override {
            if (index >= StopCount()) {
                return CBusSystem::InvalidStopID;
            }

            return DStops[index].first->ID();
        }

        std::size_t TripCount() const noexcept override {
            return DSchedule.size();
        }

        TStopTime GetStopTime(std::size_t stopindex, std::size_t tripindex) const noexcept override {
            int schedule_time = DSchedule[tripindex].hours().count() * 60 + DSchedule[tripindex].minutes().count();
            int delta = DStops[stopindex].second;

            return std::chrono::hh_mm_ss<std::chrono::seconds>(std::chrono::minutes(schedule_time + delta));
        }
    };

    struct SPath : public CBusSystem::SPath {
        std::vector<CStreetMap::TNodeID> DNodes;

        SPath(std::vector<CStreetMap::TNodeID> nodes) {
            DNodes = nodes;
        }

        ~SPath(){};

        CStreetMap::TNodeID StartNodeID() const noexcept override {
            return DNodes[0];
        }

        CStreetMap::TNodeID EndNodeID() const noexcept override {
            return DNodes[NodeCount() - 1];
        }

        std::size_t NodeCount() const noexcept override {
            return DNodes.size();
        }

        CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept override {
            if (index >= NodeCount()) {
                return CStreetMap::InvalidNodeID;
            }

            return DNodes[index];
        }
    };

    bool FindStartTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::StartElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    bool FindEndTag(std::shared_ptr< CXMLReader > xmlsource, const std::string &starttag){
        SXMLEntity TempEntity;
        while(xmlsource->ReadEntity(TempEntity, true)){
            if((TempEntity.DType == SXMLEntity::EType::EndElement)&&(TempEntity.DNameData == starttag)){
                return true;
            }
        }
        return false;
    }

    std::vector<std::shared_ptr<SStop> > DStopsByIndex;
    std::unordered_map<TStopID,std::shared_ptr<SStop> > DStopsByID;

    void ParseStop(std::shared_ptr< CXMLReader > systemsource, const SXMLEntity &stop){
        std::string stopIDStr = stop.AttributeValue(DStopIDAttr);
        std::string nodeIDStr = stop.AttributeValue(DStopNodeAttr);

        if (stopIDStr.empty() || nodeIDStr.empty()) {
            return;
        }
        
        TStopID StopID = std::stoull(stopIDStr);
        CStreetMap::TNodeID NodeID = std::stoull(nodeIDStr);
        auto NewStop = std::make_shared<SStop>(StopID, NodeID, stop.AttributeValue(DStopDescAttr));
        DStopsByIndex.push_back(NewStop);
        DStopsByID[StopID] = NewStop;
        FindEndTag(systemsource,DStopTag);
    }

    void ParseStops(std::shared_ptr< CXMLReader > systemsource){
        SXMLEntity TempEntity;

        do{
            if(!systemsource->ReadEntity(TempEntity, true)){
                return;
            }
            if((TempEntity.DType == SXMLEntity::EType::StartElement) &&(TempEntity.DNameData == DStopTag)){
                ParseStop(systemsource,TempEntity);
            }

        }while((TempEntity.DType != SXMLEntity::EType::EndElement)||(TempEntity.DNameData != DStopsTag));
    }

    std::vector<std::shared_ptr<SRoute>> DRoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> DRoutesByName;

    std::vector<std::pair<std::shared_ptr<SStop>, int>> GetRouteStops(std::shared_ptr<CXMLReader> systemsource) {
        std::vector<std::pair<std::shared_ptr<SStop>, int>> RouteStops;
        SXMLEntity TempEntity;
        
        while (systemsource->ReadEntity(TempEntity, true) && TempEntity.DNameData == DRouteStopTag) {
            std::string stopIDStr = TempEntity.AttributeValue(DRouteStopStopAttr);
            std::string deltaStr = TempEntity.AttributeValue(DRouteStopDeltaAttr);
            if (!stopIDStr.empty() && !deltaStr.empty()) {
                TStopID RouteStopID = std::stoull(stopIDStr);
                int RouteStopDelta = deltaStr.length() == 4 ? std::stoi(deltaStr.substr(1, 1)) : std::stoi(deltaStr.substr(1, 2));
                RouteStops.push_back(std::make_pair(StopByID(RouteStopID), RouteStopDelta));
            }
        }

        return RouteStops;
    }

    std::vector<TStopTime> GetRouteSchedule(std::string schedulestring) {
        std::vector<TStopTime> schedule;

        for (int i = 0; i < (int)schedulestring.length(); i += 9) {
            std::string hour_string = schedulestring.substr(i, 2);
            std::string minute_string = schedulestring.substr(i + 3, 2);
            std::string am_or_pm = schedulestring.substr(i + 6, 2);

            int hour = std::stoi(hour_string);
            if (am_or_pm == "PM" && hour != 12) { hour += 12; }
            int minute = std::stoi(minute_string);

            schedule.push_back(std::chrono::hh_mm_ss<std::chrono::seconds>(std::chrono::minutes(hour * 60 + minute)));
        }

        return schedule;
    }

    void ParseRoute(std::shared_ptr< CXMLReader > systemsource, const SXMLEntity &route){
        std::string RouteName = route.AttributeValue(DRouteNameAttr);
        std::string RouteSchedule = route.AttributeValue(DRouteScheduleAttr);
        auto NewRoute = std::make_shared<SRoute>(RouteName, GetRouteSchedule(RouteSchedule), GetRouteStops(systemsource));
        DRoutesByIndex.push_back(NewRoute);
        DRoutesByName[RouteName] = NewRoute;
    }

    void ParseRoutes(std::shared_ptr< CXMLReader > systemsource){
        SXMLEntity TempEntity;

        do{
            if(!systemsource->ReadEntity(TempEntity, true)){
                return;
            }
            if((TempEntity.DType == SXMLEntity::EType::StartElement) &&(TempEntity.DNameData == DRouteTag)){
                ParseRoute(systemsource,TempEntity);
            }

        }while((TempEntity.DType != SXMLEntity::EType::EndElement)||(TempEntity.DNameData != DRoutesTag));
    }

    void ParseBusSystem(std::shared_ptr< CXMLReader > systemsource){
        if(!FindStartTag(systemsource,DBusSystemTag)){
            return;
        }

        if(!FindStartTag(systemsource,DStopsTag)){
            return;
        }
        ParseStops(systemsource);

        if (!FindStartTag(systemsource, DRoutesTag)) {
            return;
        }
        ParseRoutes(systemsource);
    }

    
    std::map<std::pair<TStopID, TStopID>, std::shared_ptr<SPath>> DPathsbyStopIDs;

    std::vector<CStreetMap::TNodeID> GetPathNodes(std::shared_ptr<CXMLReader> systemsource) {
        std::vector<CStreetMap::TNodeID> PathNodes;
        SXMLEntity TempEntity;
        
        while (systemsource->ReadEntity(TempEntity, true) && TempEntity.DNameData == DPathNodeTag) {
            std::string nodeIDStr = TempEntity.AttributeValue(DPathNodeIDAttr);

            if (!nodeIDStr.empty()) {
                CStreetMap::TNodeID PathNodeID = std::stoull(TempEntity.AttributeValue(DPathNodeIDAttr));
                PathNodes.push_back(PathNodeID);
            }
        }

        return PathNodes;
    }

    void ParsePath(std::shared_ptr<CXMLReader> pathsource, const SXMLEntity &path) {
        auto NewPath = std::make_shared<SPath>(GetPathNodes(pathsource));
        DPathsbyStopIDs[{NewPath->StartNodeID(), NewPath->EndNodeID()}] = NewPath;
    }

    void ParsePaths(std::shared_ptr<CXMLReader> pathsource) {
        SXMLEntity TempEntity;

        do {
            if (!pathsource->ReadEntity(TempEntity, true)) {
                return;
            }
            if (TempEntity.DType == SXMLEntity::EType::StartElement && TempEntity.DNameData == DPathTag) {
                ParsePath(pathsource, TempEntity);
            }
        } while (TempEntity.DType != SXMLEntity::EType::EndElement || TempEntity.DNameData != DPathsTag);
    }

    void ParsePathSystem(std::shared_ptr<CXMLReader> pathsource) {
        if (!FindStartTag(pathsource, DPathsTag)) {
            return;
        }

        ParsePaths(pathsource);
    }

    SImplementation(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
        ParseBusSystem(systemsource);
        ParsePathSystem(pathsource);
    }

    std::size_t StopCount() const noexcept{
        return DStopsByIndex.size();
    }

    std::size_t RouteCount() const noexcept{
        return DRoutesByIndex.size();
    }
    
    std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept{
        if (index >= StopCount()) {
            return nullptr;
        }
        
        return DStopsByIndex[index];
    }
    
    std::shared_ptr<SStop> StopByID(TStopID id) const noexcept{
        if (DStopsByID.count(id) == 0) {
            return nullptr;
        }
        
        return DStopsByID.at(id);
    }
    
    std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept{
        if (index >= RouteCount()) {
            return nullptr;
        }

        return DRoutesByIndex[index];
    }
    
    std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept{
        if (DRoutesByName.count(name) == 0) {
            return nullptr;
        }

        return DRoutesByName.at(name);
    }
    
    std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept{
        auto startStop = StopByID(start);
        auto endStop = StopByID(end);

        if (!startStop || !endStop) {
            return nullptr;
        }

        CStreetMap::TNodeID startStopNode = startStop->NodeID();
        CStreetMap::TNodeID endStopNode = endStop->NodeID();
        
        if (DPathsbyStopIDs.count({startStopNode, endStopNode}) == 0) {
            return nullptr;
        }
        
        return DPathsbyStopIDs.at({startStopNode, endStopNode});
    }
    
};

CXMLBusSystem::CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource){
    DImplementation = std::make_unique<SImplementation>(systemsource,pathsource);
}
    
CXMLBusSystem::~CXMLBusSystem(){

}
    
std::size_t CXMLBusSystem::StopCount() const noexcept{
    return DImplementation->StopCount();
}
    
std::size_t CXMLBusSystem::RouteCount() const noexcept{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByIndex(std::size_t index) const noexcept{
    return DImplementation->StopByIndex(index);
}

std::shared_ptr<CBusSystem::SStop> CXMLBusSystem::StopByID(TStopID id) const noexcept{
    return DImplementation->StopByID(id);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByIndex(std::size_t index) const noexcept{
    return DImplementation->RouteByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CXMLBusSystem::RouteByName(const std::string &name) const noexcept{
    return DImplementation->RouteByName(name);
}

std::shared_ptr<CBusSystem::SPath> CXMLBusSystem::PathByStopIDs(TStopID start, TStopID end) const noexcept{
    return DImplementation->PathByStopIDs(start, end);
}
