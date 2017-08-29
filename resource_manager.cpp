#ifndef RESOURCE_MANAGER_CPP
#define RESOURCE_MANAGER_CPP
#include "resource_manager.h"

ResourceManager::ResourceManager()
{
    resource_memory_list.push_back(resource_memory_bucket());
}

ResourceManager& ResourceManager::instance()
{
    // Because Of Private Constructor Only Here Can The Static Defenition Be Made
    // C++11 Standard Stops From Double Initalization
    static ResourceManager *instance = new ResourceManager();

    // Return Instance For Global Use
    return *instance;
}

resource_memory_location ResourceManager::getNewMemoryLocation(uint32 size)
{
    assert(resource_memory_list.back().size > size);

    resource_memory_location location;
    uint32 memoryLeft = resource_memory_list.back().size-resource_memory_list.back().used;

    if(memoryLeft < size)
    {
        resource_memory_list.push_back(resource_memory_bucket());
    }

    location.ptr = resource_memory_list.back().data + resource_memory_list.back().used;
    location.bucket_id = resource_memory_list.size()-1;

    resource_memory_list.back().used+=size;
    return location;
}

resource_handle ResourceManager::create(Resource_Type type, string name)
{
    resource_handle          handle;
    resource_info            info;
    resource_memory_location location;
    info.type = type;

    switch(type)
    {
        case RESOURCE_TEXTURE:
        {
            info.size = sizeof(sf::Texture);
            location = getNewMemoryLocation(info.size);
            location.ptr = new sf::Texture;
            ((sf::Texture*)location.ptr)->loadFromFile(name);
        }break;
    }

    handle.id = resource_handle_index++;
    info.handle = handle;

    resource_currently_loaded++;
    resource_info_map[name] = info;
    resource_memory_map[handle.id] = location;

    return handle;
}

resource_handle ResourceManager::load(Resource_Type type, string name)
{
    map<string, resource_info>::iterator it = resource_info_map.find(name);

    if(it == resource_info_map.end())
    {
        return create(type, name);
    }

    return resource_info_map[name].handle;
}

void* ResourceManager::get(resource_handle handle)
{
    return resource_memory_map[handle.id].ptr;
}

#endif
