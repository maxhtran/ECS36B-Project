# CTripPlanWriter (Implementations) Documentation

The CTripPlanWriter class is an abstract class for three other classes:
CTextTripPlanWriter, CHTMLTripPlanWriter, and CSVGTripPlanWriter. These
classes will allow the travel plans to be displayed in different formats.

## Includes
- `TripPlanner.h`: allows these classes to access the travel plans
- `DataSink.h`: allows these classes to write data to a sink
- `<any>`
- `<memory>`
- `<string_view>`
- `<unordered_set>`
- `StreetMap.h` (CHTMLTripPlanWriter and CSVGTripPlanWriter): allows
these classes to access the street map

## Important Function Declarations
```cpp
std::shared_ptr<SConfig> Config() const override;

bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) override;
```

## Details
- `std::shared_ptr<SConfig> Config() const override;`: allows for modifying the output format
- `bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan) override;`: writes the travel `plan` to the `sink` and returns true if successful