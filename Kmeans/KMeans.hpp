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
#include "points.hpp"

class KMeans{
public:
    KMeans();
    std::string print_plots();
    
protected:
    unsigned long find_optimum_clusters(std::vector<Clusters> &clusters);
    std::vector<Clusters> load_clusters(std::vector<std::vector<double>> &data, int h_l_opt);
};

class KMeansMulti : public KMeans{
public:
    Clusters high, low;
    
    KMeansMulti(nlohmann::json raw);
    KMeansMulti(std::vector<std::vector<double>> raw);
    KMeansMulti(std::vector<std::vector<std::vector<double>>> raw);
    
protected:
    std::vector<std::vector<double>> to_vector(nlohmann::json &jsn);
};


class KMeansSingle : public KMeans {
public:
    Clusters clust;
    
    KMeansSingle(nlohmann::json jsn, std::string index);
    KMeansSingle(nlohmann::json jsn, std::string index, int num_clust);
    KMeansSingle(std::vector<std::vector<std::vector<double>>> raw);
    
protected:
    std::vector<double> to_vector(nlohmann::json &jsn, std::string index);
    //Clusters load_clusters(std::vector<double> &data, int clust);
};



#endif /* KMeans_hpp */
