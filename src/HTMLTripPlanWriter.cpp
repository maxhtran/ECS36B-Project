#include "HTMLTripPlanWriter.h"

struct CHTMLTripPlanWriter::SImplementation{
    std::shared_ptr<CStreetMap> DStreetMap; 
    std::shared_ptr<CBusSystem> DBussystem; 
    std::shared_ptr<CSVGTripPlanWriter> SVGWriter; 
    std::shared_ptr<CTextTripPlanWriter> TextWriter; 

    SImplementation(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem){
        DStreetMap = streetmap;
        DBussystem = bussystem;
        SVGWriter = std::make_shared<CSVGTripPlanWriter>(DStreetMap, DBussystem);
        TextWriter = std::make_shared<CTextTripPlanWriter>(DBussystem);
    }
    
    ~SImplementation(){

    }

    std::shared_ptr<SConfig> Config() const{
        return SVGWriter->Config(); 
    }

    static bool WriteString(std::shared_ptr<CDataSink> sink, const std::string &str){
        if(!sink){
            return false;
        }
        for(char ch : str){
            if(!sink->Put(ch)){
                return false; // Writes each character of the string into the sink
            }
        }
        return true;
    }

    static std::string EscapeHTML(const std::string &input){
        std::string output;
        for(char ch : input){
            if(ch == '&'){
                output += "&amp;";
            }
            else if(ch == '<'){
                output += "&lt;";
            }
            else if(ch == '>'){
                output += "&gt;";
            }
            else if(ch == '"'){
                output += "&quot;";
            }
            else{
                output += ch;
            }
        }
        return output; // Escapes HTML special characters in text
    }

    static std::string Trim(const std::string &input){
        size_t start = 0;
        while(start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))){
            start++;
        }

        size_t end = input.size();
        while(end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))){
            end--;
        }

        return input.substr(start, end - start); // Removes leading and trailing whitespace
    }

    static std::vector<std::string> SplitLines(const std::string &text){
        std::vector<std::string> lines;
        std::string current;

        for(char ch : text){
            if(ch == '\n'){
                if(!current.empty()){
                    lines.push_back(current);
                    current.clear();
                }
            }
            else if(ch != '\r'){
                current += ch;
            }
        }

        if(!current.empty()){
            lines.push_back(current);
        }

        return lines; // Splits text into lines
    }

    static bool SplitTextLine(const std::string &line, std::string &time, std::string &instruction){
        auto splitindex = line.find(": ");
        if(splitindex == std::string::npos){
            return false;
        }

        time = Trim(line.substr(0, splitindex));
        instruction = Trim(line.substr(splitindex + 2));
        return true; // Splits "time: instruction" into separate strings
    }

    bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
        if(!sink || !DBussystem || !SVGWriter || !TextWriter || plan.size() < 2){
            return false;
        }

        auto textsink = std::make_shared<CStringDataSink>();
        if(!TextWriter->WritePlan(textsink, plan)){
            return false; // Generates plain text instructions
        }

        auto svgsink = std::make_shared<CStringDataSink>();
        if(!SVGWriter->WritePlan(svgsink, plan)){
            return false; // Generates SVG map
        }

        auto lines = SplitLines(textsink->String());
        if(lines.size() != plan.size()){
            return false;
        }

        std::vector<std::pair<std::string,std::string>> parsedlines;
        for(const auto &line : lines){
            std::string time;
            std::string instruction;
            if(!SplitTextLine(line, time, instruction)){
                return false;
            }
            parsedlines.push_back({time, instruction});
        }

        if(!WriteString(sink, "<html><body>")){
            return false; 
        }

        if(!WriteString(sink, "<div class=\"time\">" + EscapeHTML(parsedlines[0].first) + "</div>")){
            return false; // Writes departure time for first bus
        }
        if(!WriteString(sink, "<div>" + EscapeHTML(parsedlines[0].second) + "</div>")){
            return false; // Writes first boarding instruction from text writer
        }

        for(size_t index = 1; index + 1 < plan.size(); index++){
            auto stop = DBussystem->StopByID(plan[index].DStopID);
            if(!stop){
                return false;
            }

            std::string stopdesc = EscapeHTML(stop->Description());
            std::string prevroute = plan[index - 1].DRouteName;
            std::string nextroute = plan[index].DRouteName;

            std::string transfertext = "Get off the " + prevroute + " bus at " + stopdesc +
                                       " (stop " + std::to_string(plan[index].DStopID) +
                                       ") and wait for the " + nextroute + " bus.";

            if(!WriteString(sink, "<div class=\"time\"></div>")){
                return false; // Writes blank time slot for transfer wait instruction
            }
            if(!WriteString(sink, "<div>" + transfertext + "</div>")){
                return false; // Writes transfer wait instruction
            }
            if(!WriteString(sink, "<div class=\"time\">" + EscapeHTML(parsedlines[index].first) + "</div>")){
                return false; // Writes next departure time from text writer
            }
            if(!WriteString(sink, "<div>" + EscapeHTML(parsedlines[index].second) + "</div>")){
                return false; // Writes next boarding instruction from text writer
            }
        }

        if(!WriteString(sink, "<div class=\"time\">" + EscapeHTML(parsedlines.back().first) + "</div>")){
            return false; // Writes final arrival time
        }
        if(!WriteString(sink, "<div>" + EscapeHTML(parsedlines.back().second) + "</div>")){
            return false; // Writes final get-off instruction from text writer
        }

        if(!WriteString(sink, svgsink->String())){
            return false; // Appends generated SVG
        }

        if(!WriteString(sink, "</body></html>")){
            return false; // Ends HTML document
        }

        return true;
    }
};

CHTMLTripPlanWriter::CHTMLTripPlanWriter(std::shared_ptr<CStreetMap> streetmap, std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(streetmap,bussystem);
}

CHTMLTripPlanWriter::~CHTMLTripPlanWriter(){

}

std::shared_ptr<CTripPlanWriter::SConfig> CHTMLTripPlanWriter::Config() const{
    return DImplementation->Config();
}

bool CHTMLTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    return DImplementation->WritePlan(sink,plan);
}