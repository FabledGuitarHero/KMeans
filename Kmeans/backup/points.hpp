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
    double price, time, minDist;
    int cluster;
    
    Point();
    Point(double price);
    Point(double price, double time);
    double distance(Point p);
};

class Clusters {
public:
    std::vector<Point> points;
    std::vector<Point> centroid;
    std::vector<Point> numPoints;
    std::vector<Point> priceSum;
    
    int k; //cluster count
    
    Clusters();
    Clusters(std::vector<double> data);
    Clusters(std::vector<std::vector<double>> data);
    void load_points(std::vector<double> data);
    void load_points(std::vector<std::vector<double>> data);
    int find_k_val(int k);
    void init_centroids(int k);
    void init_data_points();
    std::vector<Point>::iterator check_distance(std::vector<Point>::iterator cluster_p,  std::vector<Point>::iterator point_p);
    void update_centroids();
    void add_point(double data);
    void add_point(std::vector<double> data);
    void add_point(std::vector<std::vector<double>>);
    
    template <class T>
    void init_vectors(std::vector<T> &vec);
    
    long get_size();
    std::vector<std::vector<double>> get_cent_grid();
    
    void print_centroids();
    void print_by_clust();
    void print_pre_clust();
    void print();
    std::string print_plots();
};

#endif /* points_hpp */
