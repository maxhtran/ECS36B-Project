# CStreetMapIndexer Documentation

The CStreetMapIndexer class indexes the CStreetMap provided for ease of lookup
of nodes and ways. It is somewhat of a helper class for other parts of the
project.

## Includes
- `StreetMap.h`: allows this class to deal with the given CStreetMap
- `<cstdint>`
- `<memory>`
- `<string>`
- `<utility>`
- `<limits>`
- `<unordered_set>`

## Class Declaration
```cpp
class CStreetMapIndexer{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        CStreetMapIndexer(std::shared_ptr<CStreetMap> streetmap);
        ~CStreetMapIndexer();
        std::size_t NodeCount() const noexcept;
        std::size_t WayCount() const noexcept;
        std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept;
        std::shared_ptr<CStreetMap::SWay> SortedWayByIndex(std::size_t index) const noexcept;
        std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept;
        std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysByNodeID(CStreetMap::TNodeID node) const noexcept;
};
```

## Details
- `struct SImplementation;`: implementation for the indexing of the given CStreetMap
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to `SImplementation`
- `CStreetMapIndexer(std::shared_ptr<CStreetMap> streetmap);`: class constructor for the `streetmap` indexer
- `~CStreetMapIndexer();`: class destructor
- `std::size_t NodeCount() const noexcept;`: returns the node count of the street map
- `std::size_t WayCount() const noexcept;`: returns the way count of the street map
- `std::shared_ptr<CStreetMap::SNode> SortedNodeByIndex(std::size_t index) const noexcept;`: returns a node sorted by the node ID
    - index: index of the node
- `std::shared_ptr<CStreetMap::SWay> SortedWayByIndex(std::size_t index) const noexcept;`: returns a way sorted by the way ID
    - index: index of the way
- `std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysInRange(const CStreetMap::SLocation &lowerleft, const CStreetMap::SLocation &upperright) const noexcept;`: returns the set of ways that have at least one node in range of `lowerleft` and `upperright`
- `std::unordered_set<std::shared_ptr<CStreetMap::SWay>> WaysByNodeID(CStreetMap::TNodeID node) const noexcept;`: returns the set of ways that have `node` in them