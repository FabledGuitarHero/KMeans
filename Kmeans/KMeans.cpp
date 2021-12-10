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

/* | Get ready to remove this | *
 * V                          V */
std::vector<Clusters> KMeans::load_clusters(std::vector<std::vector<double>> &data, int h_l_opt){
    
    std::vector<Clusters> clusters;
    for (int i = 1; i < 25; i++){
        Clusters tmp;
        tmp.init(data[h_l_opt], i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}
/* ^ Get ready to remove this ^ *
 * |                          | */

std::vector<Clusters> KMeans::load_clusters(std::vector<double> &data){
    
    std::vector<Clusters> clusters;
    for (int i = 1; i < 25; i++){
        Clusters tmp;
        tmp.init(data, i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}


std::string KMeans::print_clusters(Clusters &clust){
    std::stringstream ss;
    int str_len = 25;
    
    ss << "[Point/Cluster/Distance]" << std::endl;
    
    int row_count = 0;
    for (auto i : clust.points){
        std::stringstream tmp;
        
        tmp << "[" << i.price << ", " << i.cluster << ", " << i.minDist << "]";
        tmp << std::setw(str_len - (int)tmp.str().length()) << " ";
        
        row_count++;
        if (row_count % 5 == 0)
            tmp << "\n";
        
        ss << tmp.str();
    }
    
    return ss.str();
}

std::string KMeans::print_min_max(std::vector<std::vector<Point*>> &min_max){
    std::stringstream ss;
    
    int str_len = 20;
    
    int row_count = 0;
    for (auto i : min_max){
        std::stringstream tmp;
        
        tmp << "[" << i[0]->price << ", " << i[1]->price << "]";
        tmp << std::setw(str_len - (int)tmp.str().length()) << " ";
        
        row_count++;
        if (row_count % 3 == 0)
            tmp << "\n";
        
        ss << tmp.str();
    }
    
    return ss.str();
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

KMeansMulti::KMeansMulti(std::map<std::string, std::vector<double>> raw){
    std::map<std::string, std::vector<Clusters>> clusters_cache;
    
    //Switch to iterator here
    for (auto i : raw){
        clusters_cache.insert(std::pair<std::string, std::vector<Clusters>>(i.first, load_clusters(i.second)));
    }
    
    for (auto i : clusters_cache){
        unsigned long opt_clust = find_optimum_clusters(i.second);
        clust.insert(std::pair<std::string, Clusters>(i.first, clusters_cache.at(i.first)[opt_clust-1]));
    }
    
}

KMeansMulti::KMeansMulti(nlohmann::json jsn){
    if (!jsn.is_object()){
        jsn = nlohmann::json::parse((std::string)jsn);
    }
   
    std::vector<std::vector<double>> data = to_vector(jsn["candles"]);
    
    
    std::vector<Clusters> high_clusters = load_clusters(data, 0);
    std::vector<Clusters> low_clusters = load_clusters(data, 1);
    
    unsigned long i = find_optimum_clusters(high_clusters);

    high = high_clusters.at(i-1);
    low = low_clusters.at(i-1);
    
    pair_high_low(high, low);
    
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

void KMeansMulti::pair_high_low(Clusters &high, Clusters &low){
    for (int i = 0; i < low.min_max.size(); i++){
        this->min_max.push_back({high.min_max[i][0], low.min_max[i][1]});
    }
}

std::map<std::string, std::vector<std::vector<std::vector<double>>>> KMeansMulti::fetch_results(){
    std::map<std::string, std::vector<std::vector<std::vector<double>>>> ret {};
    
    for (auto i : this->clust){
        std::vector<std::vector<std::vector<double>>> tmp;
        
        tmp.push_back(fetch_data(i.second.points));
        tmp.push_back(fetch_data(i.second.centroid));
        tmp.push_back(fetch_min_max(i.second.min_max));
        
        ret.insert(std::pair<std::string, std::vector<std::vector<std::vector<double>>>>(i.first, tmp));
    }
    
    return ret;
};

std::vector<std::vector<double>> KMeansMulti::fetch_data(std::vector<Point> &data){
    std::vector<std::vector<double>> ret;
    for(auto i : data){
        std::vector<double> tmp {i.price, (double)i.cluster, i.minDist};
        ret.push_back(tmp);
    }
    return ret;
}

std::vector<std::vector<double>> KMeansMulti::fetch_min_max(std::vector<std::vector<Point*>> &data){
    std::vector<std::vector<double>> ret;
    for (auto i : data)
        ret.push_back(std::vector<double> {i[0]->price, i[1]->price});
    
    return ret;
}

/*                                  *
 *  |  Get ready to remove this:  | *
 *  V                             V */
std::string KMeansMulti::print_plots(){
    std::stringstream ss;
    
    ss << print_clusters(this->high) <<"\n";
    ss << print_clusters(this->low) << "\n";
    ss << "Cluster Min/Max: " << "\n";
    ss << KMeans::print_min_max(this->min_max);
    
    return ss.str();
}

/* ^                           ^ *
 * | Get ready to remove this: | *
 *                               */

std::string KMeansMulti::print(){
    std::stringstream ss;
    
    for (auto i : this->clust){
        ss << "Data Source: " << i.first << std::endl;
        ss << print_clusters(i.second);
        ss << this->print_min_max(i.second);
    }
    
    return ss.str();
}

std::string KMeansMulti::print_min_max(Clusters &clust){
    std::stringstream ss;
    int str_len = 20;
    
    ss << "\n[Max/Min] \n";
    for (auto i : clust.min_max){
        ss << "[" << i[0]->price<< ", " << i[1]->price << "]";
        ss << std::setw(str_len - (int)ss.str().length()) << " ";
    }
    
    return ss.str();
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

    clust = clust_vec[i-1]; //Offset by index 0
    
    min_max = clust.min_max;
}


KMeansSingle::KMeansSingle(nlohmann::json jsn, std::string index, int num_clust){
    if (!jsn.is_object()){
         jsn = nlohmann::json::parse((std::string)jsn);
     }
    
    std::cout << jsn["candles"].type_name() << std::endl;
    std::vector<double> data = this->to_vector(jsn["candles"], index);
     
    clust.init(data, num_clust);
    min_max = clust.min_max;
}

std::string KMeansSingle::print_plots(){
    std::stringstream ss;
    
    ss << print_clusters(this->clust) << "\n";
    ss << "Cluster Min/Max: " << "\n";
    ss << this->print_min_max(this->min_max);
    
    return ss.str();
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
