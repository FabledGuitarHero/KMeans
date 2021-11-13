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
    double price, minDist;
    int cluster;
    
    Point();
    Point(double price);
    double distance(Point p);
};

class Clusters {
public:
    std::vector<Point> points;
    std::vector<Point> centroid;
    double wsss;
    
    
    Clusters();
    Clusters(std::vector<double> data);
    Clusters(std::vector<std::vector<double>> data);
    
    void test_init(std::vector<double> data, int num_clust);
    
    
    void print_centroids();
    void print();
    std::string print_plots();
    
private:
    std::vector<Point> numPoints;
    
    
    void find_cluster_size(int range, std::vector<double> data, double dist);
    
    std::vector<Point> init_centroids(int num_clust, int size, std::vector<double> &data);
    void load_points(std::vector<double> data);
    void init_data_points();
    void update_centroids();
    void calc_wsss();
    std::vector<Point>::iterator check_distance(std::vector<Point>::iterator cluster_p,  std::vector<Point>::iterator point_p);
    template <class T>
    void init_vectors(std::vector<T> &vec);
};

#endif /* points_hpp */
