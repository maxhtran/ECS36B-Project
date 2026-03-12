#include "TextTripPlanWriter.h"

struct CTextTripPlanWriter::SImplementation{
    struct SConfig : public CTripPlanWriter::SConfig {
        // TextTripPlanWriter has no options, so all option functions should do nothing
        std::unordered_set<std::string> enabled_flags;

        bool FlagEnabled(std::string_view flag) const override {
            return enabled_flags.contains(std::string(flag));
        }

        void EnableFlag(std::string_view flag) override {
            std::string flag_string = std::string(flag);
            if (ValidFlags().contains(flag_string)) { // only valid flags should be considered
                enabled_flags.insert(flag_string);
            }
        }

        void DisableFlag(std::string_view flag) override {
            enabled_flags.erase(std::string(flag));
        }

        std::any GetOption(std::string_view option) const override {
            std::any none;
            return none;
        }

        std::unordered_set<std::string> ValidFlags() const override {
            std::unordered_set<std::string> flags;
            
            std::string verbose_flag(Verbose);
            flags.insert(verbose_flag);

            return flags;
        }

        EOptionType GetOptionType(std::string_view option) const override {
            return EOptionType::None;
        }

        void SetOption(std::string_view option, int value) override {
            return;
        }

        void SetOption(std::string_view option, double value) override {
            return;
        }

        void SetOption(std::string_view option, const std::string &value) override {
            return;
        }

        void ClearOption(std::string_view option) override {
            return;
        }

        std::unordered_set<std::string> ValidOptions() const override {
            std::unordered_set<std::string> none;
            return none;
        }
    };

    std::shared_ptr<CBusSystem> DBusSystem;
    std::shared_ptr<SConfig> DConfig;

    SImplementation(std::shared_ptr<CBusSystem> bussystem){
        DBusSystem = bussystem;
        DConfig = std::make_shared<SConfig>();
    }

    ~SImplementation(){

    }

    std::shared_ptr<SConfig> Config() const{
        return DConfig;
    }

    bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
        if (!sink || plan.empty()) { // if either argument is invalid, return false
            return false;
        }
        
        for (int i = 0; i < static_cast<int>(plan.size()); i++) {
            CTripPlanner::STravelStep step = plan[i]; // current step in plan
            
            // * HANDLE TIME *
            CTripPlanner::TStopTime step_time = step.DTime; // get TStopTime of current step

            // convert from TStopTime to int
            int hour = step_time.hours().count();
            int minute = step_time.minutes().count();

            // handle 12-hour format
            std::string am_or_pm;
            if (hour == 0) {
                hour = 12;
                am_or_pm = "AM";
            } else if (hour < 12) {
                am_or_pm = "AM";
            } else if (hour == 12) {
                am_or_pm = "PM";
            } else {
                hour -= 12;
                am_or_pm = "PM";
            }

            // convert ints to strings and then format them
            std::string hour_string;
            std::string minute_string;
            if (hour < 10) {
                hour_string = " " + std::to_string(hour);
            } else {
                hour_string = std::to_string(hour);
            }
            if (minute < 10) {
                minute_string = "0" + std::to_string(minute);
            } else {
                minute_string = std::to_string(minute);
            }

            std::string time = hour_string + ":" + minute_string + " " + am_or_pm; // final string for time

            // * WRITE TO SINK *
            std::string write_string;
            if (step.DRouteName != "") { // if current step isn't the final step
                write_string = time + ": Take the " + step.DRouteName + " bus from " + DBusSystem->StopByID(step.DStopID)->Description() + " (stop " + std::to_string(step.DStopID) + ").\n";
                std::vector<char> write_vector(write_string.begin(), write_string.end());
                sink->Write(write_vector);
            } else { // if current step is the final step
                write_string = time + ": Get off the " + plan[i - 1].DRouteName + " bus at " + DBusSystem->StopByID(step.DStopID)->Description() + " (stop " + std::to_string(step.DStopID) + ").\n";
                std::vector<char> write_vector(write_string.begin(), write_string.end());
                sink->Write(write_vector);
            }
        }

        return true;
    }
};



CTextTripPlanWriter::CTextTripPlanWriter(std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CTextTripPlanWriter::~CTextTripPlanWriter(){

}

std::shared_ptr<CTripPlanWriter::SConfig> CTextTripPlanWriter::Config() const{
    return DImplementation->Config();
}

bool CTextTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    return DImplementation->WritePlan(sink,plan);
}