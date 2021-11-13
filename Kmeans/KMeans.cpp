//
//  KMeans.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/7/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include "KMeans.hpp"

KMeans::KMeans(){
}

//                main         index        label    data
KMeans::KMeans(std::vector<std::vector<std::vector<double>>> raw){
    
    std::vector<Clusters> high_clusters = load_clusters(raw[0], 0);
    std::vector<Clusters> low_clusters = load_clusters(raw[1], 1);
    
    unsigned long i = find_optimum_clusters(high_clusters);
    std::cout << "Nominal clusters is:" << i << std::endl;

    high = high_clusters.at(i-1);
    low = low_clusters.at(i-1);
}

KMeans::KMeans(nlohmann::json jsn){
    if (!jsn.is_object()){
        jsn = nlohmann::json::parse((std::string)jsn);
    }
   
    std::vector<std::vector<double>> data = to_vector(jsn["candles"]);
    
    
    std::vector<Clusters> high_clusters = load_clusters(data, 0);
    std::vector<Clusters> low_clusters = load_clusters(data, 1);
    
    unsigned long i = find_optimum_clusters(high_clusters);
    std::cout << "Nominal clusters is:" << i << std::endl;

    high = high_clusters.at(i-1);
    low = low_clusters.at(i-1);
    
}

std::vector<Clusters> KMeans::load_clusters(std::vector<std::vector<double>> &data, int h_l_opt){
    
    std::vector<Clusters> clusters;
    for (int i = 1; i < 10; i++){
        Clusters tmp;
        tmp.test_init(data[h_l_opt], i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}

// wss = within sum squares, sum of all the distances to the centroid
unsigned long KMeans::find_optimum_clusters(std::vector<Clusters> &clusters){
    std::vector<double> delta1, last;
    last.push_back(clusters.front().wsss);
    
    for (auto i : clusters){
        
        if(delta1.size() > 0){
            double tmp = last.back() - i.wsss;
            
            delta1.push_back(tmp);
            last.push_back(i.wsss);
            
            //ensures only two items are in vector to compare.
            if(delta1.size() > 2 || delta1.front() == 0)
                delta1.erase(delta1.begin());
            
            //this is your delta2
            if (delta1.front() - delta1.back() < 1)
                return i.centroid.size();
        }
        else{
            //initializes vector if empty with first wss
            delta1.push_back(i.wsss);
        }
    }
    
    //Needs some form of error handling here
    return 0;
}


std::vector<std::vector<double>> KMeans::to_vector(nlohmann::json &jsn){
    
    std::vector<std::vector<double>> tmp;
    std::vector<double> high, low;
    
    for(auto i : jsn){
        double tmp_high, tmp_low;
        tmp_high = i["high"];
        high.push_back(tmp_high);
        
        tmp_low = i["low"];
        low.push_back(tmp_low);
    }
    
    tmp.push_back(high);
    tmp.push_back(low);
    
    return tmp;
}



/*
 * Class : KMeansSingle
 *    + KMeans for single data sources
 */

KMeansSingle::KMeansSingle(nlohmann::json jsn, std::string index){
    if (!jsn.is_object()){
         jsn = nlohmann::json::parse((std::string)jsn);
     }
    
    std::vector<std::vector<double>> data {};
    data.push_back(this->to_vector(jsn["candles"], index));
    
    std::vector<Clusters> clust_vec = KMeans::load_clusters(data, 0);
     
     unsigned long i = find_optimum_clusters(clust_vec);
     std::cout << "Nominal clusters is:" << i << std::endl;

    clust = load_clusters(data[0], (int)i);
    std::cout << clust.print_plots() << std::endl;
}


KMeansSingle::KMeansSingle(nlohmann::json jsn, std::string index, int num_clust){
    if (!jsn.is_object()){
         jsn = nlohmann::json::parse((std::string)jsn);
     }
    
    std::vector<double> data = this->to_vector(jsn["candles"], index);
     
     
    std::cout << "Number of clusters is: " << num_clust << std::endl;
    
    clust = load_clusters(data, num_clust);
    std::cout << clust.print_plots() << std::endl;
}


KMeansSingle::KMeansSingle(std::vector<std::vector<std::vector<double>>> raw){
    
}

Clusters KMeansSingle::load_clusters(std::vector<double> &data, int clust){
    
    Clusters tmp;
    tmp.test_init(data, clust);
    
    return tmp;
}


std::vector<double> KMeansSingle::to_vector(nlohmann::json &jsn, std::string index){
    std::vector<double> tmp {};
    for (auto i : jsn)
        tmp.push_back(i[index+"_data"][index]);
    
    return tmp;
};
