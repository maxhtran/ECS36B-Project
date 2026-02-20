# COpenStreetMap Documentation

The COpenStreetMap class houses the code that can read and represent OpenStreetMap (OSM) XML data.

Includes

- `"OpenStreetMap.h"`: provides the COpenStreetMap class definition and CStreetMap interface

- `<unordered_map>`: provides std::unordered_map for fast ID lookup

- `<vector>`: provides std::vector for ordered storage of nodes and ways

- `<string>`: provides std::string for tag names and attribute values

- `<utility>`: provides std::pair for temporary attribute storage while parsing

## Class Declaration
```cpp
class COpenStreetMap : public CStreetMap{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        COpenStreetMap(std::shared_ptr< CXMLReader > src);
        ~COpenStreetMap();

        std::size_t NodeCount() const noexcept override;
        std::size_t WayCount() const noexcept override;

        std::shared_ptr<SNode> NodeByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SNode> NodeByID(TNodeID id) const noexcept override;

        std::shared_ptr<SWay> WayByIndex(std::size_t index) const noexcept override;
        std::shared_ptr<SWay> WayByID(TWayID id) const noexcept override;
};
```

## Details
- `struct SImplementation;`: implementation for the reading and parsing of the OpenStreetMap XML file
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to SImplementation
- `COpenStreetMap(std::shared_ptr< CXMLReader > src);`: class constructor for the OpenStreetMap reader
    - src: pointer to the XML reader providing OSM data
- `~COpenStreetMap();`: class destructor
- `std::size_t NodeCount() const noexcept override;`: returns the number of nodes in the map
- `std::size_t WayCount() const noexcept override;`: returns the number of ways in the map
- `std::shared_ptr<SNode> NodeByIndex(std::size_t index) const noexcept override;`: returns the SNode specified by the index, or nullptr if index is greater than or equal to NodeCount()
    - index: index of SNode to look for
- `std::shared_ptr<SNode> NodeByID(TNodeID id) const noexcept override;`: returns the SNode specified by the node ID, or nullptr if ID is not in the nodes
    - id: ID of SNode to look for
- `std::shared_ptr<SWay> WayByIndex(std::size_t index) const noexcept override;`: returns the SWay specified by the index, or nullptr if index is greater than or equal to WayCount()
    - index: index of SWay to look for
- `std::shared_ptr<SWay> WayByID(TWayID id) const noexcept override;`: returns the SWay specified by the way ID, or nullptr if ID is not in the ways
    - id: ID of SWay to look for