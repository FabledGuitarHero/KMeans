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

struct KMeans_data{
public:
    std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> data;
    
    KMeans_data();
    KMeans_data(std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> raw);
};

class KMeans{
public:
    std::map<std::string, std::shared_ptr<Clusters>> clust;
    
    KMeans();
    KMeans(std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> raw);
    KMeans(KMeans_data raw);
    
    std::map<std::string, std::vector<std::vector<std::vector<double>>>> fetch_results();
    std::string                                                          print();
    
protected:
    void                                   init(std::map<std::string, std::pair<std::vector<double>,
                                                                                std::vector<double>>> &raw);
    std::vector<std::shared_ptr<Clusters>> load_clusters(std::pair<std::vector<double>, std::vector<double>> &data);
    unsigned long                          find_optimum_clusters(std::vector<std::shared_ptr<Clusters>> &clusters);
    
    std::vector<std::vector<double>>       fetch_data(std::vector<Point> &data);
    std::vector<std::vector<double>>       fetch_min_max(std::vector<std::vector<Point*>> &data);
    
    std::string                            print_clusters(std::shared_ptr<Clusters> clust);
    std::string                            print_centroids(std::shared_ptr<Clusters> clust);
    std::string                            print_min_max(std::vector<std::vector<Point*>> &min_max);
    std::string                            print_min_max(std::shared_ptr<Clusters> clust);
};

#endif /* KMeans_hpp */
