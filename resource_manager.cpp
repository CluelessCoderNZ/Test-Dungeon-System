#ifndef RESOURCE_MANAGER_CPP
#define RESOURCE_MANAGER_CPP
#include "resource_manager.h"

ResourceManager::ResourceManager()
{
    resource_memory_list.resize(resource_memory_list.size()+1);

    defaultErrorResource[(uint32)RESOURCE_TEXTURE] = load(RESOURCE_TEXTURE, "Resources/Graphics/World/ErrorTexture.png");
    getTexture(defaultErrorResource[(uint32)RESOURCE_TEXTURE]).setRepeated(true);
}

ResourceManager& ResourceManager::instance()
{
    // Because Of Private Constructor Only Here Can The Static Defenition Be Made
    // C++11 Standard Stops From Double Initalization
    static ResourceManager *instance = new ResourceManager();

    // Return Instance For Global Use
    return *instance;
}

ResourceManager::~ResourceManager()
{
    for (map<string, resource_info>::iterator it=resource_info_map.begin(); it!=resource_info_map.end(); it++)
    {
        switch(it->second.type)
        {
            case RESOURCE_TEXTURE:
            {
                getTexture(it->second.handle) = sf::Texture();
            }break;
        };
    }
}

resource_memory_location ResourceManager::getNewMemoryLocation(uint32 size)
{
    assert(resource_memory_list.back().size > size);

    resource_memory_location location;
    uint32 memoryLeft = resource_memory_list.back().size-resource_memory_list.back().used;

    if(memoryLeft < size)
    {
        resource_memory_list.resize(resource_memory_list.size()+1);
    }

    location.ptr = resource_memory_list.back().data + resource_memory_list.back().used;
    location.bucket_id = resource_memory_list.size()-1;

    resource_memory_list.back().used+=size;
    return location;
}


void ResourceManager::loadFromFile(Resource_Type type, resource_memory_location location, string name)
{
    switch (type)
    {
        case RESOURCE_TEXTURE:
        {
            *((sf::Texture*)location.ptr) = sf::Texture();
            if(!((sf::Texture*)location.ptr)->loadFromFile(name))
            {
                location.ptr = get(defaultErrorResource[(uint32)RESOURCE_TEXTURE]);
            }
        }break;
    }
}

void ResourceManager::reloadAll()
{
    for (map<string, resource_info>::iterator it=resource_info_map.begin(); it!=resource_info_map.end(); it++)
    {
        loadFromFile(it->second.type, resource_memory_map[it->second.handle.id], it->first);
    }
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
            loadFromFile(type, location, name);
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

byte* ResourceManager::get(resource_handle handle)
{
    return resource_memory_map[handle.id].ptr;
}

sf::Texture& ResourceManager::getTexture(resource_handle handle)
{
    return *((sf::Texture*)get(handle));
}

#endif
