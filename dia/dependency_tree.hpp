//
//  dependency_tree.hpp
//  dia
//
//  Created by Daher Alfawares on 11/18/17.
//  Copyright © 2017 Daher Alfawares. All rights reserved.
//

#ifndef dependency_tree_hpp
#define dependency_tree_hpp

#include <string>
#include <vector>
#include <map>

using std::map;
using std::string;
using std::vector;

class dependency_tree {
    map<string,vector<string>> flat_tree;
    
    vector<string> operator [](std::string key){
        if(auto iterator = this->flat_tree.find(key) == this->flat_tree.end()){
            this->flat_tree[key] = vector<string>();
        }
        
        return this->flat_tree[key];
    }
};

#endif /* dependency_tree_hpp */
