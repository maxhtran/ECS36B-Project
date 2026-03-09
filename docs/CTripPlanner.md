# CTripPlanner Documentation

The CTripPlanner class determines what bus route(s) to take and when.

## Includes
- `BusSystem.h`: allows this class to deal with bus systems
- `BusSystemIndexer.h`: allows this class to index bus systems
- `<memory>`

## Class Declaration
```cpp
class CTripPlanner{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        using TStopID = CBusSystem::TStopID;
        using TStopTime = CBusSystem::TStopTime;
        using SRoute = CBusSystem::SRoute;

        struct STravelStep{
            TStopTime DTime;
            TStopID DStopID;
            std::string DRouteName;
        };
        
        using TTravelPlan = std::vector< STravelStep >;

        CTripPlanner(std::shared_ptr<CBusSystem> bussystem);
        ~CTripPlanner();

        std::shared_ptr<CBusSystemIndexer> BusSystemIndexer() const;

        std::shared_ptr< SRoute > FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const;
        std::shared_ptr< SRoute > FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const;

        bool FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const;
        bool FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const;
};
```

## Details
- `struct SImplementation;`: implementation for the trip planner
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to `SImplementation`
- `using TStopID = CBusSystem::TStopID;`: transfer the TStopID alias over from the CBusSystem class
- `using TStopTime = CBusSystem::TStopTime;`: transfer the TStopTime alias over from the CBusSystem class
- `using SRoute = CBusSystem::SRoute;`: transfer the SRoute alias over from the CBusSystem class
- `struct STravelStep{};`: struct that encapsulates data for a travel step
    - `TStopTime DTime;`: time of the stop
    - `TStopID DStopID;`: ID of the stop
    - `std::string DRouteName;`: name of the route
- `using TTravelPlan = std::vector<STravelStep>;`: alias for a vector of `STravelSteps`
- `CTripPlanner(std::shared_ptr<CBusSystem> bussystem);`: class constructor for the trip planner
    - bussystem: bus system to get data from
- `~CTripPlanner();`: class destructor
- `std::shared_ptr<CBusSystemIndexer> BusSystemIndexer() const;`: returns the bus system indexer
- `std::shared_ptr<SRoute> FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const;`: returns the route to take from `src` to `dest` if leaving at the specified `leaveat` time, or nullptr if no route exists
    - the route should be the one to arrive at `dest` earliest
- `std::shared_ptr<SRoute> FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const;`: returns the route to take from `src` to `dest` to arrive by the specified `arriveby` time, or nullptr if no route exists
    - the route should be the one to leave `src` latest
- `bool FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const;`: returns true if able to find a travel `plan` from `src` to `dest` if leaving at the specified `leaveat` time, or false if no plan is found
    - the plan should be the one to arrive at `dest` earliest
- `bool FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const;`: returns true if able to find a travel `plan` from `src` to `dest` to arrive by the specified `arriveby` time, or false if no plan is found
    - the plan should the be one to leave `src` latest