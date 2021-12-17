//
//  points.hpp
//  Kmeans
//
//  Created by Nate Henderson on 6/5/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#ifndef points_hpp
#define points_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include "json.hpp"

class Point {
public:
    double y_data, x_data, minDist;
    int cluster;
    
    Point();
    Point(double y_data);
    Point(double y_data, double x_data);
    ~Point();
    
    double y_distance(Point p);
};

class Clusters {
public:
    std::vector<Point> points;
    std::vector<Point> centroid;
    std::vector<std::vector<Point*>> min_max;
    double wsss;
    
    
    Clusters();
    ~Clusters();

    void init(const std::vector<double> y_data, const std::vector<double> x_data, int num_clust);
    
    
    void print_centroids();
    void print();
    
private:
    void find_cluster_size(int range, std::vector<double> data, double dist);
    
    std::vector<Point> init_centroids(int num_clust, const std::vector<Point> &data);
    void load_points(const std::vector<double> &y_data, const std::vector<double> &x_data);
    void init_data_points();
    void update_centroids();
    void calc_wsss();
    std::vector<Point>::iterator check_distance(std::vector<Point>::iterator cluster_p,  std::vector<Point>::iterator point_p);
};

bool sort_points(Point &p1, Point &p2);

#endif /* points_hpp */
