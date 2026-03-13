#include "SVGTripPlanWriter.h"

struct CSVGTripPlanWriter::SImplementation{
    struct SVisitedStop{
        CBusSystem::TStopID DStopID; // Stop ID to draw
        std::string DColor; // Circle color for this stop
        double DRadius; // Circle radius for this stop
    };

    struct SConfig : public CTripPlanWriter::SConfig {
        std::unordered_set<std::string> valid_flags; // All supported flags
        std::unordered_set<std::string> enabled_flags; // Currently enabled flags
        std::unordered_set<std::string> valid_options; // All supported options
        std::unordered_map<std::string, std::any> option_values; // Maps option to value
        std::unordered_map<std::string, EOptionType> option_types; // Maps option to type

        bool FlagEnabled(std::string_view flag) const override {
            std::string str(flag);
            return enabled_flags.find(str) != enabled_flags.end(); 
        }

        void EnableFlag(std::string_view flag) override {
            std::string str(flag);
            if(valid_flags.find(str) != valid_flags.end()){
                enabled_flags.insert(str); 
            }
        }

        void DisableFlag(std::string_view flag) override {
            std::string str(flag);
            enabled_flags.erase(str);
        }

        std::any GetOption(std::string_view option) const override {
            std::string str(option);
            auto it = option_values.find(str);
            if(it != option_values.end()){
                return it->second; // Returns stored option value
            }
            return std::any(); // Returns empty any if option not found
        }

        std::unordered_set<std::string> ValidFlags() const override {
            return valid_flags;
        }

        EOptionType GetOptionType(std::string_view option) const override {
            std::string str(option);
            auto it = option_types.find(str);
            if(it != option_types.end()){
                return it->second; // Returns stored type for option
            }
            return EOptionType::None; 
        }

        void SetOption(std::string_view option, int value) override {
            std::string str(option);
            if(valid_options.find(str) != valid_options.end()){
                option_values[str] = value; 
                option_types[str] = EOptionType::Int;
            }
        }

        void SetOption(std::string_view option, double value) override {
            std::string str(option);
            if(valid_options.find(str) != valid_options.end()){
                option_values[str] = value; 
                option_types[str] = EOptionType::Double;
            }
        }

        void SetOption(std::string_view option, const std::string &value) override {
            std::string str(option);
            if(valid_options.find(str) != valid_options.end()){
                option_values[str] = value; 
                option_types[str] = EOptionType::String;
            }
        }

        void ClearOption(std::string_view option) override {
            std::string str(option);
            option_values.erase(str); 
            option_types.erase(str); 
        }

        std::unordered_set<std::string> ValidOptions() const override {
            return valid_options; 
        }
    };

    std::shared_ptr<CStreetMap> DStreetMap; 
    std::shared_ptr<CBusSystem> DBussystem; 
    std::shared_ptr<SConfig> DConfig; 

    SImplementation(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem){
        DStreetMap = streetmap;
        DBussystem = bussystem;
        DConfig = std::make_shared<SConfig>();

        std::vector<std::string> flags = {
            std::string(MotorwayEnabled),
            std::string(PrimaryEnabled),
            std::string(SecondaryEnabled),
            std::string(TertiaryEnabled),
            std::string(ResidentialEnabled)
        };
        for(const auto &flag : flags){
            DConfig->valid_flags.insert(flag); 
            DConfig->EnableFlag(flag); 
        }

        std::vector<std::string> options = {
            std::string(SVGWidth),
            std::string(SVGHeight),
            std::string(SVGMarginPixels),
            std::string(SourceColor),
            std::string(DestinationColor),
            std::string(BusStopRadius),
            std::string(SourceRadius),
            std::string(DestinationRadius),
            std::string(BusColor0),
            std::string(BusColor1),
            std::string(StreetColor),
            std::string(MotorwayStroke),
            std::string(PrimaryStroke),
            std::string(SecondaryStroke),
            std::string(TertiaryStroke),
            std::string(ResidentialStroke),
            std::string(BusStroke),
            std::string(LabelMargin),
            std::string(LabelColor),
            std::string(LabelBackground),
            std::string(LabelSize),
            std::string(LabelPaintOrder)
        };
        for(const auto &option : options){
            DConfig->valid_options.insert(option); 
        }

        DConfig->SetOption(SVGWidth, 960); 
        DConfig->SetOption(SVGHeight, 540); 
        DConfig->SetOption(SVGMarginPixels, 30); 

        DConfig->SetOption(SourceColor, std::string("#00FF00"));
        DConfig->SetOption(DestinationColor, std::string("#FF0000")); 

        DConfig->SetOption(BusStopRadius, 8.0); 
        DConfig->SetOption(SourceRadius, 8.0); 
        DConfig->SetOption(DestinationRadius, 8.0);

        DConfig->SetOption(BusColor0, std::string("#8E24AA"));
        DConfig->SetOption(BusColor1, std::string("#F57C00")); 
        DConfig->SetOption(StreetColor, std::string("#B0B0B0"));

        DConfig->SetOption(MotorwayStroke, 6); 
        DConfig->SetOption(PrimaryStroke, 4); 
        DConfig->SetOption(SecondaryStroke, 2); 
        DConfig->SetOption(TertiaryStroke, 2); 
        DConfig->SetOption(ResidentialStroke, 2); 

        DConfig->SetOption(BusStroke, 8); 
        DConfig->SetOption(LabelMargin, 8); 

        DConfig->SetOption(LabelColor, std::string("#000000"));
        DConfig->SetOption(LabelBackground, std::string("#FFFFFF80")); 

        DConfig->SetOption(LabelSize, 16); 
        DConfig->SetOption(LabelPaintOrder, std::string("stroke fill"));
    }

    ~SImplementation(){
    }

    std::shared_ptr<SConfig> Config() const{
        return DConfig; 
    }

    std::shared_ptr<CBusSystem::SStop> StopForID(CBusSystem::TStopID stopid) const{
        return DBussystem->StopByID(stopid);
    }

    std::shared_ptr<CStreetMap::SNode> NodeForStopID(CBusSystem::TStopID stopid) const{
        auto stop = StopForID(stopid);
        if(!stop){
            return nullptr;
        }
        return DStreetMap->NodeByID(stop->NodeID()); // Returns the map node associated with a bus stop
    }

    bool LocationForStopID(CBusSystem::TStopID stopid, CStreetMap::SLocation &location) const{
        auto node = NodeForStopID(stopid);
        if(!node){
            return false;
        }
        location = node->Location(); // Outputs the geographic location for a stop
        return true;
    }

    SSVGPoint ProjectLocation(const CStreetMap::SLocation &location,
                              const CStreetMap::SLocation &lowerleft,
                              const CStreetMap::SLocation &upperright,
                              int width,
                              int height,
                              int margin) const{
        double x = margin + ((location.DLongitude - lowerleft.DLongitude) /
                   (upperright.DLongitude - lowerleft.DLongitude)) * (width - 2.0 * margin);
        double y = margin + ((upperright.DLatitude - location.DLatitude) /
                   (upperright.DLatitude - lowerleft.DLatitude)) * (height - 2.0 * margin);
        return SSVGPoint{x, y}; // Converts latitude and longitude into svg x and y coordinates
    }

    bool StreetEnabled(const std::string &highway) const{
        if(highway == "motorway"){
            return DConfig->FlagEnabled(MotorwayEnabled);
        }
        if(highway == "primary"){
            return DConfig->FlagEnabled(PrimaryEnabled);
        }
        if(highway == "secondary"){
            return DConfig->FlagEnabled(SecondaryEnabled);
        }
        if(highway == "tertiary"){
            return DConfig->FlagEnabled(TertiaryEnabled);
        }
        if(highway == "residential"){
            return DConfig->FlagEnabled(ResidentialEnabled);
        }
        return false; // Returns whether this street type should be drawn
    }

    std::string StreetStrokeWidth(const std::string &highway) const{
        if(highway == "motorway"){
            return std::to_string(std::any_cast<int>(DConfig->GetOption(MotorwayStroke)));
        }
        if(highway == "primary"){
            return std::to_string(std::any_cast<int>(DConfig->GetOption(PrimaryStroke)));
        }
        if(highway == "secondary"){
            return std::to_string(std::any_cast<int>(DConfig->GetOption(SecondaryStroke)));
        }
        if(highway == "tertiary"){
            return std::to_string(std::any_cast<int>(DConfig->GetOption(TertiaryStroke)));
        }
        return std::to_string(std::any_cast<int>(DConfig->GetOption(ResidentialStroke))); // Returns configured width for street type
    }

    bool SupportedHighway(const std::string &highway) const{
        return highway == "motorway" ||
               highway == "primary" ||
               highway == "secondary" ||
               highway == "tertiary" ||
               highway == "residential"; // Returns true for supported street types
    }

    bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
        if(!sink || !DStreetMap || !DBussystem){
            return false; 
        }
        if(plan.size() < 2){
            return false; 
        }

        int width = std::any_cast<int>(DConfig->GetOption(SVGWidth)); 
        int height = std::any_cast<int>(DConfig->GetOption(SVGHeight)); 
        int margin = std::any_cast<int>(DConfig->GetOption(SVGMarginPixels)); 

        CSVGWriter writer(sink, width, height); 
        CStreetMapIndexer streetmapindexer(DStreetMap); 
        CBusSystemIndexer busindexer(DBussystem); 

        std::string sourcecolor = std::any_cast<std::string>(DConfig->GetOption(SourceColor)); // Color for starting stop
        std::string destcolor = std::any_cast<std::string>(DConfig->GetOption(DestinationColor)); // Color for ending stop
        std::string buscolor0 = std::any_cast<std::string>(DConfig->GetOption(BusColor0)); // First bus route color
        std::string buscolor1 = std::any_cast<std::string>(DConfig->GetOption(BusColor1)); // Second bus route color

        double sourceradius = std::any_cast<double>(DConfig->GetOption(SourceRadius)); // Radius for starting stop
        double destradius = std::any_cast<double>(DConfig->GetOption(DestinationRadius)); // Radius for ending stop
        double busradius = std::any_cast<double>(DConfig->GetOption(BusStopRadius)); // Radius for intermediate stops

        std::vector<SVisitedStop> visitedstops;
        std::vector<CStreetMap::SLocation> triplocations; 

        for(size_t stepindex = 0; stepindex + 1 < plan.size(); stepindex++){
            if(plan[stepindex].DRouteName.empty()){
                return false; // Every plan entry must have a route name except for final
            }

            auto routeindexer = busindexer.RouteByName(plan[stepindex].DRouteName);
            if(!routeindexer){
                return false; // Route name must exist
            }

            auto segmentstops = routeindexer->StopIDsSourceDestination(plan[stepindex].DStopID,
                                                                       plan[stepindex + 1].DStopID);

            if(segmentstops.empty() || segmentstops.front() != plan[stepindex].DStopID){
                segmentstops.insert(segmentstops.begin(), plan[stepindex].DStopID); // Ensures source stop is included
            }
            if(segmentstops.back() != plan[stepindex + 1].DStopID){
                segmentstops.push_back(plan[stepindex + 1].DStopID); // Ensures destination stop is included
            }

            for(size_t stopindex = 0; stopindex < segmentstops.size(); stopindex++){
                CStreetMap::SLocation location;
                if(!LocationForStopID(segmentstops[stopindex], location)){
                    return false; // Every stop must map to a street node location
                }
                triplocations.push_back(location); // Add stop location to trip extents

                bool alreadyadded = false;
                for(const auto &visited : visitedstops){
                    if(visited.DStopID == segmentstops[stopindex]){
                        alreadyadded = true;
                        break;
                    }
                }

                if(!alreadyadded){
                    std::string color = (stepindex % 2) ? buscolor1 : buscolor0; // Alternates bus colors 
                    double radius = busradius; 

                    if(stepindex == 0 && stopindex == 0){
                        color = sourcecolor;
                        radius = sourceradius; 
                    }
                    if(stepindex + 2 == plan.size() && stopindex + 1 == segmentstops.size()){
                        color = destcolor;
                        radius = destradius; 
                    }

                    visitedstops.push_back({segmentstops[stopindex], color, radius}); // Records stop for drawing
                }
            }

            for(size_t stopidindex = 0; stopidindex + 1 < segmentstops.size(); stopidindex++){
                auto path = DBussystem->PathByStopIDs(segmentstops[stopidindex], segmentstops[stopidindex + 1]);
                if(!path){
                    return false; // A path between adjacent route stops must exist
                }

                for(size_t nodeindex = 0; nodeindex < path->NodeCount(); nodeindex++){
                    auto node = DStreetMap->NodeByID(path->GetNodeID(nodeindex));
                    if(!node){
                        return false;
                    }
                    triplocations.push_back(node->Location()); // Add all bus path nodes to trip extents
                }
            }
        }

        CStreetMap::SLocation triplowerleft;
        CStreetMap::SLocation tripupperright;
        if(!SGeographicUtils::CalculateExtents(triplocations, triplowerleft, tripupperright)){
            return false; 
        }

        std::vector<CStreetMap::SLocation> maplocations;
        for(size_t nodeindex = 0; nodeindex < streetmapindexer.NodeCount(); nodeindex++){
            auto node = streetmapindexer.SortedNodeByIndex(nodeindex);
            if(node){
                maplocations.push_back(node->Location()); // Collects all map node locations
            }
        }

        CStreetMap::SLocation maplowerleft;
        CStreetMap::SLocation mapupperright;
        if(!SGeographicUtils::CalculateExtents(maplocations, maplowerleft, mapupperright)){
            return false; // Computes extents of entire street map
        }

        CStreetMap::SLocation lowerleft;
        CStreetMap::SLocation upperright;

        lowerleft.DLatitude = triplowerleft.DLatitude; // Uses trip latitude range
        upperright.DLatitude = tripupperright.DLatitude;
        lowerleft.DLongitude = maplowerleft.DLongitude; // Uses full map longitude range
        upperright.DLongitude = mapupperright.DLongitude;

        if(lowerleft.DLatitude == upperright.DLatitude){
            lowerleft.DLatitude -= 0.000001;
            upperright.DLatitude += 0.000001; // Avoids divide by zero on projection
        }
        if(lowerleft.DLongitude == upperright.DLongitude){
            lowerleft.DLongitude -= 0.000001;
            upperright.DLongitude += 0.000001; // Avoids divide by zero on projection
        }

        auto ways = streetmapindexer.WaysInRange(lowerleft, upperright);
        for(const auto &way : ways){
            if(!way){
                continue;
            }

            std::string highway;
            for(size_t attrindex = 0; attrindex < way->AttributeCount(); attrindex++){
                if(way->GetAttributeKey(attrindex) == "highway"){
                    highway = way->GetAttribute("highway"); // Finds highway type from way attributes
                    break;
                }
            }

            if(!SupportedHighway(highway) || !StreetEnabled(highway)){
                continue; // Skips unsupported or disabled street types
            }

            std::vector<SSVGPoint> points;
            for(size_t nodeindex = 0; nodeindex < way->NodeCount(); nodeindex++){
                auto node = DStreetMap->NodeByID(way->GetNodeID(nodeindex));
                if(node){
                    points.push_back(ProjectLocation(node->Location(),
                                                     lowerleft,
                                                     upperright,
                                                     width,
                                                     height,
                                                     margin)); // Projects street way nodes into svg coordinates
                }
            }

            if(points.size() >= 2){
                TAttributes style = {
                    {"fill", "none"},
                    {"stroke", std::any_cast<std::string>(DConfig->GetOption(StreetColor))},
                    {"stroke-width", StreetStrokeWidth(highway)}
                };
                if(!writer.SimplePath(points, style)){
                    return false; // Draws street path
                }
            }
        }

        for(size_t stepindex = 0; stepindex + 1 < plan.size(); stepindex++){
            auto routeindexer = busindexer.RouteByName(plan[stepindex].DRouteName);
            if(!routeindexer){
                return false;
            }

            auto segmentstops = routeindexer->StopIDsSourceDestination(plan[stepindex].DStopID,
                                                                       plan[stepindex + 1].DStopID);

            if(segmentstops.empty() || segmentstops.front() != plan[stepindex].DStopID){
                segmentstops.insert(segmentstops.begin(), plan[stepindex].DStopID); // Ensures source stop is included
            }
            if(segmentstops.back() != plan[stepindex + 1].DStopID){
                segmentstops.push_back(plan[stepindex + 1].DStopID); // Ensures destination stop is included
            }

            std::vector<SSVGPoint> buspoints;
            for(size_t stopidindex = 0; stopidindex + 1 < segmentstops.size(); stopidindex++){
                auto path = DBussystem->PathByStopIDs(segmentstops[stopidindex], segmentstops[stopidindex + 1]);
                if(!path){
                    return false;
                }

                for(size_t nodeindex = 0; nodeindex < path->NodeCount(); nodeindex++){
                    auto node = DStreetMap->NodeByID(path->GetNodeID(nodeindex));
                    if(!node){
                        return false;
                    }

                    auto point = ProjectLocation(node->Location(),
                                                 lowerleft,
                                                 upperright,
                                                 width,
                                                 height,
                                                 margin); // Projects bus route path nodes into svg coordinates

                    if(buspoints.empty() ||
                       buspoints.back().DX != point.DX ||
                       buspoints.back().DY != point.DY){
                        buspoints.push_back(point); // Avoids duplicating identical consecutive points
                    }
                }
            }

            if(buspoints.size() >= 2){
                TAttributes busstyle = {
                    {"fill", "none"},
                    {"stroke", (stepindex % 2) ? buscolor1 : buscolor0},
                    {"stroke-width", std::to_string(std::any_cast<int>(DConfig->GetOption(BusStroke)))}
                };
                if(!writer.SimplePath(buspoints, busstyle)){
                    return false; // Draws bus route path
                }
            }
        }

        for(const auto &visited : visitedstops){
            auto stop = StopForID(visited.DStopID);
            if(!stop){
                return false;
            }

            CStreetMap::SLocation location;
            if(!LocationForStopID(visited.DStopID, location)){
                return false;
            }

            auto center = ProjectLocation(location, lowerleft, upperright, width, height, margin); // Stop circle center in svg space

            TAttributes circlestyle = {
                {"fill", visited.DColor},
                {"stroke", "#000000"},
                {"stroke-width", "1"}
            };
            if(!writer.Circle(center, visited.DRadius, circlestyle)){
                return false; // Draws stop circle
            }

            std::string escapedlabel;
            for(char ch : stop->Description()){
                if(ch == '&'){
                    escapedlabel += "&amp;";
                }
                else if(ch == '<'){
                    escapedlabel += "&lt;";
                }
                else if(ch == '>'){
                    escapedlabel += "&gt;";
                }
                else if(ch == '\"'){
                    escapedlabel += "&quot;";
                }
                else if(ch == '\''){
                    escapedlabel += "&apos;";
                }
                else{
                    escapedlabel += ch;
                }
            } // Escapes xml special characters in stop label text

            SSVGPoint labelpoint{
                center.DX + std::any_cast<int>(DConfig->GetOption(LabelMargin)),
                center.DY - std::any_cast<int>(DConfig->GetOption(LabelMargin))
            }; 

            TAttributes textstyle = {
                {"fill", std::any_cast<std::string>(DConfig->GetOption(LabelColor))},
                {"font-size", std::to_string(std::any_cast<int>(DConfig->GetOption(LabelSize)))},
                {"stroke", std::any_cast<std::string>(DConfig->GetOption(LabelBackground))},
                {"stroke-width", "3"},
                {"paint-order", std::any_cast<std::string>(DConfig->GetOption(LabelPaintOrder))}
            };
            if(!writer.Text(labelpoint, escapedlabel, textstyle)){
                return false; // Draws stop label text
            }
        }

        return true; 
    }
};

CSVGTripPlanWriter::CSVGTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(streetmap, bussystem); 
}

CSVGTripPlanWriter::~CSVGTripPlanWriter(){
}

std::shared_ptr<CTripPlanWriter::SConfig> CSVGTripPlanWriter::Config() const{
    return DImplementation->Config(); 
}

bool CSVGTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    return DImplementation->WritePlan(sink, plan); 
}