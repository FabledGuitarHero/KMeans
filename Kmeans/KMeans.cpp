//
//  KMeans.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/7/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include "KMeans.hpp"

/* * * * * * * * * * * * * * * * * * *
 *                                   *
 *   KMeans_data Class Declaration:  *
 *   Struct used to organize data    *
 *   uniformly to be classified in   *
 *         the KMeans class.         *
 *                                   *
 * * * * * * * * * * * * * * * * * * */

KMeans_data::KMeans_data() : data{}{}

KMeans_data::KMeans_data(std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> raw) : data{raw}{}

/* * * * * * * * * * * * * * * * * * *
 *                                   *
 *   KMeans_proc Class Declaration:  *
 *  Struct used to return the KMeans *
 *   processed data to the calling   *
 *            function.              *
 *                                   *
 * * * * * * * * * * * * * * * * * * */

KMeans_proc::KMeans_proc(std::map<std::string, std::vector<std::vector<std::vector<double>>>> raw){
    /*
     *  Did not go the unique_ptr route because the calling function KMeans::fetch_data() generates
     *      and returns the above map, would be needless copy-constructing to place it on the heap.
     */
    data = raw;
}

std::map<std::string, std::vector<std::vector<std::vector<double>>>>::iterator KMeans_proc::begin(){
    return data.begin();
}

std::map<std::string, std::vector<std::vector<std::vector<double>>>>::iterator KMeans_proc::end(){
    return data.end();
}

/* * * * * * * * * * * * * * * * * * *
 *                                   *
 *      KMeans Class Declaration     *
 *                                   *
 * * * * * * * * * * * * * * * * * * */

KMeans::KMeans() : clust{}{}

KMeans::KMeans(std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> raw) : clust{}{
    init(raw);
}

KMeans::KMeans(KMeans_data raw) : clust{}{
    init(raw.data);
}

void KMeans::init(std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> &raw){
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


std::vector<std::shared_ptr<Clusters>> KMeans::load_clusters(std::pair<std::vector<double>, std::vector<double>> &data){
    std::vector<std::shared_ptr<Clusters>> clusters;
    for (int i = 1; i < 25; i++){
        std::shared_ptr<Clusters> tmp (new Clusters());
        tmp->init(data.first, data.second, i);
        clusters.push_back(tmp);
    }
    
    return clusters;
}

/* * * * * * * * * * * * * * * * * * * * * *
 *  WSS = Within Sum Squares, sum of all   *
 *     the distances to the centroid       *
 *                                         *
 *  Uses the Elbow method to determine     *
 *     the optimal amount of clusters      *
 * * * * * * * * * * * * * * * * * * * * * */
unsigned long KMeans::find_optimum_clusters(std::vector<std::shared_ptr<Clusters>> &clusters){
    std::vector<double> delta1, last;
    last.push_back(clusters.front()->wss);
    
    for (auto& i : clusters){
        
        if(delta1.size() > 0){
            double tmp = last.back() - i->wss;
            
            delta1.push_back(tmp);
            last.push_back(i->wss);
            
            //ensures only two items are in vector to compare.
            if(delta1.size() > 2 || delta1.front() == 0)
                delta1.erase(delta1.begin());
            
            //this is your delta2
            if (delta1.front() - delta1.back() < 1)
                return i->centroid.size();
        }
        else{
            //initializes vector if empty with first wss
            delta1.push_back(i->wss);
        }
    }
    
    throw std::runtime_error(std::string("Error determining K Clusters.  Could not determine optimal number of clusters."));
}



/* * * * * * * * * * * * * * * * * * *
 *       KMeans: Print Methods       *
 * * * * * * * * * * * * * * * * * * */

std::string KMeans::print(){
    std::stringstream ss;
    
    for (auto& i : this->clust){
        ss << "Data Source: " << i.first << "\n";
        ss << print_clusters(i.second) << "\n";
        ss << print_centroids(i.second) << "\n";
        ss << this->print_min_max(i.second) << "\n\n";
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
        
        tmp << "[" << i.y_data << ", " << i.cluster << ", " << i.minDist << "]";
        tmp << std::setw(str_len - (int)tmp.str().length()) << " ";
        
        row_count++;
        if (row_count % 5 == 0)
            tmp << "\n";
        
        ss << tmp.str();
    }
    
    return ss.str();
}

std::string KMeans::print_centroids(std::shared_ptr<Clusters> clust){
    std::stringstream ss;
    int str_len = 25;
    
    ss << "[Cluster/Data]" << std::endl;
    
    int row_count = 0;
    for (auto i : clust->centroid){
        std::stringstream tmp;
        
        tmp << "[" << i.cluster << ", " << i.y_data  << " " << i.x_data << "]";
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
        
        tmp << "[" << i[0]->y_data << ", " << i[1]->y_data << "]";
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
        ss << "[" << i[0]->y_data<< ", " << i[1]->y_data << "]";
        ss << std::setw(str_len - (int)ss.str().length()) << " ";
    }
    
    return ss.str();
}

/* * * * * * * * * * * * * * * * * * *
 *     KMeans: Fetch data methods    *
 * * * * * * * * * * * * * * * * * * */

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
        std::vector<double> tmp {i.y_data, i.x_data, (double)i.cluster, i.minDist};
        ret.push_back(tmp);
    }
    return ret;
}

std::vector<std::vector<double>> KMeans::fetch_min_max(std::vector<std::vector<Point*>> &data){
    std::vector<std::vector<double>> ret;
    for (auto i : data)
        ret.push_back(std::vector<double> {i[0]->y_data, i[1]->y_data});
    
    return ret;
}
