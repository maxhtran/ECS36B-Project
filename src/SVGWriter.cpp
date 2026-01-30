#include "SVGWriter.h"
#include "svg.h"
#include <string>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;

struct CSVGWriter::SImplementation{
    std::shared_ptr<CDataSink> DSink;
    svg_context_ptr DContext;

    static svg_return_t WriteFunction(svg_user_context_ptr user, const char *text) { // Writes SVG text to sink
        if(!user || !text) return SVG_ERR_NULL;
        SImplementation *Implementation = (SImplementation *)user;
        while (*text) {
            if(!Implementation->DSink->Put(*text)) return SVG_ERR_IO;
            text++;
        }
        return SVG_OK;
    }

    static svg_return_t CleanupFunction(svg_user_context_ptr user){ // Cleanup callback
        return user ? SVG_OK : SVG_ERR_NULL;
    }

    SImplementation(std::shared_ptr<CDataSink> sink, TSVGPixel width, TSVGPixel height) { // Initialize SVG writer
        DSink = sink;
        DContext = svg_create(WriteFunction, CleanupFunction, this, width, height);
    }

    ~SImplementation(){ // Destroy SVG context on cleanup
        if (DContext) svg_destroy(DContext);
    };

    std::string CreateStyleString(const TAttributes &style) { // Convert attributes to SVG style string
        size_t size = style.size();
        if (size == 0) {
            return "";
        }
        std::string string;
        for (size_t i = 0; i < size; i++) {
            if (i == 0) {
                string = style[i].first + ":" + style[i].second;
                if (i != (size - 1)) {
                    string = string + ";";
                }
            }
            else if(i != (size - 1)){
                string = string + style[i].first + ":" + style[i].second + ";";
            }
            else {
                string = string + style[i].first + ":" + style[i].second;
            }
        }
        return string;
    }

    bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){ // Draw SVG circle
        svg_point_t Center{center.DX, center.DY}; 
        std::string Style = CreateStyleString(style);
        return svg_circle(DContext, &Center, radius, Style.c_str()) == SVG_OK;
        
    }

    bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){ // Draw SVG rectangle
        svg_point_t Point = {topleft.DX, topleft.DY};
        svg_size_t Size = {size.DWidth, size.DHeight};
        std::string Style = CreateStyleString(style);
        return svg_rect(DContext, &Point, &Size, Style.c_str()) == SVG_OK;
    }

    bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){ // Draw SVG line
        svg_point_t Point1 = {start.DX, start.DY};
        svg_point_t Point2 = {end.DX, end.DY};
        std::string Style = CreateStyleString(style);
        return svg_line(DContext, &Point1, &Point2, Style.c_str()) == SVG_OK;
    }

    bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){ // Draw connected line segments
        if (points.size() < 2) {
            return false;
        }
        for(size_t i = 0; i + 1 < points.size(); ++i) {
            if(!Line(points[i], points[i+1], style)){
                return false;
            }
        }
        return true;  
    }

    size_t numGroups = 0; // Tracks open SVG groups

    bool GroupBegin(const TAttributes &attrs){ // Begin SVG group
        std::string Style = CreateStyleString(attrs);
        const char *Arg = Style.empty() ? nullptr : Style.c_str();
        if(svg_group_begin(DContext, Arg) == SVG_OK) {
            numGroups++;
            return true;
        }
        return false;
    }

    bool GroupEnd(){  // End SVG group
        if(numGroups < 1) return false;
        if(svg_group_end(DContext) == SVG_OK) {
            numGroups--;
            return true;
        }
        return false;
    }

};

CSVGWriter::CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){
    DImplementation = std::make_unique<SImplementation>(sink, width, height);
}

CSVGWriter::~CSVGWriter(){

}

bool CSVGWriter::Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
    return DImplementation->Circle(center, radius,style);
}

bool CSVGWriter::Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){
    return DImplementation->Rectange(topleft, size, style);
}

bool CSVGWriter::Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){
    return DImplementation->Line(start, end, style);
}

bool CSVGWriter::SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){
    return DImplementation->SimplePath(points, style);
}

bool CSVGWriter::GroupBegin(const TAttributes &attrs){
    return DImplementation->GroupBegin(attrs);
}

bool CSVGWriter::GroupEnd(){
    return DImplementation->GroupEnd();
}