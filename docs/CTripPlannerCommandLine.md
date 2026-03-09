# CTripPlannerCommandLine Documentation

The CTripPlannerCommandLine class implements the main functionality of the
tripplanner program.

## Includes
- `DataFactory.h`: allows this class to access files/directories as sources/sinks
- `TripPlanner.h`: allows this class to access the travel plans
- `TripPlanWriter.h`: allows this class to write the travel plans
- `<memory>`
- `<vector>`

## Class Declaration
```cpp
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
```

## Details
- `struct SImplementation;`: implementation for the trip planner command line
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to `SImplementation`
- `struct SConfig{};`: struct that encapsulates the various sources, sinks, maps, etc. for use
    - `std::shared_ptr<CDataSource> DCommandSource;`: source to get commands from
    - `std::shared_ptr<CDataSink> DOutSink;`: sink to write to
    - `std::shared_ptr<CDataSink> DErrorSink;`: sink to write errors to
    - `std::shared_ptr<CDataFactory> DResultsFactory;` files/directories for results
    - `std::shared_ptr<CTripPlanner> DTripPlanner;` trip planner that generates travel plans
    - `std::shared_ptr<CStreetMap> DStreetMap;` street map to plan trips from
    - `std::shared_ptr<CTripPlanWriter> DOutWriter;`: writes the travel plans to sink
    - `std::shared_ptr<CTripPlanWriter> DStorageWriter;`: writes the travel plans to storage
- `CTripPlannerCommandLine(std::shared_ptr<SConfig> config);`: class constructor for the trip planner command line
- `~CTripPlannerCommandLine();`: class destructor
- `bool ProcessCommands();`: handles commands from user and returns true if successful