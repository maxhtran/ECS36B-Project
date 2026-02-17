# CXMLBusSystem Documentation

The CXMLBusSystem class inherits from the abstract CBusSystem class, implementing
its functions in a way that can handle XML format.

## Includes
- `BusSystem.h`: provides the CBusSystem abstract class to inherit from
- `XMLReader.h`: provides the XML functionality

## Class Declaration
```cpp
class CXMLBusSystem : public CBusSystem{
    private:
        struct SImplementation;
        std::unique_ptr< SImplementation > DImplementation;
    public:
        CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource);
        ~CXMLBusSystem();

        std::size_t StopCount() const noexcept override;
        std::size_t RouteCount() const noexcept override;
        std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SStop> StopByID(TStopID id) const noexcept override;
        std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept override;
        std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept override;
};
```

## Details
- `struct SImplementation;`: implementation for the reading and parsing of the XML bus system
- `std::unique_ptr< SImplementation > DImplementation;`: (unique) pointer to `SImplementation`
- `CXMLBusSystem(std::shared_ptr< CXMLReader > systemsource, std::shared_ptr< CXMLReader > pathsource);`: class constructor for the XML bus system
    - systemsource: pointer to the XML reader for the bus system information
    - pathsource: pointer to the XML reader for the path information
- `~CXMLBusSystem();` class destructor
- `std::size_t StopCount() const noexcept override;`: returns the number of stops in the system
- `std::size_t RouteCount() const noexcept override;`: returns the number of routes in the system
- `std::shared_ptr<SStop> StopByIndex(std::size_t index) const noexcept override;`: returns the SStop specified by the index, or nullptr if index is greater than or equal to StopCount()
    - index: index of SStop to look for
- `std::shared_ptr<SStop> StopByID(TStopID id) const noexcept override;`: returns the SStop specified by the stop ID, or nullptr if ID is not in the stops
    - id: ID of SStop to look for
- `std::shared_ptr<SRoute> RouteByIndex(std::size_t index) const noexcept override;`: returns the SRoute specified by the index, or nullptr if index is greater than or equal to RouteCount()
    - index: index of SRoute to look for
- `std::shared_ptr<SRoute> RouteByName(const std::string &name) const noexcept override;`: returns the SRoute specified by the name, or nullptr if name is not in the routes
    - name: name of SRoute to look for
- `std::shared_ptr<SPath> PathByStopIDs(TStopID start, TStopID end) const noexcept override;`: returns the SPath connects the two stops, or nullptr if path doesn't exist
    - start: ID of the stop at which the path starts
    - end: ID of the stop at which the path ends