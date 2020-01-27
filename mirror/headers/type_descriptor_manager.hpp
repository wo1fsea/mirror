#include <map>
#include "type_descriptor.hpp"

class type_descriptor_manager
{
    std::map<std::string, type_descriptor_base*> type_descriptor_map;
};