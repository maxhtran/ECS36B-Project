# CXMLReader Documentation

The CXMLReader class houses the code that can read and parse XML.

## Includes
- `<memory>`: provides std::unique_ptr and std::shared_ptr
- `"XMLEntity.h"`: provides SXMLEntity struct for the XML entity to be read/parsed
- `"DataSource.h"`: provides CDataSource type for input

## Class Declaration
```cpp
class CXMLReader {
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;

    public:
        CXMLReader(std::shared_ptr< CDataSource > src);
        ~CXMLReader();

        bool End() const;
        bool ReadEntity(SXMLEntity &entity, bool skipcdata = false);
}
```

## Details
- `struct SImplementation;`: implementation for the reading/parsing of XML
- `std::unique_ptr\<SImplementation\> DImplementation;`: (unique) pointer to `SImplementation`
- `CXMLReader(std::shared_ptr< CDataSource > src);`: class constructor for XML reader
    - src: pointer to data source
- `~CXMLReader();` class destructor
- `bool End() const;`: returns true if all XML entities have been read
- `bool ReadEntity(SXMLEntity &entity, bool skipcdata = false);`: returns true if the given entity is successfully read
    - &entity: pointer to the XML entity to be read
    - skipcdata: if true, only element-type entities will be returned