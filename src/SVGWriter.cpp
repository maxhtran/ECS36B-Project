#include "SVGWriter.h"
#include "svg.h"
#include <string>
#include <vector>
#include <iostream>

struct CSVGWriter::SImplementation{

    bool Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){

    }

    bool Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){

    }

    bool Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){

    }

    bool SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){

    }

    bool GroupBegin(const TAttributes &attrs){

    }

    bool GroupEnd(){

    }

};

CSVGWriter::CSVGWriter(std::shared_ptr< CDataSink > sink, TSVGPixel width, TSVGPixel height){
    DImplementation = std::make_unique<SImplementation>();
}

CSVGWriter::~CSVGWriter(){

}

bool CSVGWriter::Circle(const SSVGPoint &center, TSVGReal radius, const TAttributes &style){
    return DImplementation->Circle(center, radius,style);
}

bool CSVGWriter::Rectange(const SSVGPoint &topleft, const SSVGSize &size, const TAttributes &style){

}

bool CSVGWriter::Line(const SSVGPoint &start, const SSVGPoint &end, const TAttributes &style){

}

bool CSVGWriter::SimplePath(const std::vector<SSVGPoint> points, const TAttributes &style){

}

bool CSVGWriter::GroupBegin(const TAttributes &attrs){

}

bool CSVGWriter::GroupEnd(){

}