//
//  points.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/5/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include "points.hpp"

using json = nlohmann::json;

Point::Point(){
    price = 0.0;
    time = 0.0;
    cluster = -1;
    minDist = __DBL_MAX__;
}

Point::Point(double price){
    this->price = price;
    cluster = -1;
    minDist = __DBL_MAX__;
}

Point::Point(double price, double time){
    this->price = price;
    this->time = time;
    cluster = -1;
    minDist = __DBL_MAX__;
}

/*double Point::distance(Point p){
    //return (p.price - price) * (p.price - price);
    return (p.price - price);
}*/

double Point::distance(Point p){
    return (p.price - price) + (p.time - time);
    //return  (p.time - time);
    //return (p.price - price);
}

Clusters::Clusters(){
    points = {};
    centroid = {};
    centroid = {};
    numPoints = {};
    priceSum = {};
    
    k = 0;
}

Clusters::Clusters(std::vector<double> data){
    load_points(data);
    this->k = find_k_val(1);
    
    //replace with this line for highs/lows
   
    std::cout << "number of clusters: " << k << std::endl;
    init_centroids(k);
    init_data_points();
    
}

Clusters::Clusters(std::vector<std::vector<double>> data){
    load_points(data);
    this->k = find_k_val(1);
    //replace with this line for highs/lows
      
    std::cout << "number of clusters: " << k << std::endl;
    init_centroids(k);
    init_data_points();
    update_centroids(); //Rough estimate of clustoids
    update_centroids(); //Refines clustoids
}

void Clusters::load_points(std::vector<double> data){
    for(auto i : data){
        Point p = Point(i);
        points.push_back(p);
    }
}

void Clusters::load_points(std::vector<std::vector<double>> data){
    for (auto i : data){
        Point p = Point(i[1], i[0]);
        points.push_back(p);
    }
}

int Clusters::find_k_val(int k){
    int delta = 10;
    int return_k = k;
    
    if (points.size() / k > delta){
        return k = find_k_val(return_k + 1);
    }
    
    return return_k;
}

void Clusters::init_centroids(int k){
    unsigned long range = points.size() / k;
    
    for (unsigned long i = 0; i + range <= points.size();){
        Point *p = &points.at(i);
        p->cluster = (int)centroid.size();
        centroid.push_back(*p);
        i = i + range;
    }
};

void Clusters::init_data_points(){
    for (std::vector<Point>::iterator p = begin(points);
         p != end(points); p++){
        
        std::vector<Point>::iterator c = begin(centroid);
        c = check_distance(c, p);
        p->cluster = c->cluster;
        //p->minDist = p->time - c->time;
        p->minDist = c->distance(*p);
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

void Clusters::update_centroids(){
    std::vector<int> nPoints;
    std::vector<double> ysum, dist, xsum;
    init_vectors(nPoints);
    init_vectors(ysum);
    init_vectors(xsum);
    //init_vectors(dist);
    
    for(std::vector<Point>::iterator it = begin(points); it != end(points); it++){
        int clusterId = it->cluster;
        nPoints[clusterId] += 1;
        ysum[clusterId] += it->price;
        //xsum[clusterId] += floor(it->time*100)/100;
        xsum[clusterId] += it->time;
      
        //it->minDist = __DBL_MAX__;
        //dist[clusterId] += abs(it->minDist);
    }
    
    for(std::vector<Point>::iterator cent = begin(centroid); cent != end(centroid); cent++){
        int clusterId = cent->cluster;
        cent->price = ysum[clusterId] / nPoints[clusterId];
        cent->time = xsum[clusterId] / nPoints[clusterId];
        if(isnan(cent->time)){
            std::cout << "here" << std::endl;
        }
    }
    
    init_data_points();
}

template <class T>
void Clusters::init_vectors(std::vector<T> &vec){
    for(int i = 0; i < this->k; i++)
        vec.push_back(0);
}

void Clusters::add_point(double data){
    std::vector<double> tmp = {data};
    load_points(tmp);
    init_data_points();
}

void Clusters::add_point(std::vector<double> data){
    load_points(data);
    init_data_points();
}

void Clusters::add_point(std::vector<std::vector<double>> data){
    load_points(data);
    init_data_points();
}


long Clusters::get_size(){
    return centroid.size();
}

std::vector<std::vector<double>> Clusters::get_cent_grid(){
    std::vector<std::vector<double>> ret_arr;
    
    for (auto i : centroid){
        std::vector<double> tmp;
        tmp.push_back(i.time);
        tmp.push_back(i.price);
        ret_arr.push_back(tmp);
    }
    
    return ret_arr;
}


void Clusters::print_centroids(){
    for(auto &i : centroid){
        std::cout << i.price << " in cluster: " << i.cluster << std::endl;
    }
}

void Clusters::print_by_clust(){
    for(std::vector<Point>::iterator i = begin(centroid); i != end(centroid); i++){
        for(std::vector<Point>::iterator p = begin(points); p != end(points); p++){
            if (p->cluster == i->cluster){
                std::cout << p->price << " in cluster: " << i->cluster << std::endl;
            }
        }
    }
}

void Clusters::print_pre_clust(){
    for (auto i : points){
        std::cout << i.price << " time: " << i.time << " in cluster: " << i.cluster << std::endl;
    }
}

void Clusters::print(){
    for(std::vector<Point>::iterator i = begin(points); i != end(points); i++){
        
        auto n = std::prev(points.end(), std::distance(i, points.end())+1);
        if ((n->price < centroid.at(i->cluster).price  &&
             centroid.at(i->cluster).price < i->price) ||
             centroid.at(i->cluster).price == i->price) {
            std::cout << centroid.at(i->cluster).price <<" time: " << centroid.at(i->cluster).time << " in cluster: " << i->cluster << " <--- centroid" << std::endl;
        }
        
            std::cout << i->price << " time: " << i->time << " distance: " << i->minDist << " in cluster: " << i->cluster << std::endl;
        
    }
}

std::string Clusters::print_plots(){
    std::vector<std::vector<std::vector<double>>> ret_points;
    json str;
    
    for(auto i : points){
        //double clust = centroid[i.cluster].time;
        //json tmp = {i.price, centroid[i.cluster].time + i.minDist };
        json tmp = {i.price, i.time, i.cluster, i.minDist};
        str["data"].push_back(tmp);
    }
    
    for(auto i : centroid){
        if (!isnan(i.price)){
            json tmp = {i.price, i.time};
            str["centroid"].push_back(tmp);
            
        }
    }
    
    
    return str.dump();
}
