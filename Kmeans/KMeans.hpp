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
#include <memory>
#include "points.hpp"

struct KMeansResult{
public:
    std::map<std::string, std::shared_ptr<Clusters>> data;
    
    KMeansResult();
};

class KMeans{
public:
    std::map<std::string, std::shared_ptr<Clusters>> clust;
    std::vector<std::vector<Point*>> min_max;
    
    KMeans();
    KMeans(std::map<std::string, std::vector<double>> raw);
    
    std::map<std::string, std::vector<std::vector<std::vector<double>>>> fetch_results();
    std::string print();
    
protected:
    std::vector<std::shared_ptr<Clusters>> load_clusters(std::vector<double> &data);
    unsigned long find_optimum_clusters(std::vector<std::shared_ptr<Clusters>> &clusters);
    void pair_high_low(Clusters &high, Clusters &low);
    
    std::vector<std::vector<double>>fetch_data(std::vector<Point> &data);
    std::vector<std::vector<double>>fetch_min_max(std::vector<std::vector<Point*>> &data);
    
    std::string print_clusters(std::shared_ptr<Clusters> clust);
    std::string print_centroids(std::shared_ptr<Clusters> clust);
    std::string print_min_max(std::vector<std::vector<Point*>> &min_max);
    std::string print_min_max(std::shared_ptr<Clusters> clust);
};

#endif /* KMeans_hpp */
