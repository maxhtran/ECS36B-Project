#include "TextTripPlanWriter.h"

struct CTextTripPlanWriter::SImplementation{

    SImplementation(std::shared_ptr<CBusSystem> bussystem){

    }

    ~SImplementation(){

    }

    std::shared_ptr<SConfig> Config() const{
        return nullptr;
    }

    bool WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
        return false;
    }
};



CTextTripPlanWriter::CTextTripPlanWriter(std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CTextTripPlanWriter::~CTextTripPlanWriter(){

}

std::shared_ptr<CTripPlanWriter::SConfig> CTextTripPlanWriter::Config() const{
    return DImplementation->Config();
}

bool CTextTripPlanWriter::WritePlan(std::shared_ptr<CDataSink> sink, const TTravelPlan &plan){
    return DImplementation->WritePlan(sink,plan);
}

