#ifndef ENTITY_H
#define ENTITY_H
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "game_consts.h"
#include "game_map.h"
#include "math_utils.h"
#include "math_utils.cpp"

using namespace std;

struct GameState;

// Adding new systems and components notes:
//-----------------------------------------
// COMPONENTS:
// When adding new components the steps required are
// (1): Add a new enum to Entity_Component_Flags
// (2): Update the ENTITY_COMPONENT_MAX_FLAGS by 1
// (3): Writing the component structure using the naming style of Entity_Component_XXXX
// (4): In generateByteSizeDataOfComponents() add exception for the new component to figure out its size for chunk storage
// (5): In allocateEntity() add exception for clearing chunk to default component
// (6): Add shortened macro to entity.h
// After these steps the component can be used normally by requesting it through getEntityComponent()
// NOTES:
// When writing the structures never use dynamic memory allocations as this will mess up the chunk system
// instead try to use pointers to this data. Example: vector<sf::Vector2i> AI_Path becomes vector<sf::Vector2i> *AI_Path
// These pointers though slower than the chunk mechanism are rare enough that they have very little cache misses
//
// SYSTEMS:
// When adding new systems the steps required are
// (1): Add a new enum to Entity_System_Flags
// (2): Update the ENTITY_SYSTEM_MAX_FLAGS by 1
// (3): Write system function using the naming style of Entity_System_XXXX
// (4): Add exception to UpdateEntity() with the function using correct parameters
// After these steps adding the system to entities will automatically be set and run
// NOTES:
// When writing systems remmeber it to be good practice to check for components in non abudunt systems.
// For example in the case of a 'burn damage' system check for health component before trying to write as it is likely for bugs to occur
// where these effects are applied unwittingly. In contrast for something like position_velocity system it might be uneccessery as these
// components in style are less likely to be removed from an entity

#define ENTITY_COMPONENT_MAX_FLAGS 5
enum Entity_Component_Flags
{
    EC_NONE         = 0,
    EC_POSITION     = 1 << 0,
    EC_VELOCITY     = 1 << 1,
    EC_SPEED        = 1 << 2,
    EC_SHAPERENDER  = 1 << 3,
    EC_SIZE_BOUNDS  = 1 << 4
};

#define HAS_COMPONENT(x) (getEntity(controller, ref).component & (uint32)x)
#define GET_EC_POSITION() ((Entity_Component_Position*)getEntityComponent(controller, ref, EC_POSITION))
#define GET_EC_VELOCITY() ((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))
#define GET_EC_SPEED() ((Entity_Component_Speed*)getEntityComponent(controller, ref, EC_SPEED))
#define GET_EC_SHAPERENDER() ((Entity_Component_ShapeRender*)getEntityComponent(controller, ref, EC_SHAPERENDER))
#define GET_EC_SIZE_BOUNDS() ((Entity_Component_Size*)getEntityComponent(controller, ref, EC_SIZE_BOUNDS))

static uint32 kEntity_Component_ByteSize[ENTITY_COMPONENT_MAX_FLAGS];
static string kEntity_Component_StringName[ENTITY_COMPONENT_MAX_FLAGS];

// Entity Component Structures
struct Entity_Component_Position
{
    sf::Vector2f position;
    uint32       room = 0;

    Entity_Component_Position(){}
    Entity_Component_Position(sf::Vector2f _pos, uint32 _room)
    {
        position = _pos;
        room     = _room;
    }
};

struct Entity_Component_Velocity
{
    sf::Vector2f velocity;
};

struct Entity_Component_Speed
{
    real32 speed;
};

struct Entity_Component_ShapeRender
{
    sf::Color fillColour;
    sf::Color outlineColour;
    real32    outlineThickness = 0;
    real32    size = 0;
};

struct Entity_Component_Size
{
    sf::IntRect size;
};

#define ENTITY_SYSTEM_MAX_FLAGS 5
enum Entity_System_Flags
{
    ES_NONE                 = 0,
    ES_PHYSICS_VELOCITY     = 1 << 0,
    ES_KEYBOARD_CONTROL     = 1 << 1,
    ES_CIRCLE_RENDER        = 1 << 2,
    ES_COLLISIONTILEMAP     = 1 << 3,
    ES_BASIC_TEST_AI        = 1 << 4
};

struct Entity
{
    bool                    flagForClear= false;
    uint32                  id          = 0;
    uint32                  component   = EC_NONE;
    uint32                  system      = ES_NONE;

    uint32                  chunkSize;
    // Chunk data followed after entity header data above

    // NOTE(Connor): No Catch case for non-existing component
    uint32 getComponentByteOffset(uint32 flag)
    {
        uint32 offset = 0;
        uint32 index = 0;
        for(uint32 i = 1; i < (uint32)flag; i+=i)
        {
            if(i & (uint32)component)
                offset += kEntity_Component_ByteSize[index];
            index++;
        }

        return offset;
    }
};

struct Entity_Chunk
{
    char*   arena;
    uint32  usedSpace;
};

struct Entity_Reference
{
    uint32 global_id=0;
    uint32 chunkID=0;
    uint32 chunkOffset=0;
    int32  sort_key = 0;
};

class Entity_Chunk_Array
{
    public:
        void addChunk();
        void removeChunk(uint32 id);
        void compressChunk(uint32 id, vector<Entity_Reference> &entity_list);

        // This returns memory area where sizeof(entity)+entity_chunk fits
        bool getNewEntitySpace(uint32 entityChunkSize, Entity_Reference &ref, bool allowResizing=true);

        uint32                 chunkSize=8192;
        vector<Entity_Chunk>   chunk;
};

struct Entity_State_Controller
{
    Entity_Chunk_Array             entity_storage;
    std::vector<Entity_Reference>  entity_list;
    uint32                         current_entity = 0;
};

// Memory Functions
void            generateByteSizeDataOfComponents();
Entity          getEntity(Entity_State_Controller &controller, Entity_Reference ref);
void*           getEntityComponent(Entity_State_Controller &controller, Entity_Reference ref, Entity_Component_Flags flag);
Entity_Reference allocateEntity(Entity_State_Controller &controller, uint32 global_id, Entity entity);
void            removeEntity(Entity_State_Controller &controller, Entity_Reference ref);

// Entity System Functions
void Entity_System_PhysicsVelocity(Entity_State_Controller &controller, real32 t, GameMap &map, Entity_Reference &entity);
void Entity_System_KeyboardControl(Entity_State_Controller &controller, real32 t, InputState &input, Entity_Reference &entity);
void Entity_System_CircleRender(Entity_State_Controller &controller, sf::RenderTarget *target, GameMap &map, Entity_Reference &entity);
void Entity_System_CollisionTilemap(Entity_State_Controller &controller, real32 t, GameMap &map, Tileset &tileset, Entity_Reference &entity);
void Entity_System_BasicTestAI(Entity_State_Controller &controller, real32 t, Entity_Reference &entity);

Entity_Reference createPlayerEntity(GameMap &map, Entity_State_Controller &controller, Entity_Component_Position position);
Entity_Reference createDumbAIEntity(GameMap &map, Entity_State_Controller &controller, Entity_Component_Position position);
void moveEntityToRoom(GameMap &map, MapRoomConnection connection, Entity_State_Controller &controller, Entity_Reference entity);
void insertion_sortUpdateOrder(vector<Entity_Reference> &list);

sf::Vector2f localRoomPositionToScreen(GameMap &map, Entity_Component_Position &position);

#endif /* end of include guard: ENTITY_H */
