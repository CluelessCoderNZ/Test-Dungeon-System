#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "game_consts.h"
#include <cstring>
#include <SFML/Graphics.hpp>


#define kResourceTypeCount 3
enum Resource_Type
{
    RESOURCE_TEXTURE,
    RESOURCE_MUSIC,
    RESOURCE_SOUND
};

struct resource_handle
{
    uint32 id = 0;
};

struct resource_memory_location
{
    byte* ptr;
    uint32  bucket_id;
};

struct resource_info
{
    resource_handle handle;
    uint32          size;
    Resource_Type   type;
};

struct resource_memory_bucket
{
    uint32  size=8092;
    uint32  used=0;
    byte* data;

    resource_memory_bucket()
    {
        data = (byte*)malloc(size);
    }

    ~resource_memory_bucket()
    {
        // NOTE(Connor): Freeing Malloc Causes Crashes at program end therefore I'm allowing the OS to clean up instead
        //free(data);
    }
};

class ResourceManager
{
    public:
        // Instance Functions
        ResourceManager();
        ~ResourceManager();
        static ResourceManager& instance();                 // Returns Instance Of Resource


        void            reloadAll();
        void            loadFromFile(Resource_Type type, resource_memory_location location, string name);
        resource_memory_location getNewMemoryLocation(uint32 size);
        resource_handle create(Resource_Type type, string name);
        resource_handle load(Resource_Type type, string name);
        byte*           get(resource_handle handle);

        sf::Texture&    getTexture(resource_handle handle);

        uint32                                          resource_currently_loaded = 0;
        uint32                                          resource_handle_index = 0;
        map<string, resource_info>                      resource_info_map;
        map<uint32, resource_memory_location>           resource_memory_map;
        vector<resource_memory_bucket>                  resource_memory_list;

        resource_handle defaultErrorResource[kResourceTypeCount];
};

#endif
