//
//  KMeans.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/7/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include "KMeans.hpp"

KMeans::KMeans() : clust{}, min_max{}{}

KMeans::KMeans(std::map<std::string, std::vector<double>> raw) : clust{}, min_max{}{
    std::map<std::string, std::vector<std::shared_ptr<Clusters>>> clusters_cache;
    
    for (auto i : raw){
        clusters_cache.insert(std::pair<std::string,
                              std::vector<std::shared_ptr<Clusters>>>(i.first, load_clusters(i.second)));
    }
    
    for (auto i : clusters_cache){
        unsigned long opt_clust = find_optimum_clusters(i.second);
        clust.insert(std::pair<std::string,
                               std::shared_ptr<Clusters>>(i.first, i.second[opt_clust-1]));
    }
    
}

/* * * * * * * * * * * * * * * * * * *
 *       KMeans: Private Methods     *
 * * * * * * * * * * * * * * * * * * */


std::vector<std::shared_ptr<Clusters>> KMeans::load_clusters(std::vector<double> &data){
    std::vector<std::shared_ptr<Clusters>> clusters;
    for (int i = 1; i < 25; i++){
        std::shared_ptr<Clusters> tmp (new Clusters());
        tmp->init(data, i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}

// wss = within sum squares, sum of all the distances to the centroid
unsigned long KMeans::find_optimum_clusters(std::vector<std::shared_ptr<Clusters>> &clusters){
    std::vector<double> delta1, last;
    last.push_back(clusters.front()->wsss);
    
    for (auto& i : clusters){
        
        if(delta1.size() > 0){
            double tmp = last.back() - i->wsss;
            
            delta1.push_back(tmp);
            last.push_back(i->wsss);
            
            //ensures only two items are in vector to compare.
            if(delta1.size() > 2 || delta1.front() == 0)
                delta1.erase(delta1.begin());
            
            //this is your delta2
            if (delta1.front() - delta1.back() < 1)
                return i->centroid.size();
        }
        else{
            //initializes vector if empty with first wss
            delta1.push_back(i->wsss);
        }
    }
    
    //Needs some form of error handling here
    return 0;
}

void KMeans::pair_high_low(Clusters &high, Clusters &low){
    for (int i = 0; i < low.min_max.size(); i++){
        this->min_max.push_back({high.min_max[i][0], low.min_max[i][1]});
    }
}



/* * * * * * * * * * * * * * * * * * *
 *       KMeans: Print Methods       *
 * * * * * * * * * * * * * * * * * * */

std::string KMeans::print(){
    std::stringstream ss;
    
    for (auto& i : this->clust){
        ss << "Data Source: " << i.first << std::endl;
        ss << print_clusters(i.second);
        ss << this->print_min_max(i.second);
    }
    
    return ss.str();
}

std::string KMeans::print_clusters(std::shared_ptr<Clusters> clust){
    std::stringstream ss;
    int str_len = 25;
    
    ss << "[Point/Cluster/Distance]" << std::endl;
    
    int row_count = 0;
    for (auto i : clust->points){
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

std::string KMeans::print_min_max(std::shared_ptr<Clusters> clust){
    std::stringstream ss;
    int str_len = 20;
    
    ss << "\n[Max/Min] \n";
    for (auto i : clust->min_max){
        ss << "[" << i[0]->price<< ", " << i[1]->price << "]";
        ss << std::setw(str_len - (int)ss.str().length()) << " ";
    }
    
    return ss.str();
}


std::map<std::string, std::vector<std::vector<std::vector<double>>>> KMeans::fetch_results(){
    std::map<std::string, std::vector<std::vector<std::vector<double>>>> ret {};
    
    for (auto& i : this->clust){
        std::vector<std::vector<std::vector<double>>> tmp;
        
        tmp.push_back(fetch_data(i.second->points));
        tmp.push_back(fetch_data(i.second->centroid));
        tmp.push_back(fetch_min_max(i.second->min_max));
        
        ret.insert(std::pair<std::string, std::vector<std::vector<std::vector<double>>>>(i.first, tmp));
    }
    
    return ret;
};

std::vector<std::vector<double>> KMeans::fetch_data(std::vector<Point> &data){
    std::vector<std::vector<double>> ret;
    for(auto i : data){
        std::vector<double> tmp {i.price, (double)i.cluster, i.minDist};
        ret.push_back(tmp);
    }
    return ret;
}

std::vector<std::vector<double>> KMeans::fetch_min_max(std::vector<std::vector<Point*>> &data){
    std::vector<std::vector<double>> ret;
    for (auto i : data)
        ret.push_back(std::vector<double> {i[0]->price, i[1]->price});
    
    return ret;
}
