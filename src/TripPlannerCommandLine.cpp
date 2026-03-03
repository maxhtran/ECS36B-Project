#include "TripPlannerCommandLine.h"

struct CTripPlannerCommandLine::SImplementation{
    std::shared_ptr<CDataSource> DCommandSource;
    std::shared_ptr<CDataSink> DOutSink;
    std::shared_ptr<CDataSink> DErrorSink;
    std::shared_ptr<CDataFactory> DResultsFactory;
    std::shared_ptr<CTripPlanner> DTripPlanner;
    std::shared_ptr<CStreetMap> DStreetMap;
    std::shared_ptr<CTripPlanWriter> DOutWriter;
    std::shared_ptr<CTripPlanWriter> DStorageWriter;
    inline static constexpr std::string_view DExitCommand = "exit";
    inline static constexpr std::string_view DHelpCommand = "help";

    SImplementation(std::shared_ptr<SConfig> config){
        DCommandSource = config->DCommandSource;
        DOutSink = config->DOutSink;
        DErrorSink = config->DErrorSink;
        DResultsFactory = config->DResultsFactory;
        DTripPlanner = config->DTripPlanner;
        DStreetMap = config->DStreetMap;
        DOutWriter = config->DOutWriter;
        DStorageWriter = config->DStorageWriter;
    }

    ~SImplementation(){

    }

    void OutputString(const std::string &str){
        DOutSink->Write(std::vector<char>{str.begin(),str.end()});
    }

    void OutputPrompt(){
        OutputString("> ");
    }

    std::string InputCommand(){
        std::string Command;
        char TempCh;
        while(!DCommandSource->End()){
            if(DCommandSource->Get(TempCh)){
                if(TempCh == '\n'){
                    return Command;
                }
                Command += std::string(1,TempCh);
            }
        }
        return Command;
    }

    void ParseCommand(const std::string &cmd, std::vector<std::string> &args){
        args.clear();
        size_t Index = 0;
        while(Index < cmd.length()){
            while((Index < cmd.length()) && std::isspace(cmd[Index])){
                Index++;
            }
            std::string Argument;
            while((Index < cmd.length()) && !std::isspace(cmd[Index])){
                Argument += std::string(1,cmd[0]);
                Index++;
            }
            if(!Argument.empty()){
                args.push_back(Argument);
            }
        }
    }

    bool ProcessCommands(){
        while(!DCommandSource->End()){
            OutputPrompt();
            auto Command = InputCommand();
            std::vector<std::string> Arguments;
            ParseCommand(Command,Arguments);
            if(!Arguments.empty()){
                if(Arguments[0] == DExitCommand){
                    return true;
                }
                else if(Arguments[0] == DHelpCommand){
                    OutputString("--------------------------------------------------------------------------\n"
                                    "help     Display this help menu\n"
                                    "exit     Exit the program\n"
                                    "count    Output the number of stops in the system\n"
                                    "config   Output the current configuration\n"
                                    "toggle   Syntax \"toggle flag\"\n"
                                    "         Will toggle the flag specified.\n"
                                    "set      Syntax \"set option value\"\n"
                                    "         Will set the option specified with the value.\n"
                                    "stop     Syntax \"stop [0, count)\"\n"
                                    "         Will output stop ID, node ID, and Lat/Lon for and description.\n"
                                    "leaveat  Syntax \"leaveat time start end\" \n"
                                    "         Calculates the best trip plan from start to end leaving at time.\n"
                                    "arriveby Syntax \"arriveby time start end\" \n"
                                    "         Calculates the best trip plan from start to end arriving by time.\n"
                                    "save     Saves the last calculated trip to file\n");
                }
            }
        }

        return true;
    }
};

CTripPlannerCommandLine::CTripPlannerCommandLine(std::shared_ptr<SConfig> config){
    DImplementation = std::make_unique<SImplementation>(config);
}

CTripPlannerCommandLine::~CTripPlannerCommandLine(){

}

bool CTripPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}
