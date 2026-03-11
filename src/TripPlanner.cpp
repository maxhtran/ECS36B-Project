#include "TripPlanner.h"
#include <unordered_map>

struct CTripPlanner::SImplementation{
    std::shared_ptr<CBusSystemIndexer> DBusSystemIndexer;
    
    SImplementation(std::shared_ptr<CBusSystem> bussystem){
        DBusSystemIndexer = std::make_shared<CBusSystemIndexer>(bussystem);
    }

    ~SImplementation(){

    }
        
    std::shared_ptr<CBusSystemIndexer> BusSystemIndexer() const{
        return DBusSystemIndexer;
    }
        
    std::shared_ptr< SRoute > FindDirectRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat) const{
        // find all routes with src and dest
        std::unordered_set<std::string> src_dest_routes;
        if (!DBusSystemIndexer->RoutesByStopIDs(src, dest, src_dest_routes)) { 
            return nullptr; // return nullptr if none are found
        }

        // for each route with src and dest, go through the stop times of their src stops to find which ones leave src at the leaveat time
        std::unordered_map<std::shared_ptr<CBusSystemIndexer::SRouteIndexer>, size_t> leaveat_matches;
        for (std::string route : src_dest_routes) { 
            auto route_indexer = DBusSystemIndexer->RouteByName(route);
            size_t src_index = route_indexer->FindStopIndex(src);

            for (int trip_index = 0; trip_index < static_cast<int>(route_indexer->TripCount()); trip_index++) {
                if (route_indexer->GetStopTime(src_index, trip_index).to_duration() >= leaveat.to_duration()) {
                    leaveat_matches[route_indexer] = trip_index;
                    break;
                }
            }
        }

        if (leaveat_matches.empty()) { // if no routes with src and dest leave src at the leaveat time, return nullptr
            return nullptr; 
        }

        // find the route that gets to dest the earliest
        std::shared_ptr<CBusSystemIndexer::SRouteIndexer> min_dest_time_route = leaveat_matches.begin()->first; 
        size_t mdtr_dest_index = min_dest_time_route->FindStopIndex(dest);
        size_t mdtr_trip_index = leaveat_matches.begin()->second;
        for (auto match : leaveat_matches) {
            std::shared_ptr<CBusSystemIndexer::SRouteIndexer> cur_route = match.first;
            size_t cur_dest_index = cur_route->FindStopIndex(dest);
            size_t cur_trip_index = match.second;

            if (cur_route->GetStopTime(cur_dest_index, cur_trip_index).to_duration() < min_dest_time_route->GetStopTime(mdtr_dest_index, mdtr_trip_index).to_duration()) {
                min_dest_time_route = cur_route;
                mdtr_dest_index = cur_dest_index;
                mdtr_trip_index = cur_trip_index;
            }
        }

        return min_dest_time_route;
    }
        
    std::shared_ptr< SRoute > FindDirectRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby) const{
        // find all routes with src and dest
        std::unordered_set<std::string> src_dest_routes;
        if (!DBusSystemIndexer->RoutesByStopIDs(src, dest, src_dest_routes)) { 
            return nullptr; // return nullptr if none are found
        }

        // for each route with src and dest, go through the stop times of their dest stops to find which ones get to dest at the arriveby time
        std::unordered_map<std::shared_ptr<CBusSystemIndexer::SRouteIndexer>, size_t> arriveby_matches;
        for (std::string route : src_dest_routes) { 
            auto route_indexer = DBusSystemIndexer->RouteByName(route);
            size_t dest_index = route_indexer->FindStopIndex(dest);

            for (int trip_index = 0; trip_index < static_cast<int>(route_indexer->TripCount()); trip_index++) {
                if (route_indexer->GetStopTime(dest_index, trip_index).to_duration() <= arriveby.to_duration()) {
                    arriveby_matches[route_indexer] = trip_index;
                    break;
                }
            }
        }

        if (arriveby_matches.empty()) { // if no routes with src and dest get to dest at the arriveby time, return nullptr
            return nullptr; 
        }

        // find the route that leaves src the latest
        std::shared_ptr<CBusSystemIndexer::SRouteIndexer> max_src_time_route = arriveby_matches.begin()->first; 
        size_t mstr_src_index = max_src_time_route->FindStopIndex(src);
        size_t mstr_trip_index = arriveby_matches.begin()->second;
        for (auto match : arriveby_matches) {
            std::shared_ptr<CBusSystemIndexer::SRouteIndexer> cur_route = match.first;
            size_t cur_src_index = cur_route->FindStopIndex(src);
            size_t cur_trip_index = match.second;

            if (cur_route->GetStopTime(cur_src_index, cur_trip_index).to_duration() > max_src_time_route->GetStopTime(mstr_src_index, mstr_trip_index).to_duration()) {
                max_src_time_route = cur_route;
                mstr_src_index = cur_src_index;
                mstr_trip_index = cur_trip_index;
            }
        }

        return max_src_time_route;
    }
        
    bool FindRouteLeaveTime(TStopID src, TStopID dest, TStopTime leaveat, TTravelPlan &plan) const{
        plan.clear(); // ensure that plan is clear

        // find the correct route and get its indexer
        std::shared_ptr<SRoute> route = FindDirectRouteLeaveTime(src, dest, leaveat);
        if (!route) { // if no route matches the parameters, return false
            return false;
        }
        std::shared_ptr<CBusSystemIndexer::SRouteIndexer> route_indexer = DBusSystemIndexer->RouteByName(route->Name());
    
        // find out which of the route's trips leaves at the leaveat time
        size_t src_stop_index = route_indexer->FindStopIndex(src);
        size_t trip_index;
        for (trip_index = 0; trip_index < route_indexer->TripCount(); trip_index++) {
            if (route_indexer->GetStopTime(src_stop_index, trip_index).to_duration() >= leaveat.to_duration()) {
                break;
            }
        }

        // get all of the stops between src and dest
        std::vector<TStopID> stops_in_between = route_indexer->StopIDsSourceDestination(src, dest);

        // build travel plan
        plan.push_back({route_indexer->GetStopTime(route_indexer->FindStopIndex(src), trip_index), src, route->Name()});
        for (TStopID stop : stops_in_between) {
            size_t cur_stop_index = route_indexer->FindStopIndex(stop);
            TStopTime time = route_indexer->GetStopTime(cur_stop_index, trip_index);

            plan.push_back({time, stop, route->Name()});
        }
        plan.push_back({route_indexer->GetStopTime(route_indexer->FindStopIndex(dest), trip_index), dest, ""});

        return true;
    }
        
    bool FindRouteArrivalTime(TStopID src, TStopID dest, TStopTime arriveby, TTravelPlan &plan) const{
        plan.clear(); // ensure that plan is clear

        // find the correct route and get its indexer
        std::shared_ptr<SRoute> route = FindDirectRouteArrivalTime(src, dest, arriveby);
        if (!route) { // if no route matches the parameters, return false
            return false;
        }
        std::shared_ptr<CBusSystemIndexer::SRouteIndexer> route_indexer = DBusSystemIndexer->RouteByName(route->Name());
    
        // find out which of the route's trips arrives by the arriveby time
        size_t dest_stop_index = route_indexer->FindStopIndex(dest);
        size_t trip_index;
        for (trip_index = 0; trip_index < route_indexer->TripCount(); trip_index++) {
            if (route_indexer->GetStopTime(dest_stop_index, trip_index).to_duration() <= arriveby.to_duration()) {
                break;
            }
        }

        // get all of the stops between src and dest
        std::vector<TStopID> stops_in_between = route_indexer->StopIDsSourceDestination(src, dest);

        // build travel plan
        plan.push_back({route_indexer->GetStopTime(route_indexer->FindStopIndex(src), trip_index), src, route->Name()});
        for (TStopID stop : stops_in_between) {
            size_t cur_stop_index = route_indexer->FindStopIndex(stop);
            TStopTime time = route_indexer->GetStopTime(cur_stop_index, trip_index);

            plan.push_back({time, stop, route->Name()});
        }
        plan.push_back({route_indexer->GetStopTime(dest_stop_index, trip_index), dest, ""});

        return true;
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

