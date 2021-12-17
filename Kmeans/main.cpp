//
//  main.cpp
//  Kmeans
//
//  Created by Nate Henderson on 6/5/20.
//  Copyright © 2020 Nate Henderson. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <ctime>
#include <map>
#include "json.hpp"
#include "points.hpp"
#include "KMeans.hpp"

using namespace std;



std::map<std::string, std::vector<double>> parse_to_vector(nlohmann::json &jsn, std::string path){
    //Parses json string if not already decoded
    if(jsn.is_string())
        jsn = nlohmann::json::parse((std::string)jsn);
    
    std::map<std::string, std::vector<double>> ret {};
    
    
    auto f = path.find('/');
    if(f != std::string::npos){
        std::string new_path = path.substr(f + 1,path.length());
        std::string cur_path = path.substr(0, f);
        
        for (auto i : jsn){
            std::map<std::string, std::vector<double>> tmp = parse_to_vector(i.at(cur_path), new_path);
            
            if (ret.find(tmp.begin()->first) == ret.end())
                ret.insert({tmp.begin()->first, tmp.begin()->second});
            else{
                //Really sloppy way of appending TmpMap(K,V)->V into RetMap(K,V)->V.
                std::string key = tmp.begin()->first;
                ret.at(key).insert(ret.at(key).end(), tmp.at(key).begin(), tmp.at(key).end());
            }
        }
    }
    else{
        //subj obejct from jsn array, from code above.
        if(jsn.is_object())
            return std::map<std::string, std::vector<double>> {{path, {jsn[path]}}};
        
        //Is an object and needs to be looped.
        //If key doesn't exist, create new array.
        //  Else, add to array.
        for (auto i : jsn){
            if (ret.find(path) == ret.end())
                ret.insert({path, {i[path]}});
            else
                ret.at(path).push_back(i[path]);
        }
    }

    
    return ret;
}

std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> parse_to_vector(
                                                                            nlohmann::json &jsn,
                                                                            std::vector<std::string> y_path,
                                                                            std::vector<std::string> x_path
                                                                            ){
    if (x_path.size() > 1)
        throw std::runtime_error(std::string("X-Axis can only have one dataset.  Aborting."));
    
    
    std::map<std::string, std::pair<std::vector<double>, std::vector<double>>> ret {};
    std::map<std::string, std::vector<double>> x_data = parse_to_vector(jsn, x_path[0]);
    
    
    for (auto i : y_path){
        std::map<std::string, std::vector<double>> tmp = parse_to_vector(jsn, i);
        
        // Ret<K,<K1,V1>> K = Key, V = Value
        ret.insert(std::make_pair(tmp.begin()->first,
                                  std::make_pair(tmp.begin()->second, x_data.begin()->second)));
    }
    
    return ret;
}

void print_file(std::string result){
    //std::ofstream file("/Volumes/web/KMean/output.txt");
    std::ofstream file("output.txt");
    if(file.is_open()){
        file << result;
    }
    else{
        cout << "failed to open file" << endl;
    }
    file.close();
}

std::string open_file(std::string file_name){
    std::ifstream file(file_name);
    std::string line;
    std::stringstream input;
    
    if (file.is_open()){
        while(std::getline(file, line)){
            input << line << "\r";
        }
    }
    else {
        throw std::runtime_error(std::string("File courrupted or missing."));
    }
    
    return input.str().substr(0, input.str().length()-1);
}


int main(int argc, const char * argv[]) {
    std::string str = open_file("FB.txt");

    nlohmann::json jsn = nlohmann::json::parse(str);
    
    try{
    
        KMeans_data tmp2 (parse_to_vector(jsn["candles"],
                          std::vector<std::string> {"high", "low", "rsi_data/rsi"},
                          std::vector<std::string> {"datetime"}));
    
        KMeans test(tmp2);
        std::map<std::string, std::vector<std::vector<std::vector<double>>>> answer = test.fetch_results();
        std::cout << test.print() << std::endl;
            
        nlohmann::json proc;
        for (auto i : answer){
            proc[i.first]["points"] = i.second[0];
            proc[i.first]["centroid"] = i.second[1];
            proc[i.first]["min_max"] = i.second[2];
        }
    
        //std:cout << proc.dump(4) << std::endl;
        print_file(proc.dump());
        
        
    }
    catch(std::exception &e){
        std::cout << e.what() << std::endl;
   }
}
