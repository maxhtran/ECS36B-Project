# CSVGWriter Documentation

The CSVGWriter class houses the code that can write SVG.

## Includes
- `<memory>`: provides std::unique_ptr and std::shared_ptr
- `<vector>`: provides std::vector
- `"DataSink.h"`: provides CDataSink type for output
- `"XMLEntity.h"`: provides TAttributes alias

## Typedefs
- `typedef int TSVGPixel;`: alias for size of SVG context
- `typedef double TSVGReal;`: alias for size, coordinate, and radius values
- `typedef TSVGReal TSVGCoordinate;`: alias for coordinate values specifically

## Structs
```cpp
struct SSVGPoint{
    TSVGCoordinate DX;
    TSVGCoordinate DY;
};
```
- for coordinate points on the SVG context

```cpp
struct SSVGSize{
    TSVGReal DWidth;
    TSVGReal DHeight;
};
```
- for rectangle sizes

## Class Declaration
```cpp
class CSVGWriter{
    private:
        struct SImplementation;
        std::unique_ptr<SImplementation> DImplementation;
        
    public:
        CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height);
        ~CSVGWriter();
        
        bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style);
        bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style);
        bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style);
        bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style);
        bool GroupBegin(const TAttributes &attrs);
        bool GroupEnd();

};
```

## Details
- `struct SImplementation;`: implementation for the writing of SVG
- `std::unique_ptr<SImplementation> DImplementation;`: (unique) pointer to `SImplementation`
- `CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height);`: class constructor for SVG writer
    - sink: pointer to output
    - width: width of SVG context
    - height: height of SVG context
- `~CSVGWriter();`: class destructor
- `bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style);`: creates an SVG circle
    - returns true on success, false on failure
    - &center: pointer to coordinates of the circle's center
    - radius: length of the circle's radius
    - &style: pointer to the circle's specified attributes
- `bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style);`: creates an SVG rectangle
    - returns true on success, false on failure
    - &center: pointer to coordinates of the rectangle's top left corner
    - &size: pointer to the rectangle's dimensions
    - &style: pointer to the rectangle's specified attributes
- `bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style);`: creates an SVG line
    - returns true on success, false on failure
    - &start: pointer to coordinates of the line's start point
    - &end: pointer to coordinates of the line's end point
    - &style: pointer to the line's specified attributes
- `bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style);`: creates a path made up of multiple SVG lines
    - returns true on success, false on failure
    - points: vector of coordinates of the lines' points
    - &style: pointer to the line's specified attributes
- `bool GroupBegin(const TAttributes &attrs);`: creates an SVG group
    - returns true on success, false on failure
    - &attrs: pointer the the group's specified attributes
- `bool GroupEnd();`: ends an SVG group
    - returns true on success, false on failure