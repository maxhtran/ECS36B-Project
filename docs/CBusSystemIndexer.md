# CBusSystemIndexer Documentation

The CBusSystemIndexer class indexes the CBusSystem provided for ease of lookup
of stops and routes. It is somewhat of a helper class for other parts of the
project.

## Includes
- `BusSystem.h`: allows this class to deal with the given CBusSystem
- `<unordered_set>`

## Class Declaration
```cpp
class CBusSystemIndexer{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
        
    public:
        using TStopID = CBusSystem::TStopID;
        using SStop = CBusSystem::SStop;
        using SRoute = CBusSystem::SRoute;
        struct SRouteIndexer : public SRoute{
            virtual ~SRouteIndexer(){};
            virtual size_t FindStopIndex(TStopID stopid, size_t start = 0) const = 0;
            virtual std::vector< TStopID > StopIDsSourceDestination(TStopID src, TStopID dest) const = 0;
        };

        CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem);
        ~CBusSystemIndexer();

        std::size_t StopCount() const noexcept;
        std::size_t RouteCount() const noexcept;
        std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SRouteIndexer> SortedRouteByIndex(std::size_t index) const noexcept;
        std::shared_ptr<SRouteIndexer> RouteByName(const std::string &name) const noexcept;
        bool RoutesByStopID(TStopID stopid, std::unordered_set< std::string > &routes) const noexcept;
        bool RoutesByStopIDs(TStopID src, TStopID dest, std::unordered_set< std::string > &routes) const noexcept;
        bool StopIDsByRoutes(const std::string &route1, const std::string &route2, std::unordered_set< TStopID > &stops) const noexcept;
};
```

## Details
- `struct SImplementation;`: implementation for the indexing of the given CBusSystem
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to `SImplementation`
- `using TStopID = CBusSystem::TStopID;`: transfer the TStopID alias over from the CBusSystem class
- `using SStop = CBusSystem::SStop;`: transfer the SStop alias over from the CBusSystem class
- `using SRoute = CBusSystem::SRoute;`: transfer the SRoute alias over from the CBusSystem class
- `struct SRouteIndexer : public SRoute{};`: SRoute, but with additional functionality
    - `virtual ~SRouteIndexer(){};`: destructor for SRouteIndexer (included for good practice)
    - `virtual size_t FindStopIndex(TStopID stopid, size_t start = 0) const = 0;`: returns the stop index of the `stopid` starting at `start`, or `std::numeric_limits<size_t>::max()` if it can't be found
    - `virtual std::vector<TStopID> StopIDsSourceDestination(TStopID src, TStopID dest) const = 0`: returns the stop IDs of the stops between `src` and `dest`
- `CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem);`: class constructor for the `bussystem` indexer
- `~CBusSystemIndexer();`: class destructor
- `std::size_t StopCount() const noexcept;`: returns the number of stops in the CBusSystem being indexed
- `std::size_t RouteCount() const noexcept;`: returns the number of routes in the CBusSystem being indexed
- `std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept;`: returns the SStop specified by `index` where the stops are sorted by their ID, or nullptr if `index` >= StopCount()
- `std::shared_ptr<SRouteIndexer> SortedRouteByIndex(std::size_t index) const noexcept;`: returns the SRouteIndexer specified by `index` where the routes are sorted by their name, or nullptr if `index` >= RouteCount()
- `std::shared_ptr<SRouteIndexer> RouteByName(const std::string &name) const noexcept;`: returns a route indexer by the route `name`
- `bool RoutesByStopID(TStopID stopid, std::unordered_set<std::string> &routes) const noexcept;`: finds all names of `routes` that stop at `stopid`
- `bool RoutesByStopIDs(TStopID src, TStopID dest, std::unordered_set<std::string> &routes) const noexcept;`: finds all `routes` that have `src` and `dest` in their route
- `bool StopIDsByRoutes(const std::string &route1, const std::string &route2, std::unordered_set<TStopID> &stops) const noexcept;`: finds all stops that have the two `routes` in common