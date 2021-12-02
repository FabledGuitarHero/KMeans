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

std::vector<Clusters> KMeans::load_clusters(std::vector<std::vector<double>> &data, int h_l_opt){
    
    std::vector<Clusters> clusters;
    for (int i = 1; i < 25; i++){
        Clusters tmp;
        tmp.init(data[h_l_opt], i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}

//                main         index        label    data
KMeansMulti::KMeansMulti(std::vector<std::vector<std::vector<double>>> raw){
    
    std::vector<Clusters> high_clusters = load_clusters(raw[0], 0);
    std::vector<Clusters> low_clusters = load_clusters(raw[1], 1);
    
    unsigned long i = find_optimum_clusters(high_clusters);
    std::cout << "Nominal clusters is:" << i << std::endl;

    high = high_clusters.at(i-1);
    low = low_clusters.at(i-1);
}

KMeansMulti::KMeansMulti(nlohmann::json jsn){
    if (!jsn.is_object()){
        jsn = nlohmann::json::parse((std::string)jsn);
    }
   
    std::vector<std::vector<double>> data = to_vector(jsn["candles"]);
    std::cout << data.size() << std::endl;
    
    
    std::vector<Clusters> high_clusters = load_clusters(data, 0);
    std::vector<Clusters> low_clusters = load_clusters(data, 1);
    
    unsigned long i = find_optimum_clusters(high_clusters);
    std::cout << "Nominal clusters is:" << i << std::endl;

    high = high_clusters.at(i-1);
    low = low_clusters.at(i-1);
    
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


std::vector<std::vector<double>> KMeansMulti::to_vector(nlohmann::json &jsn){
    
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
     //std::cout << "Nominal clusters is:" << i << std::endl;

    clust = clust_vec[i-1]; //Offset by index 0
    //Clusters clust2 = load_clusters(data[0], (int)i);
}


KMeansSingle::KMeansSingle(nlohmann::json jsn, std::string index, int num_clust){
    if (!jsn.is_object()){
         jsn = nlohmann::json::parse((std::string)jsn);
     }
    
    std::cout << jsn["candles"].type_name() << std::endl;
    std::vector<double> data = this->to_vector(jsn["candles"], index);
     
    //std::cout << "Number of clusters is: " << num_clust << std::endl;
    
    //clust = load_clusters(data, num_clust);
    clust.init(data, num_clust);
}

std::vector<double> KMeansSingle::to_vector(nlohmann::json &jsn, std::string path){
    if(jsn.is_string())
        jsn = nlohmann::json::parse((std::string)jsn);
    
    std::vector<double> ret {};
    
    
    auto f = path.find('/');
    if(f != std::string::npos){
        
        for (auto i : jsn){
            std::string new_path = path.substr(f + 1,path.length());
            std::string cur_path = path.substr(0, f);

            std::vector<double> tmp = to_vector(i.at(cur_path), new_path);
            
            ret.insert(ret.end(), tmp.begin(), tmp.end());
        }
    }
    else{
        //subj obejct from jsn array, from code above.
        if(jsn.is_object()){
            return std::vector<double> {jsn[path]};
        }
        
        //Is an object and needs to be looped.
        for (auto i : jsn){
            ret.push_back(i[path]);
        }
    }

    
    return ret;
};
