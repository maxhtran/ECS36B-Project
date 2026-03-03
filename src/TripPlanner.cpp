#include "TripPlanner.h"


struct CTripPlanner::SImplementation{
        SImplementation(std::shared_ptr<CBusSystem> bussystem){

        }

        ~SImplementation(){

        }
        
        std::shared_ptr<CBusSystemIndexer> BusSystemIndexer() const{
            return nullptr;
        }
        
        std::shared_ptr< SRoute > FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const{
            return nullptr;
        }
        
        std::shared_ptr< SRoute > FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const{
            return nullptr;
        }
        
        bool FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const{
            return false;
        }
        
        bool FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const{
            return false;
        }        
};


CTripPlanner::CTripPlanner(std::shared_ptr<CBusSystem> bussystem){
    DImplementation = std::make_unique<SImplementation>(bussystem);
}

CTripPlanner::~CTripPlanner(){

}

std::shared_ptr<CBusSystemIndexer> CTripPlanner::BusSystemIndexer() const{
    return DImplementation->BusSystemIndexer();
}

std::shared_ptr< CTripPlanner::SRoute > CTripPlanner::FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const{
    return DImplementation->FindDirectRouteLeaveTime(src,dest,leaveat);
}

std::shared_ptr< CTripPlanner::SRoute > CTripPlanner::FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const{
    return DImplementation->FindDirectRouteArrivalTime(src,dest,arriveby);
}

bool CTripPlanner::FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const{
    return DImplementation->FindRouteLeaveTime(src,dest,leaveat,plan);
}

bool CTripPlanner::FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const{
    return DImplementation->FindRouteArrivalTime(src,dest,arriveby,plan);
}

