//
//  KMeans.hpp
//  Kmeans
//
//  Created by Nate Henderson on 6/7/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#ifndef KMeans_hpp
#define KMeans_hpp

#include <stdio.h>
#include <fstream>
#include <map>
#include "points.hpp"

class KMeans{
public:
    KMeans();
    std::string print_plots();
    
protected:
    unsigned long find_optimum_clusters(std::vector<Clusters> &clusters);
    std::vector<Clusters> load_clusters(std::vector<std::vector<double>> &data, int h_l_opt);
    std::vector<Clusters> load_clusters(std::vector<double> &data);
    
    
    
    std::string print_clusters(Clusters &);
    std::string print_min_max(std::vector<std::vector<Point*>> &min_max);
};

class KMeansMulti : public KMeans{
public:
    Clusters high, low;                                             //Delete
    std::vector<std::vector<Point*>> min_max;
    std::map<std::string, Clusters> clust;
    
    KMeansMulti(nlohmann::json raw);                                //Delete
    KMeansMulti(std::map<std::string, std::vector<double>> raw);
    KMeansMulti(std::vector<std::vector<std::vector<double>>> raw); //Delete
    
    std::map<std::string, std::vector<std::vector<std::vector<double>>>> fetch_results();
    
    std::string print();
    
protected:
    std::vector<std::vector<double>> to_vector(nlohmann::json &jsn);
    void pair_high_low(Clusters &high, Clusters &low);
    std::vector<std::vector<double>>fetch_data(std::vector<Point> &data);
    std::vector<std::vector<double>>fetch_min_max(std::vector<std::vector<Point*>> &data);
    std::string print_plots();
    std::string print_min_max(Clusters &clust);
};


class KMeansSingle : public KMeans {
public:
    Clusters clust;
    std::vector<std::vector<Point*>> min_max;
    
    KMeansSingle(nlohmann::json jsn, std::string index);
    KMeansSingle(nlohmann::json jsn, std::string index, int num_clust);
    KMeansSingle(std::vector<std::vector<std::vector<double>>> raw);
    
    std::string print_plots();
    
protected:
    std::vector<double> to_vector(nlohmann::json &jsn, std::string index);
};



#endif /* KMeans_hpp */
