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
#include "json.hpp"
#include "points.hpp"
#include "KMeans.hpp"

using namespace std;

void print_file(std::string result){
    std::ofstream file("/Volumes/web/KMean/output.txt");
    //std::ofstream file("output.txt");
    if(file.is_open()){
        cout << "here" << endl;
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
    KMeansMulti my_test(jsn);
    KMeansSingle rsi_plots(jsn, "rsi_data/rsi", 3);
    //KMeansSingle my_test(jsn, "rsi");

    
    //THIS PRINTS PLOTS TO FILE
    nlohmann::json json;
    json.push_back(my_test.high.print_plots());
    json.push_back(my_test.low.print_plots());
    json.push_back(rsi_plots.clust.print_plots());
    std::string result = my_test.high.print_plots();
    
    
    print_file(json.dump());
}
