//
//  points.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/5/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include "points.hpp"

using json = nlohmann::json;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Points Class:                                                        *
 *     Classifies raw data as a data point for a cluster.                *
 *                                                                       *
 *     Contains:                                                         *
 *        Price (aka Data): double                                       *
 *        Cluster: The assigned cluster the point belongs to             *
 *        minDist: Distance from the point to the center of the cluster. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Point::Point() : price(0.0), cluster(-1), minDist(__DBL_MAX__){}

Point::Point(double price) : price(price), cluster(-1), minDist(__DBL_MAX__){}


double Point::distance(Point p){
    return (p.price - price);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Clusters Class:                                                      *
 *     Collection of similar points                                      *
 *                                                                       *
 *     Contains:                                                         *
 *        Points: vector of points assigned to the cluster               *
 *        Centroid: point at the center of the cluster                   *
 *        numPoints: number of points inside the cluster                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Clusters::Clusters() : points({}), centroid({}), numPoints({}), min_max({}){}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  1) Inialize points from raw data                                     *
 *  2) Establish initial centroids, clusters                             *
 *  3) Load points and sort into established clusters                    *
 *  4) Repeatedly re-sort clusters/points, recalculates centroid to      *
 *        establish accurate centroid point.                             *
 *  5) Calculate within sum squared                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Clusters::init(std::vector<double> data, int num_clust){
    load_points(data);
    sort(data.begin(), data.end());
    int range = data[data.size()-1] - data[0];
    
    centroid = init_centroids(num_clust, range, data);
    init_data_points();
    
    for (int i = 0; i < 20; i ++){
         update_centroids();
    }

    calc_wsss();
}


std::vector<Point> Clusters::init_centroids(int num_clust, int size, std::vector<double> &data){
    std::vector<Point> return_cent;
    unsigned long range = data.size() / num_clust;
    
    for (unsigned long i = range; i <= data.size()+1;){
        Point p(data[i-1]);
        p.cluster = (int)return_cent.size();
        return_cent.push_back(p);
        min_max.push_back(std::vector<Point*>{&p, &p});
        
        i += range;
    }
    
    return return_cent;
};


void Clusters::load_points(std::vector<double> data){
    for(auto i : data){
        Point p(i);
        points.push_back(p);
    }
}

std::vector<Point>::iterator Clusters::check_distance(std::vector<Point>::iterator cluster_p, std::vector<Point>::iterator point_p){
    std::vector<Point>::iterator current = cluster_p;
    cluster_p++;
    
    if (cluster_p == end(centroid))
        return current;
    
    double distance = current->distance(*cluster_p)/2.0;
    if(current->distance(*point_p) > distance)
        current = check_distance(cluster_p, point_p);
    
    return current;
}


void Clusters::calc_wsss(){
   double total = 0;
    
    for (auto i : points)
        total += abs(i.minDist);
 
    wsss = total;
}



void Clusters::init_data_points(){
    for (std::vector<Point>::iterator p = begin(points); p != end(points); p++){
        
        /* * * * * * * * * * * * * * * * * * * * * *
         *  Calculates new distance from centroid  *
         * * * * * * * * * * * * * * * * * * * * * */
        std::vector<Point>::iterator c = begin(centroid);
        c = check_distance(c, p);
        p->cluster = c->cluster;
        p->minDist = c->distance(*p);
        
        /* * * * * * * * * * * * * * * * * * * * *
         *  Updates clusters min, max distance   *
         *  (min_max[x][0] is max)               *
         *  (min_max[x][1] is min)               *
         * * * * * * * * * * * * * * * * * * * * */
        Point& pnt = *p;
        
        if (p->minDist > min_max[p->cluster][0]->minDist)
            min_max[p->cluster][0] = &pnt;
        if (p->minDist < min_max[p->cluster][1]->minDist)
            min_max[p->cluster][1] = &pnt;
    }
}



void Clusters::update_centroids(){
    std::vector<int> nPoints {std::vector<int>((int)centroid.size(), 0)};
    std::vector<double> ysum {std::vector<double>(centroid.size(), 0)};
    //ysum = dist = xsum;
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * 1) Iterates all points in vector                        *
     * 2) Updates point count in nPoints at points cluster ID  *
     * 3) Updates the total sum of points at clusterID         *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    for(std::vector<Point>::iterator it = begin(points); it != end(points); it++){
        int clusterId = it->cluster;
        nPoints[clusterId] += 1;
        ysum[clusterId] += it->price;
    }
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  1) Iterates centroid vector                            *
     *  2) Gets the clusterID at current cluster               *
     *  3) calcualtes the current locatoin based on the sum of *
     *     points/number of points                             *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    for(std::vector<Point>::iterator cent = begin(centroid); cent != end(centroid); cent++){
        int clusterId = cent->cluster;
        cent->price = ysum[clusterId] / nPoints[clusterId];

    }
    
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *  Calls to recalculate point's distance from new centroid  *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    init_data_points();
}

void Clusters::print_centroids(){
    for(auto &i : centroid){
        std::cout << i.price << " for cluster: " << i.cluster << std::endl;
    }
}

void Clusters::print(){
    for(std::vector<Point>::iterator i = begin(points); i != end(points); i++){
        std::cout << i->price << " distance: " << i->minDist << " in cluster: " << i->cluster << std::endl;
    }
}

std::string Clusters::print_high_low(){
    json str;
    
    for (auto i : min_max){
        json tmp = {i[0]->price, i[1]->price};
        str["min_max"].push_back(tmp);
    }
    
    return str.dump();
}

std::string Clusters::print_plots(){
    std::vector<std::vector<std::vector<double>>> ret_points;
    json str;
    
    for(auto i : points){
        json tmp = {i.price, i.cluster, i.minDist};
        str["data"].push_back(tmp);
    }
    
    
    for(auto i : centroid){
        if (!isnan(i.price)){
            json tmp = {i.price};
            str["centroid"].push_back(tmp);
        }
    }
    

    return str.dump();
}
