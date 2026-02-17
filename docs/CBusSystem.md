# CBusSystem Documentation

The CBusSystem class is the abstract interface for the CXMLBusSystem class, providing 
the signatures of the necessary functions and a convenient alias for the stop IDs.

## Includes
- `StreetMap.h`: provides an alias for node IDs

## Class Declaration
```cpp
class CBusSystem {
    public:
        using TStopID = uint64_t;

        inline static constexpr TStopID InvalidStopID = std::numeric_limits<TStopID>::max();

        struct SStop{
            virtual ~SStop(){};
            virtual TStopID ID() const noexcept = 0;
            virtual CStreetMap::TNodeID NodeID() const noexcept = 0;
            virtual std::string Description() const noexcept = 0;
            virtual std::string Description(const std::string &description) noexcept = 0;
        };

        struct SRoute{
            virtual ~SRoute(){};
            virtual std::string Name() const noexcept = 0;
            virtual std::size_t StopCount() const noexcept = 0;
            virtual TStopID GetStopID(std::size_t index) const noexcept = 0;
        };

        struct SPath{
            virtual ~SPath(){};
            virtual CStreetMap::TNodeID StartNodeID() const noexcept = 0;
            virtual CStreetMap::TNodeID EndNodeID() const noexcept = 0;
            virtual std::size_t NodeCount() const noexcept = 0;
            virtual CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept = 0;
        };

        virtual ~CBusSystem(){};

        virtual std::size_t StopCount() const noexcept = 0;
        virtual std::size_t RouteCount() const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0;
        virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept = 0;
        virtual std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept = 0;
};
```

## Details
- `using TStopID = uint64_t;` alias for stop IDs
- `inline static constexpr TStopID InvalidStopID = std::numeric_limits<TStopID>::max();` defines value for invalid stop IDs
- `struct SStop{};`: struct abstract interface for stops
    - `virtual ~SStop(){};`: destructor for SStop (included for good practice)
    - `virtual TStopID ID() const noexcept = 0;`: returns the stop ID of the stop
    - `virtual CStreetMap::TNodeID NodeID() const noexcept = 0;`: returns the node ID of the stop
    - `virtual std::string Description() const noexcept = 0;`: returns the description of the stop
    - `virtual std::string Description(const std::string &description) noexcept = 0;`: sets the description of the stop and returns it
        - description: description to set the stop to
- `struct SRoute{};`: struct abstract interface for routes
    - `virtual ~SRoute(){};`: destructor for SRoute (included for good practice)
    - `virtual std::string Name() const noexcept = 0;`: returns the name of the route
    - `virtual std::size_t StopCount() const noexcept = 0;` returns the number of stops on the route
    - `virtual TStopID GetStopID(std::size_t index) const noexcept = 0;` returns the stop ID specified by the index, or InvalidStopID if index is greater than or equal to StopCount()
        - index: index of stop to look for
- `struct SPath{};`: struct abstract interface for paths
    - `virtual ~SPath(){};` destructor for SPath (included for good practice)
    - `virtual CStreetMap::TNodeID StartNodeID() const noexcept = 0;`: returns the start node ID of the path
    - `virtual CStreetMap::TNodeID EndNodeID() const noexcept = 0;`: returns the end node ID of the path
    - `virtual std::size_t NodeCount() const noexcept = 0;` returns the node count in the path
    - `virtual CStreetMap::TNodeID GetNodeID(std::size_t index) const noexcept = 0;` returns the node specified by the index, or InvalidNodeID if index is out of range
        - index: index of node to look for
- `virtual ~CBusSystem(){};`: destructor for CBusSystem (included for good practice)
- `virtual std::size_t StopCount() const noexcept = 0;`: returns the number of stops in the system
- `virtual std::size_t RouteCount() const noexcept = 0;`: returns the number of routes in the system
- `virtual std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept = 0;`: returns the SStop specified by the index, or nullptr if index is greater than or equal to StopCount()
    - index: index of SStop to look for
- `virtual std::shared_ptr<SStop> StopByID(TStopID id) const noexcept = 0;`: returns the SStop specified by the stop ID, or nullptr if ID is not in the stops
    - id: ID of SStop to look for
- `virtual std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept = 0;`: returns the SRoute specified by the index, or nullptr if index is greater than or equal to RouteCount()
    - index: index of SRoute to look for
- `virtual std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept = 0;`: returns the SRoute specified by the name, or nullptr if name is not in the routes
    - name: name of SRoute to look for
- `virtual std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept = 0;`: returns the SPath connects the two stops, or nullptr if path doesn't exist
    - start: ID of the stop at which the path starts
    - end: ID of the stop at which the path ends