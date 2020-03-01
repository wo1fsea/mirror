#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_MANAGER_
#define _MIRROR_TYPE_DESCRIPTOR_MANAGER_

#include <map>
#include "type_descriptor.hpp"

namespace mirror
{
class type_descriptor_manager
{
    std::map<std::string, type_descriptor *> type_descriptor_map;
};
} // namespace mirror

#endif
