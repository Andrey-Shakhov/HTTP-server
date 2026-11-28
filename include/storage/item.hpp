#pragma once
#include <string>

namespace storage{
    struct Item
    {
        unsigned int id;
        std::string name;
        int value;
        unsigned int version;
    };
    
}
