#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "game_consts.h"


enum Resource_Type
{
    RESOURCE_TEXTURE,
    RESOURCE_MUSIC,
    RESOURCE_SOUND
};

struct resource_handle
{
    uint32 id;
};

struct resource_memory_location
{
    void* ptr;
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
    void* data;

    resource_memory_bucket()
    {
        data = malloc(size);
    }
    ~resource_memory_bucket()
    {
        free(data);
    }
};

class ResourceManager
{
    public:
        // Instance Functions
        ResourceManager();
        static ResourceManager& instance();                 // Returns Instance Of Resource

        resource_memory_location getNewMemoryLocation(uint32 size);
        resource_handle create(Resource_Type type, string name);
        resource_handle load(Resource_Type type, string name);
        void*           get(resource_handle handle);

        uint32                                          resource_currently_loaded;
        uint32                                          resource_handle_index;
        map<string, resource_info>                      resource_info_map;
        map<uint32, resource_memory_location>           resource_memory_map;
        vector<resource_memory_bucket>                  resource_memory_list;
};

#endif
