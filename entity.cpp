#ifndef ENTITY_CPP
#define ENTITY_CPP
#include "entity.h"



void generateByteSizeDataOfComponents()
{
    kEntity_Component_ByteSize[0]   = sizeof(Entity_Component_Position);
    kEntity_Component_StringName[0] = "Position";
    kEntity_Component_ByteSize[1]   = sizeof(Entity_Component_Velocity);
    kEntity_Component_StringName[1] = "Velocity";
    kEntity_Component_ByteSize[2]   = sizeof(Entity_Component_Speed);
    kEntity_Component_StringName[2] = "Speed";
    kEntity_Component_ByteSize[3]   = sizeof(Entity_Component_ShapeRender);
    kEntity_Component_StringName[3] = "Shape Data";
    kEntity_Component_ByteSize[4]   = sizeof(Entity_Component_Size);
    kEntity_Component_StringName[4] = "Bounds";
    kEntity_Component_ByteSize[5]   = sizeof(Entity_Component_PlayerRender);
    kEntity_Component_StringName[5] = "Player Render";
}

Entity getEntity(Entity_State_Controller &controller, Entity_Reference ref)
{
    return *(Entity*)(controller.entity_storage.chunk[ref.chunkID].arena+ref.chunkOffset);
}

void* getEntityComponent(Entity_State_Controller &controller, Entity_Reference ref, Entity_Component_Flags flag)
{
    char* entityLocation = (controller.entity_storage.chunk[ref.chunkID].arena+ref.chunkOffset);

    return (void*)(entityLocation+sizeof(Entity)+((Entity*)entityLocation)->getComponentByteOffset((uint32)flag));
}

Entity_Reference allocateEntity(Entity_State_Controller &controller, uint32 global_id, Entity entity)
{
    // Calculate Entity Component Chunk Space
    entity.chunkSize = entity.getComponentByteOffset(1 << ENTITY_COMPONENT_MAX_FLAGS);

    // Allocate Data
    Entity_Reference reference;
    controller.entity_storage.getNewEntitySpace(entity.chunkSize + sizeof(Entity), reference);
    reference.global_id = global_id;

    // Copy Entity Header Data
    *((Entity*)(controller.entity_storage.chunk[reference.chunkID].arena+reference.chunkOffset)) = entity;

    // Initalize chunk with blank data
    char* pointer = controller.entity_storage.chunk[reference.chunkID].arena+reference.chunkOffset+sizeof(Entity);


    if(entity.component & (uint32)EC_POSITION)
    {
        auto temp = Entity_Component_Position();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[0]);
        pointer+=kEntity_Component_ByteSize[0];
    }
    if(entity.component & (uint32)EC_VELOCITY)
    {
        auto temp = Entity_Component_Velocity();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[1]);
        pointer+=kEntity_Component_ByteSize[1];
    }
    if(entity.component & (uint32)EC_SPEED)
    {
        auto temp = Entity_Component_Speed();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[2]);
        pointer+=kEntity_Component_ByteSize[2];
    }
    if(entity.component & (uint32)EC_SHAPERENDER)
    {
        auto temp = Entity_Component_ShapeRender();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[3]);
        pointer+=kEntity_Component_ByteSize[3];
    }
    if(entity.component & (uint32)EC_SIZE_BOUNDS)
    {
        auto temp = Entity_Component_Size();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[4]);
        pointer+=kEntity_Component_ByteSize[4];
    }
    if(entity.component & (uint32)EC_PLAYERRENDER)
    {
        auto temp = Entity_Component_PlayerRender();
        memcpy(pointer, &temp, kEntity_Component_ByteSize[5]);
        pointer+=kEntity_Component_ByteSize[5];
    }

    return reference;
}

void removeEntity(Entity_State_Controller &controller, Entity_Reference ref)
{
    ((Entity*)(controller.entity_storage.chunk[ref.chunkID].arena+ref.chunkOffset))->flagForClear = true;
    for(uint32 i=0; i < controller.entity_list.size(); i++)
    {
        if(controller.entity_list[i].global_id == ref.global_id)
        {
            controller.entity_list.erase(controller.entity_list.begin()+i);
            break;
        }
    }
}

void Entity_Chunk_Array::addChunk()
{
    chunk.resize(chunk.size()+1);
    chunk[chunk.size()-1].arena = (char*)malloc(chunkSize);
}

void Entity_Chunk_Array::removeChunk(uint32 id)
{
    delete[] chunk[id].arena;
    chunk.erase(chunk.begin()+id);
}

void Entity_Chunk_Array::compressChunk(uint32 id, vector<Entity_Reference> &entity_list)
{
    vector<Entity_Reference>  updatedEntityIds;
    uint32 memoryPushBack   = 0;
    uint32 chunkCursor      = 0;
    while(chunkCursor < chunk[id].usedSpace)
    {
        Entity *ent = ((Entity*)(chunk[id].arena + chunkCursor));
        uint32 ent_size = sizeof(Entity) + ent->chunkSize;

        if(ent->flagForClear)
        {
            memoryPushBack = sizeof(Entity) + ent->chunkSize;
        }else if(memoryPushBack > 0)
        {
            Entity_Reference ref;
            ref.global_id = ent->id;
            ref.chunkID   = id;
            ref.chunkOffset = chunkCursor-memoryPushBack;
            updatedEntityIds.push_back(ref);

            memcpy((void*)(chunk[id].arena+chunkCursor-memoryPushBack), (void*)(chunk[id].arena+chunkCursor), sizeof(Entity) + ent->chunkSize);
        }
        chunkCursor += ent_size;
    }
    chunk[id].usedSpace =chunk[id].usedSpace - memoryPushBack;

    // Update register
    for(uint32 i = 0; i < entity_list.size(); i++)
    {
        for(uint32 x = 0; x < updatedEntityIds.size(); x++)
        {
            if(entity_list[i].global_id == updatedEntityIds[x].global_id)
            {
                entity_list[i] = updatedEntityIds[x];
                updatedEntityIds.erase(updatedEntityIds.begin()+x);
                break;
            }
        }
    }
}

bool Entity_Chunk_Array::getNewEntitySpace(uint32 entityChunkSize, Entity_Reference &ref, bool allowResizing)
{
    assert(entityChunkSize < chunkSize);

    for(uint32 i = 0; i < chunk.size(); i++)
    {
        uint32 remaingSpace = chunkSize - chunk[i].usedSpace;
        if(remaingSpace >= entityChunkSize)
        {
            ref.chunkID     = i;
            ref.chunkOffset = chunk[i].usedSpace;

            chunk[i].usedSpace += entityChunkSize;
            return true;
        }

        if(i+1 == chunk.size() && allowResizing)
        {
            addChunk();
        }
    }

    return false;
}




void Entity_System_PhysicsVelocity(Entity_State_Controller &controller, real32 t, GameMap &map, Entity_Reference &ref)
{
    Entity entity = getEntity(controller, ref);

    GET_EC_POSITION()->position += GET_EC_VELOCITY()->velocity*t;

    // TODO(Connor): Reconsider Splitting this functionality into its own system
    sf::Vector2f globalPos = GET_EC_POSITION()->position + sf::Vector2f(map.room[GET_EC_POSITION()->room].bounds.left, map.room[GET_EC_POSITION()->room].bounds.top);

    for(uint32 i = 0; i < map.room[GET_EC_POSITION()->room].connection.size(); i++)
    {
        MapRoomConnection connect = map.room[GET_EC_POSITION()->room].connection.at(i);
        if((real32)map.room[connect.secondaryRoom.id].bounds.left < globalPos.x &&
           (real32)map.room[connect.secondaryRoom.id].bounds.top  < globalPos.y &&
           (real32)map.room[connect.secondaryRoom.id].bounds.left + map.room[connect.secondaryRoom.id].bounds.width  > globalPos.x &&
           (real32)map.room[connect.secondaryRoom.id].bounds.top  + map.room[connect.secondaryRoom.id].bounds.height > globalPos.y)
        {
            moveEntityToRoom(map, connect, controller, ref);
            break;
        }
    }

}

void Entity_System_KeyboardControl(Entity_State_Controller &controller, real32 t, InputState &input, Entity_Reference &ref)
{
    Entity entity = getEntity(controller, ref);
    if(HAS_COMPONENT(EC_VELOCITY) && HAS_COMPONENT(EC_POSITION))
    {
        real32 signDirectionX = (GET_EC_VELOCITY()->velocity.x > 0) ? -1 : 1;
        real32 signDirectionY = (GET_EC_VELOCITY()->velocity.y > 0) ? -1 : 1;

        GET_EC_VELOCITY()->velocity.x += signDirectionX * (0.2 * abs(GET_EC_VELOCITY()->velocity.x) + 0.01) * t;
        GET_EC_VELOCITY()->velocity.y += signDirectionY * (0.2 * abs(GET_EC_VELOCITY()->velocity.y) + 0.01) * t;

        real32 new_signDirectionX  = (GET_EC_VELOCITY()->velocity.x > 0) ? -1 : 1;
        real32 new_signDirectionY  = (GET_EC_VELOCITY()->velocity.y > 0) ? -1 : 1;

        if(new_signDirectionX != signDirectionX)
            GET_EC_VELOCITY()->velocity.x=0;
        if(new_signDirectionY != signDirectionY)
            GET_EC_VELOCITY()->velocity.y=0;

        if(input.action(INPUT_UP).isDown)
        {
            GET_EC_VELOCITY()->velocity.y = -GET_EC_SPEED()->speed;
        }
        if(input.action(INPUT_DOWN).isDown)
        {
            GET_EC_VELOCITY()->velocity.y = GET_EC_SPEED()->speed;
        }
        if(input.action(INPUT_LEFT).isDown)
        {
            GET_EC_VELOCITY()->velocity.x = -GET_EC_SPEED()->speed;
        }
        if(input.action(INPUT_RIGHT).isDown)
        {
            GET_EC_VELOCITY()->velocity.x = GET_EC_SPEED()->speed;
        }
    }

}

void Entity_System_CircleRender(Entity_State_Controller &controller, sf::RenderTarget *target, GameMap &map, Entity_Reference &ref)
{
    sf::CircleShape circle(50);
    circle.setPosition(localRoomPositionToScreen(map, *GET_EC_POSITION()));

    if(HAS_COMPONENT(EC_SHAPERENDER))
    {
        Entity_Component_ShapeRender* data = GET_EC_SHAPERENDER();
        circle.setFillColor(data->fillColour);
        circle.setOutlineColor(data->outlineColour);
        circle.setOutlineThickness(data->outlineThickness);
        circle.setRadius(data->size/2);
        circle.setOrigin(circle.getRadius(), circle.getRadius()*2 + circle.getOutlineThickness());
    }
    target->draw(circle);
}

void Entity_System_CollisionTilemap(Entity_State_Controller &controller, real32 t, GameMap &map, Tileset &tileset, Entity_Reference &ref)
{
    Entity entity = getEntity(controller, ref);
    if(HAS_COMPONENT(EC_SIZE_BOUNDS))
    {
        sf::FloatRect   bounds   = scaleRect(GET_EC_SIZE_BOUNDS()->size, sf::Vector2f(1.0f / map.tileSize.x,1.0f / map.tileSize.y));
        sf::Vector2f    position = GET_EC_POSITION()->position;
        sf::Vector2f    velocity = GET_EC_VELOCITY()->velocity;
        uint32          room     = GET_EC_POSITION()->room;

        bounds.left += position.x;
        bounds.top  += position.y;

        // TODO(Connor): Recheck this code for sticky walls for other entities other than the player who always has his position at the bottom middle of his collision boundary
        if(doesRectContainSolidTiles(map, tileset, room, bounds, sf::Vector2f(velocity.x*t, 0)))
        {
            if(GET_EC_VELOCITY()->velocity.x < 0)
            {
                GET_EC_POSITION()->position.x = floor(GET_EC_POSITION()->position.x) + bounds.width/2 + 0.01;
            }else{
                GET_EC_POSITION()->position.x = ceil(GET_EC_POSITION()->position.x) - bounds.width/2 - 0.01;
            }
            GET_EC_VELOCITY()->velocity.x = 0;
        }

        if(doesRectContainSolidTiles(map, tileset, room, bounds, sf::Vector2f(0, velocity.y*t)))
        {
            if(GET_EC_VELOCITY()->velocity.y < 0)
            {
                GET_EC_POSITION()->position.y = floor(GET_EC_POSITION()->position.y) + bounds.height + 0.01;
            }else{
                GET_EC_POSITION()->position.y = ceil(GET_EC_POSITION()->position.y) - 0.01;
            }
            GET_EC_VELOCITY()->velocity.y = 0;
        }

    }else
    {
        uint32 tileID_X = map.room[GET_EC_POSITION()->room].getTile
                        (GET_EC_POSITION()->position.x + GET_EC_VELOCITY()->velocity.x*t,
                         GET_EC_POSITION()->position.y).tileID;

         uint32 tileID_Y = map.room[GET_EC_POSITION()->room].getTile
                         (GET_EC_POSITION()->position.x,
                          GET_EC_POSITION()->position.y + GET_EC_VELOCITY()->velocity.y*t).tileID;

        if(tileset.tile[tileID_X].isSolid)
            GET_EC_VELOCITY()->velocity.x = 0;

        if(tileset.tile[tileID_Y].isSolid)
            GET_EC_VELOCITY()->velocity.y = 0;
    }
}

void Entity_System_PlayerRender(Entity_State_Controller &controller, sf::RenderTarget *target, GameMap &map, Entity_Reference &ref, real32 t)
{
    if(HAS_COMPONENT(EC_PLAYERRENDER))
    {
        Entity_Component_PlayerRender *data = GET_EC_PLAYERRENDER();
        data->sprite.setPosition(localRoomPositionToScreen(map, *GET_EC_POSITION()));
        if(GET_EC_VELOCITY()->velocity == sf::Vector2f(0,0))
        {
            data->animation_walk_front.timer.restart();
            data->animation_walk_front.animate(0, data->sprite);
        }else if(GET_EC_VELOCITY()->velocity.y < 0)
        {
            data->animation_walk_back.animate(t, data->sprite);
        }else
        {
            data->animation_walk_front.animate(t, data->sprite);
        }
        target->draw(data->sprite);
    }
}

void Entity_System_BasicTestAI(Entity_State_Controller &controller, real32 t, Entity_Reference &ref)
{
    real32 angle = atan2(((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity.y,((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity.x);
    angle += (getRandomFloat()-0.5)*0.3*t;

    ((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity.x = cos(angle) * ((Entity_Component_Speed*)getEntityComponent(controller, ref, EC_SPEED))->speed;
    ((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity.y = sin(angle) * ((Entity_Component_Speed*)getEntityComponent(controller, ref, EC_SPEED))->speed;

    sf::FloatRect   main_bounds   = scaleRect(((Entity_Component_Size*)getEntityComponent(controller, ref, EC_SIZE_BOUNDS))->size, sf::Vector2f(1.0f / 32.0,1.0f / 20.0));
    sf::Vector2f    main_position = ((Entity_Component_Position*)getEntityComponent(controller, ref, EC_POSITION))->position;
    main_bounds.left += main_position.x;
    main_bounds.top  += main_position.y;
}


Entity_Reference createPlayerEntity(GameMap &map, Entity_State_Controller &controller, Entity_Component_Position position)
{
    Entity entity;
    entity.id           = controller.current_entity++;
    entity.component    = (uint32)EC_POSITION | (uint32)EC_VELOCITY | (uint32)EC_SPEED | (uint32)EC_PLAYERRENDER | (uint32)EC_SIZE_BOUNDS;
    entity.system       = (uint32)ES_PHYSICS_VELOCITY | (uint32)ES_KEYBOARD_CONTROL | (uint32)ES_PLAYER_RENDER | (uint32)ES_COLLISIONTILEMAP;

    Entity_Reference ref = allocateEntity(controller, entity.id, entity);
    map.room[position.room].entity_list.push_back(ref);

    *((Entity_Component_Position*)getEntityComponent(controller, ref, EC_POSITION))             = position;
    ((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity    = sf::Vector2f(0,0);
    ((Entity_Component_Speed*)getEntityComponent(controller, ref,    EC_SPEED))->speed          = 0.05;
    ((Entity_Component_Size*)getEntityComponent(controller, ref, EC_SIZE_BOUNDS))->size         = sf::IntRect(-20, -10, 40, 10);

    return ref;
}

Entity_Reference createDumbAIEntity(GameMap &map, Entity_State_Controller &controller, Entity_Component_Position position)
{
    Entity entity;
    entity.id           = controller.current_entity++;
    entity.component    = (uint32)EC_POSITION | (uint32)EC_VELOCITY | (uint32)EC_SPEED | (uint32)EC_PLAYERRENDER | (uint32)EC_SIZE_BOUNDS;
    entity.system       = (uint32)ES_PHYSICS_VELOCITY | (uint32)ES_BASIC_TEST_AI | (uint32)ES_PLAYER_RENDER | (uint32)ES_COLLISIONTILEMAP;

    Entity_Reference ref = allocateEntity(controller, entity.id, entity);
    map.room[position.room].entity_list.push_back(ref);

    Entity_Component_ShapeRender shape;
    shape.fillColour        = sf::Color::Green;
    shape.outlineColour     = sf::Color::White;
    shape.outlineThickness  = 2;
    shape.size              = 16;

    *((Entity_Component_Position*)getEntityComponent(controller, ref, EC_POSITION))             = position;
    ((Entity_Component_Velocity*)getEntityComponent(controller, ref, EC_VELOCITY))->velocity    = sf::Vector2f(0,0);
    ((Entity_Component_Speed*)getEntityComponent(controller, ref,    EC_SPEED))->speed          = 0.3;
    // *(Entity_Component_ShapeRender*)getEntityComponent(controller, ref, EC_SHAPERENDER)         = shape;
    ((Entity_Component_Size*)getEntityComponent(controller, ref, EC_SIZE_BOUNDS))->size         = sf::IntRect(-9, -5, 18, 5);

    return ref;
}

void moveEntityToRoom(GameMap &map, MapRoomConnection connection, Entity_State_Controller &controller, Entity_Reference ref)
{
    sf::Vector2f roomDiffrence;
    roomDiffrence.x = map.room[connection.primaryRoom.id].bounds.left - map.room[connection.secondaryRoom.id].bounds.left;
    roomDiffrence.y = map.room[connection.primaryRoom.id].bounds.top  - map.room[connection.secondaryRoom.id].bounds.top;

    GET_EC_POSITION()->position += roomDiffrence;
    GET_EC_POSITION()->room     = connection.secondaryRoom.id;

    for(uint32 i = 0; i < map.room[connection.primaryRoom.id].entity_list.size(); i++)
    {
        if(map.room[connection.primaryRoom.id].entity_list[i].global_id == ref.global_id)
        {
            map.room[connection.primaryRoom.id].entity_list.erase(i + map.room[connection.primaryRoom.id].entity_list.begin());
        }
    }
    map.room[connection.secondaryRoom.id].entity_list.push_back(ref);
}

void insertion_sortUpdateOrder(vector<Entity_Reference> &list)
{
    uint32 j;
    for (uint32 i = 0; i < list.size(); i++){
        j = i;

        while (j > 0 && list[j].sort_key < list[j-1].sort_key)
        {
            iter_swap(list.begin()+j, list.begin()+j-1);
            j--;
        }
    }
}

sf::Vector2f localRoomPositionToScreen(GameMap &map, Entity_Component_Position &position)
{
    return sf::Vector2f((map.room[position.room].bounds.left + position.position.x) * map.tileSize.x,
                       (map.room[position.room].bounds.top  + position.position.y) * map.tileSize.y);
}


#endif /* end of include guard: ENTITY_CPP */
