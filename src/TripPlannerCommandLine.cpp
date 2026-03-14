#include "TripPlannerCommandLine.h"
#include "GeographicUtils.h"

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
    inline static constexpr std::string_view DCountCommand = "count";
    inline static constexpr std::string_view DConfigCommand = "config";
    inline static constexpr std::string_view DToggleCommand = "toggle";
    inline static constexpr std::string_view DSetCommand = "set";
    inline static constexpr std::string_view DStopCommand = "stop";
    inline static constexpr std::string_view DLeaveAtCommand = "leaveat";
    inline static constexpr std::string_view DArriveByCommand = "arriveby";
    inline static constexpr std::string_view DSaveCommand = "save";

    bool DLastPlanExist = false;
    std::string DLAOrAB;
    CBusSystem::TStopID DLastSrc, DLastDest;
    CBusSystem::TStopTime DLastTime;
    CTripPlanner::TTravelPlan DLastPlan;

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

    void OutputError(const std::string &str) {
        DErrorSink->Write(std::vector<char>(str.begin(), str.end()));
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
                Argument += std::string(1,cmd[Index]);
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

                else if (Arguments[0] == DCountCommand) {
                    OutputString(std::to_string(DTripPlanner->BusSystemIndexer()->StopCount()) + " stops\n");
                }

                else if (Arguments[0] == DConfigCommand) {
                    // get all flags and options, then sort each alphabetically
                    std::unordered_set<std::string> OutWriter_flags_set = DOutWriter->Config()->ValidFlags();
                    std::unordered_set<std::string> StorageWriter_flags_set = DStorageWriter->Config()->ValidFlags();
                    std::unordered_set<std::string> options_set = DStorageWriter->Config()->ValidOptions();

                    std::vector<std::string> OutWriter_flags(OutWriter_flags_set.begin(), OutWriter_flags_set.end());
                    std::vector<std::string> StorageWriter_flags(StorageWriter_flags_set.begin(), StorageWriter_flags_set.end());
                    std::vector<std::string> options(options_set.begin(), options_set.end());

                    std::vector<std::string> flags;
                    flags.insert(flags.end(), OutWriter_flags.begin(), OutWriter_flags.end());
                    flags.insert(flags.end(), StorageWriter_flags.begin(), StorageWriter_flags.end());

                    std::sort(flags.begin(), flags.end());
                    std::sort(options.begin(), options.end());

                    // print each flag and its value
                    for (std::string flag : flags) {
                        std::string value;
                        
                        if (DOutWriter->Config()->FlagEnabled(flag) || DStorageWriter->Config()->FlagEnabled(flag)) {
                            value = "true";
                        } else {
                            value = "false";
                        }

                        while (flag.length() < 19) { // pads the flag/option names so that the colons are aligned
                            flag += " ";
                        }

                        OutputString(flag + ": " + value + "\n");
                    }

                    // print each option and its value
                    for (std::string option : options) {
                        std::string value;

                        std::any option_value = DStorageWriter->Config()->GetOption(option);
                        CTripPlanWriter::SConfig::EOptionType option_type = DStorageWriter->Config()->GetOptionType(option);

                        if (option_type == CTripPlanWriter::SConfig::EOptionType::Int) { // if option type is int
                            value = std::to_string(std::any_cast<int>(option_value));
                        } else if (option_type == CTripPlanWriter::SConfig::EOptionType::Double) { // if option type is double
                            value = std::to_string(std::any_cast<double>(option_value));
                        } else { // if option type is string
                            value = std::any_cast<std::string>(option_value);
                        }

                        while (option.length() < 19) { // pads the flag/option names so that the colons are aligned
                            option += " ";
                        }

                        OutputString(option + ": " + value + "\n");
                    }
                }

                else if (Arguments[0] == DToggleCommand) {
                    if (Arguments.size() != 2) { // if there isn't exactly one argument given for command
                        OutputError("Invalid toggle command, see help.\n");
                    } else {
                        std::string flag = Arguments[1];
                    
                        if (!DOutWriter->Config()->ValidFlags().contains(flag) && !DStorageWriter->Config()->ValidFlags().contains(flag)) { // if given flag isn't valid
                            OutputError("Invalid toggle parameter, see help.\n");
                        } else { // if there are no errors
                            std::string value;

                            if (DOutWriter->Config()->ValidFlags().contains(flag)) { // if flag is in DOutWriter
                                if (DOutWriter->Config()->FlagEnabled(flag)) { // if flag is already enabled, disable it; otherwise, enable it
                                    DOutWriter->Config()->DisableFlag(flag);
                                    value = "false";
                                } else {
                                    DOutWriter->Config()->EnableFlag(flag);
                                    value = "true";
                                }
                            } else if (DStorageWriter->Config()->ValidFlags().contains(flag)) { // if flag is in DStorageWriter
                                if (DStorageWriter->Config()->FlagEnabled(flag)) { // if flag is already enabled, disable it; otherwise, enable it
                                    DStorageWriter->Config()->DisableFlag(flag);
                                    value = "false";
                                } else {
                                    DStorageWriter->Config()->EnableFlag(flag);
                                    value = "true";
                                }
                            }

                            OutputString("Flag " + flag + " is now " + value + "\n"); // output confirmation message
                        }
                    }
                }

                else if (Arguments[0] == DSetCommand) {
                    if (Arguments.size() != 3) { // if there aren't exactly two arguments given for command
                        OutputError("Invalid set command, see help.\n");
                    } else {
                        std::string option = Arguments[1];
                        std::string option_value = Arguments[2];

                        if (!DStorageWriter->Config()->ValidOptions().contains(option)) { // if given option isn't valid
                            OutputError("Invalid set parameter, see help.\n");
                        } else {
                            try {
                                if (DStorageWriter->Config()->GetOptionType(option) == CTripPlanWriter::SConfig::EOptionType::Int) { // for int options
                                    int value = std::stoi(option_value); // convert option_value string to int
                                    DStorageWriter->Config()->SetOption(option, value);
                                    OutputString("Option " + option + " is now " + std::to_string(value) + "\n"); // output confirmation message
                                } else if (DStorageWriter->Config()->GetOptionType(option) == CTripPlanWriter::SConfig::EOptionType::Double) { // for double options
                                    double value = std::stod(option_value); // convert option_value string to double
                                    DStorageWriter->Config()->SetOption(option, value);
                                    OutputString("Option " + option + " is now " + std::to_string(value) + "\n"); // output confirmation message
                                } else { // for string options
                                    DStorageWriter->Config()->SetOption(option, option_value);
                                    OutputString("Option " + option + " is now " + option_value + "\n"); // output confirmation message
                                }
                            } catch (const std::exception &e) { // if given value's type doesn't match the given option's type
                                OutputError("Invalid set parameter, see help.\n");
                            }
                        }
                    }
                }

                else if (Arguments[0] == DStopCommand) {
                    if (Arguments.size() != 2) { // if there isn't exactly one argument given for command
                        OutputError("Invalid stop command, see help.\n");
                    } else {
                        try {
                            size_t stop_index = std::stoull(Arguments[1]);
                            std::shared_ptr<CBusSystem::SStop> stop = DTripPlanner->BusSystemIndexer()->SortedStopByIndex(stop_index);
                            
                            if (!stop) { // if stop doesn't exist
                                OutputError("Invalid stop parameter, see help.\n");
                            } else {
                                // get location and convert it to DMS form
                                CStreetMap::SLocation location = DStreetMap->NodeByID(stop->NodeID())->Location();
                                std::string converted_location = SGeographicUtils::ConvertLLToDMS(location);

                                OutputString("Stop " + std::to_string(stop_index) + ":\n"
                                                "    ID          : " + std::to_string(stop->ID()) + "\n"
                                                "    Node ID     : " + std::to_string(stop->NodeID()) + "\n"
                                                "    Location    : " + converted_location + "\n"
                                                "    Description : " + stop->Description() + "\n");
                            }
                        } catch (const std::exception &e) { // if given argument isn't valid
                            OutputError("Invalid stop parameter, see help.\n");
                        }
                    }
                }

                else if (Arguments[0] == DLeaveAtCommand) {
                    if (Arguments.size() != 4) { // if there aren't exactly three arguments given for command
                        OutputError("Invalid leaveat command, see help.\n");
                    } else {
                        try {
                            std::string time_str = Arguments[1];
                            CBusSystem::TStopID src = std::stoull(Arguments[2]);
                            CBusSystem::TStopID dest = std::stoull(Arguments[3]);

                            // convert time to TStopTime
                            int hour, minute;
                            std::string am_or_pm;

                            if (time_str.length() == 6) { // if hour is single-digit
                                hour = std::stoi(time_str.substr(0, 1));
                                minute = std::stoi(time_str.substr(2, 2));
                                am_or_pm = time_str.substr(4, 2);
                            } else { // if hour is double-digit
                                hour = std::stoi(time_str.substr(0, 2));
                                minute = std::stoi(time_str.substr(3, 2));
                                am_or_pm = time_str.substr(5, 2);
                            }

                            // handle midnight and PM times
                            if (am_or_pm == "AM" && hour == 12) {
                                hour = 0;
                            } else if (am_or_pm == "PM" && hour < 12) {
                                hour += 12;
                            }

                            CBusSystem::TStopTime time = CBusSystem::TStopTime(std::chrono::hours(hour) + std::chrono::minutes(minute));

                            // create travel plan
                            CTripPlanner::TTravelPlan plan;
                            if (!DTripPlanner->FindRouteLeaveTime(src, dest, time, plan)) { // if no route matches the given arguments
                                OutputError("Unable to find route from " + Arguments[2] + " to " + Arguments[3] + ".\n");
                            } else {
                                DOutWriter->WritePlan(DOutSink, plan);

                                // save plan information for save command
                                DLastPlanExist = true;
                                DLAOrAB = "la";
                                DLastSrc = src;
                                DLastDest = dest;
                                DLastTime = time;
                                DLastPlan = plan;
                            }
                        } catch (const std::exception &e) { // if one or more given arguments are invalid
                            OutputError("Invalid leaveat parameter, see help.\n");
                        }
                    }
                }

                else if (Arguments[0] == DArriveByCommand) {
                    if (Arguments.size() != 4) { // if there aren't exactly three arguments given for command
                        OutputError("Invalid arriveby command, see help.\n");
                    } else {
                        try {
                            std::string time_str = Arguments[1];
                            CBusSystem::TStopID src = std::stoull(Arguments[2]);
                            CBusSystem::TStopID dest = std::stoull(Arguments[3]);

                            // convert time to TStopTime
                            int hour, minute;
                            std::string am_or_pm;

                            if (time_str.length() == 6) { // if hour is single-digit
                                hour = std::stoi(time_str.substr(0, 1));
                                minute = std::stoi(time_str.substr(2, 2));
                                am_or_pm = time_str.substr(4, 2);
                            } else { // if hour is double-digit
                                hour = std::stoi(time_str.substr(0, 2));
                                minute = std::stoi(time_str.substr(3, 2));
                                am_or_pm = time_str.substr(5, 2);
                            }

                            // handle midnight and PM times
                            if (am_or_pm == "AM" && hour == 12) {
                                hour = 0;
                            } else if (am_or_pm == "PM" && hour < 12) {
                                hour += 12;
                            }

                            CBusSystem::TStopTime time = CBusSystem::TStopTime(std::chrono::hours(hour) + std::chrono::minutes(minute));

                            // create travel plan
                            CTripPlanner::TTravelPlan plan;
                            if (!DTripPlanner->FindRouteArrivalTime(src, dest, time, plan)) { // if no route matches the given arguments
                                OutputError("Unable to find route from " + Arguments[2] + " to " + Arguments[3] + ".\n");
                            } else {
                                DOutWriter->WritePlan(DOutSink, plan);

                                // save plan information for save command
                                DLastPlanExist = true;
                                DLAOrAB = "ab";
                                DLastSrc = src;
                                DLastDest = dest;
                                DLastTime = time;
                                DLastPlan = plan;
                            }
                        } catch (const std::exception &e) { // if one or more given arguments are invalid
                            OutputError("Invalid arriveby parameter, see help.\n");
                        }
                    }
                }

                else if (Arguments[0] == DSaveCommand) {
                    if (!DLastPlanExist) { // if no plan has been made yet
                        OutputError("No valid trip to save, see help.\n");
                    } else {
                        std::string minutes_str = std::to_string(DLastTime.hours().count() * 60 + DLastTime.minutes().count()); // get minutes since midnight for filename
                        std::string filename = DLAOrAB + "_" + minutes_str + "_" + std::to_string(DLastSrc) + "_" + std::to_string(DLastDest) + ".html"; // put together filename
                        std::shared_ptr<CDataSink> file = DResultsFactory->CreateSink(filename); // create file to write to
                        DStorageWriter->WritePlan(file, DLastPlan); // write the plan to the file

                        OutputString("Trip saved to <results>/" + filename + "\n"); // output confirmation message
                    }
                }

                else { // if command is invalid
                    OutputError("Unknown command \"" + Arguments[0] + "\" type help for help.\n");
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