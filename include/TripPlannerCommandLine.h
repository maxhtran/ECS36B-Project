#ifndef TRIPPLANNERCOMMANDLINE_H
#define TRIPPLANNERCOMMANDLINE_H

#include "DataFactory.h"
#include "TripPlanner.h"
#include "TripPlanWriter.h"
#include <memory>
#include <vector>

class CTripPlannerCommandLine{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
    public:
        struct SConfig{
            std::shared_ptr<CDataSource> DCommandSource;
            std::shared_ptr<CDataSink> DOutSink;
            std::shared_ptr<CDataSink> DErrorSink;
            std::shared_ptr<CDataFactory> DResultsFactory;
            std::shared_ptr<CTripPlanner> DTripPlanner;
            std::shared_ptr<CStreetMap> DStreetMap;
            std::shared_ptr<CTripPlanWriter> DOutWriter;
            std::shared_ptr<CTripPlanWriter> DStorageWriter;
        };

        CTripPlannerCommandLine(std::shared_ptr<SConfig> config);
        ~CTripPlannerCommandLine();
        bool ProcessCommands();
};

#endif
