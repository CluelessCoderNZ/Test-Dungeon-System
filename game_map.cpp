#ifndef GAME_MAP_CPP
#define GAME_MAP_CPP
#include "game_map.h"


MapDirection strToMapDirection(string a)
{
    for(uint32 i = 1; i < 5; i++)
    {
        if(kMapDirectionString[i] == a)
        {
            return (MapDirection)i;
        }
    }

    return DIR_NONE;
}

MapRoom loadTiledRoom_CSV(string filename, sf::Vector2u size)
{
    MapRoom output;
    output = createRoom(size, 0);

    stringstream data(fastReadFile(filename));
    uint32 i = 0;

    while(data.good())
    {
        string temp;
        getline(data, temp);
        stringstream line(temp);

        while(line.good())
        {
            string str_number;
            getline(line, str_number, ',');

            output.tilemap[i++].tileID = str2uint(str_number) + 1;
        }
    }

    return output;
}

MapRoom loadTiledRoom_JSON(string filename)
{
    MapRoom     output;
    Json::Value root = readJsonFile(filename);

    if(root.isMember("height") && root["height"].isUInt() && root.isMember("width") && root["width"].isUInt() &&
       root.isMember("tileheight") && root["tileheight"].isUInt() && root.isMember("tilewidth") && root["tilewidth"].isUInt())
    {
        output = createRoom(sf::Vector2u(root["width"].asUInt(), root["height"].asUInt()));
        if(root.isMember("layers") && root["layers"].isArray())
        {
            for(int16 i = 0; i < root["layers"][i].size(); i++)
            {
                if(root["layers"][i].isMember("name"))
                {
                    if(root["layers"][i]["name"].asString() == "Tile_Data")
                    {
                        if(root["layers"][i].isMember("data") && root["layers"][i]["data"].isArray())
                        {
                            for(int16 j = 0; j < root["layers"][i]["data"].size(); j++)
                            {
                                output.tilemap[j].tileID = root["layers"][i]["data"][j].asUInt();
                            }
                        }else{
                            // NOTE(Connor): Error handling
                            cout << "Error: Loading '"+filename+"' tile data missing" << endl;
                        }
                    }else if(root["layers"][i]["name"].asString() == "Meta_Data")
                    {
                        if(root["layers"][i].isMember("objects") && root["layers"][i]["objects"].isArray())
                        {
                            for(int16 j = 0; j < root["layers"][i]["objects"].size(); j++)
                            {
                                room_metadata_item item;
                                if(root["layers"][i]["objects"][j].isMember("x") && root["layers"][i]["objects"][j].isMember("y") &&
                                   root["layers"][i]["objects"][j]["x"].isUInt() && root["layers"][i]["objects"][j]["y"].isUInt())
                                {
                                    item.position.x = root["layers"][i]["objects"][j]["x"].asUInt() / root["tilewidth"].asUInt();
                                    item.position.y = root["layers"][i]["objects"][j]["y"].asUInt() / root["tileheight"].asUInt();
                                }
                                if(root["layers"][i]["objects"][j].isMember("type") && root["layers"][i]["objects"][j]["type"].isString())
                                {
                                    item.type = root["layers"][i]["objects"][j]["type"].asString();

                                    if(item.type == "Door")
                                        output.doorsFree++;
                                }
                                if(root["layers"][i]["objects"][j].isMember("properties") && root["layers"][i]["objects"][j]["properties"].isArray())
                                {
                                    item.data    = root["layers"][i]["objects"][j]["properties"];
                                    item.hasData = true;
                                }

                                output.metadata.push_back(item);
                            }
                        }else{
                            // NOTE(Connor): Error handling
                            cout << "Error: Loading '"+filename+"' missing object meta data" << endl;
                        }
                    }else{
                        // NOTE(Connor): Error handling
                        cout << "Error: Loading '"+filename+"' unknown map layer '"+root["layers"][i]["name"].asString()+"'" << endl;
                    }
                }else{
                    // NOTE(Connor): Error handling
                    cout << "Error: Loading '"+filename+"' no map layer name/type specified" << endl;
                }
            }

        }else{
            // NOTE(Connor): Error handling
            cout << "Error: Loading '"+filename+"' no height or width specified" << endl;
        }
    }else{
        // NOTE(Connor): Error handling
        cout << "Error: Loading '"+filename+"' no height or width specified" << endl;
    }
    return output;
}

MapRoom loadTiledRoom(string filename, sf::Vector2u size)
{
    string filetype = filename.substr(filename.find_last_of(".")+1);
    MapRoom output = MapRoom();

    if(filetype == "csv")
    {
        output = loadTiledRoom_CSV(filename, size);
    }else if(filetype == "json")
    {
        output = loadTiledRoom_JSON(filename);
    }else{
        // NOTE(Connor): Error handling
        cout << "Error: Loading '"+filename+"' unknown filetype" << endl;
    }
    return output;
}

MapRoom loadRoomFromChanceSplit(RoomIndexConfigFile &file, real32 splitPoint)
{
    assert(splitPoint <= 1 && splitPoint >= 0);

    MapRoom room;

    uint32 targetChance = splitPoint * file.chanceWeightTotal;
    uint32 chanceCurrent = 0;
    for(int i = 0; i < file.root["RoomData"].size(); i++)
    {
        chanceCurrent += file.root["RoomData"][i]["ChanceWeight"].asUInt();
        if(targetChance < chanceCurrent)
        {
            room = loadTiledRoom(file.folderDirectory + file.root["RoomData"][i]["Name"].asString());
            room.room_type = i+1;
            break;
        }
    }

    return room;
}

RoomIndexConfigFile loadRoomIndexConfigFile(string filename)
{
    RoomIndexConfigFile file;
    file.filename           = filename;
    file.folderDirectory    = filename.substr(0,filename.find_last_of('/')+1);
    file.root               = readJsonFile(filename);

    for(int i = 0; i < file.root["RoomData"].size(); i++)
    {
        file.chanceWeightTotal += file.root["RoomData"][i]["ChanceWeight"].asUInt();
    }

    return file;
}

vector<sf::Vector2i> getRoomToRoomPath(GameMap &map, sf::Vector2u corridor, MapRoom_GraphEdge edge)
{
    uint32 maxPathDepth = edge.length*2;

    sf::IntRect corridorSize(0, 0, corridor.x, corridor.y);
    int32      wallSize = 1;

    sf::Vector2i startLocation  = edge.p1.point;
    sf::Vector2i targetLocation = edge.p2.point;

    // Place Locations in viable positions
    switch(edge.p1.doorDirection)
    {
        case DIR_NORTH:
        {
            startLocation.x -= wallSize;
            startLocation.y -= corridorSize.height;
        }break;
        case DIR_SOUTH:
        {
            startLocation.x -= wallSize;
            startLocation.y++;
        }break;
        case DIR_EAST:
        {
            startLocation.y -= wallSize;
            startLocation.x++;
        }break;
        case DIR_WEST:
        {
            startLocation.y -= wallSize;
            startLocation.x -= corridorSize.width;
        }break;
    }

    switch(edge.p2.doorDirection)
    {
        case DIR_NORTH:
        {
            targetLocation.x -= wallSize;
            targetLocation.y -= corridorSize.height;
        }break;
        case DIR_SOUTH:
        {
            targetLocation.x -= wallSize;
            targetLocation.y++;
        }break;
        case DIR_EAST:
        {
            targetLocation.y -= wallSize;
            targetLocation.x++;
        }break;
        case DIR_WEST:
        {
            targetLocation.y -= wallSize;
            targetLocation.x -= corridorSize.width;
        }break;
    }

    struct Node
    {
        sf::Vector2i position;
        sf::Vector2i parentDirection;
        int32        parent    = -1;
        real32       cost      = 0;
        real32       heuristic = 0;
    };

    const uint32       kTileNeighbourCount = 4;
    const sf::Vector2i kTileNeighbours[] =
    {
        sf::Vector2i(0 ,1 ),
        sf::Vector2i(0 ,-1),
        sf::Vector2i(1 ,0 ),
        sf::Vector2i(-1,0 )
    };

    vector<Node> openList;
    vector<Node> closedList;

    openList.resize(1);
    openList[0].position  = startLocation;
    openList[0].heuristic = edge.length;
    openList[0].parentDirection = kMapDirectionNormals[edge.p1.doorDirection];

    while(openList.size() > 0)
    {
        Node current = openList[0];
        openList.erase(openList.begin());
        closedList.push_back(current);
        if(current.position == targetLocation)
        {
            break;
        }
        // Add Neighbours
        for(uint32 i = 0; i < kTileNeighbourCount && current.cost < maxPathDepth; i++)
        {
            Node neighbour;
            neighbour.position = current.position + kTileNeighbours[i];

            bool isValid = true;
            corridorSize.left = neighbour.position.x;
            corridorSize.top  = neighbour.position.y;
            // Check For Collisions
            for(uint32 j = 0; j < map.room.size(); j++)
            {
                if(map.room[j].bounds.intersects(corridorSize))
                {
                    isValid = false;
                    break;
                }
            }

            if(isValid)
            {
                // Check if exists
                for(uint32 j = 0; j < closedList.size(); j++)
                {
                    if(closedList[j].position == neighbour.position)
                    {
                        isValid = false;
                        break;
                    }
                }

                if(isValid)
                {
                    // Calculate Heuristics
                    neighbour.cost            = current.cost+1;
                    neighbour.heuristic       = getManhattanDistance(neighbour.position, targetLocation);
                    neighbour.parentDirection = kTileNeighbours[i];
                    neighbour.parent          = closedList.size()-1;

                    // Check and remove from open list if better
                    for(uint32 j = 0; j < openList.size(); j++)
                    {
                        if(openList[j].position == neighbour.position)
                        {
                            if(openList[j].cost > neighbour.cost)
                            {
                                openList.erase(openList.begin()+j);
                            }else{
                                isValid = false;
                            }
                            break;
                        }
                    }

                    // Add to Open List
                    if(isValid)
                    {
                        bool needsAdded = true;
                        for(uint32 j = 0; j < openList.size(); j++)
                        {
                            if(openList[j].cost + openList[j].heuristic > neighbour.cost + neighbour.heuristic)
                            {
                                openList.insert(openList.begin() + j, neighbour);
                                needsAdded = false;
                                break;
                            }
                        }
                        if(needsAdded)
                        {
                            openList.push_back(neighbour);
                        }
                    }
                }
            }
        }

    }

    vector<sf::Vector2i> path;
    int32        parent_id = closedList.size()-1;
    while(parent_id >= 0)
    {
        path.push_back(closedList[parent_id].position);
        parent_id = closedList[parent_id].parent;
    }

    vector<sf::Vector2i> output;

    sf::Vector2i lastDirection;
    for(uint32 i = 1; i < path.size(); i++)
    {
        sf::Vector2i direction = path[i-1] - path[i];
        if(lastDirection != direction)
        {
            output.push_back(path[i - 1]);
        }
        lastDirection = direction;
    }
    output.push_back(path[path.size()-1]);

    return output;
}

void generateHomeDistanceForRoom(GameMap &map, uint32 room_id, uint32 steps, int32 sourceEdge)
{
    map.room[room_id].steps_fromHomeRoom = steps;
    for(uint32 i = 0; i < map.graphMap.size(); i++)
    {
        if(sourceEdge == i || !map.graphMap[i].is_main_branch)
            continue;

        if(map.graphMap[i].p1.room.id == room_id)
        {
            generateHomeDistanceForRoom(map, map.graphMap[i].p2.room.id, steps+1, i);
        }else if(map.graphMap[i].p2.room.id == room_id)
        {
            generateHomeDistanceForRoom(map, map.graphMap[i].p1.room.id, steps+1, i);
        }
    }
}

GameMap generateRandomGenericDungeon(uint32 seed, string roomdata_filename)
{
    cout << "Generating Map with seed " << seed << endl;
    GameMap     map;
    RoomIndexConfigFile indexFile = loadRoomIndexConfigFile(roomdata_filename);

    mt19937     random_engine(seed);
    uniform_real_distribution<real32> dist_real32(0.0f,1.0f);

    // Randomly place rooms
    uint32          generatedRoomMininum= 10;
    uint32          roomMarginSize      = 9;
    sf::Vector2u    generationRoomRange = sf::Vector2u(150,150);
    sf::Vector2u    startPosition       = sf::Vector2u(5000,5000);
    for(uint32 i = 0; i < generatedRoomMininum || dist_real32(random_engine) > 1.0/50.0;)
    {
        MapRoom room = loadRoomFromChanceSplit(indexFile, dist_real32(random_engine));
        // NOTE(Connor): If wanting to be truly in range take away the room size
        room.bounds.left = startPosition.x + generationRoomRange.x * dist_real32(random_engine);
        room.bounds.top  = startPosition.y + generationRoomRange.y * dist_real32(random_engine);

        // Check for collision
        b32 success = true;
        sf::IntRect roomMarginBounds(room.bounds.left - roomMarginSize, room.bounds.top - roomMarginSize, 2 * roomMarginSize  + room.bounds.width, 2 * roomMarginSize + room.bounds.height);
        for(uint32 i = 0; i < map.room.size(); i++)
        {
            if(map.room[i].bounds.intersects(roomMarginBounds))
            {
                success = false;
                break;
            }
        }

        if(success)
        {
            room.room_group = i;
            map.room.push_back(room);
            i++;
        }
    }

    {   // Generate Graph Map of room's doors
        vector<Vec2f> points;

        for(uint32 i = 0; i < map.room.size(); i++)
        {
            for(int j = 0; j < map.room[i].metadata.size(); j++)
            {
                if(map.room[i].metadata[j].type == "Door")
                {
                    points.push_back(Vec2f(map.room[i].bounds.left + map.room[i].metadata[j].position.x, map.room[i].bounds.top + map.room[i].metadata[j].position.y));
                }
            }
        }
        // Generate graph node edges through triangulation
        Delaunay triangulation;
        triangulation.triangulate(points);
        vector<Edge> rawedges = triangulation.getEdges();
        vector<MapRoom_GraphEdge> sortedGraphMap;

        // Cull inter-room edges and sorts for MST
        for(uint32 i = 0; i < rawedges.size(); i++)
        {
            MapRoom_GraphEdge edge;

            uint found = 0;
            for(uint32 x = 0; x < map.room.size() && found != 2; x++)
            {
                if(map.room[x].bounds.contains(rawedges[i].p1.x, rawedges[i].p1.y))
                {
                    edge.p1.point   = sf::Vector2i(rawedges[i].p1.x, rawedges[i].p1.y);
                    edge.p1.room.id = x;
                    found++;
                }

                if(map.room[x].bounds.contains(rawedges[i].p2.x, rawedges[i].p2.y))
                {
                    edge.p2.point   = sf::Vector2i(rawedges[i].p2.x, rawedges[i].p2.y);
                    edge.p2.room.id = x;
                    found++;
                }
            }

            if(found == 2 && edge.p1.room.id != edge.p2.room.id)
            {
                edge.length = getDistance(edge.p1.point, edge.p2.point);

                // Find Room Door Direction
                int room_id   = edge.p1.room.id;
                for(int j = 0; j < map.room[room_id].metadata.size(); j++)
                {
                    if(map.room[room_id].metadata[j].type == "Door")
                    {
                        if(map.room[room_id].metadata[j].position.x + map.room[room_id].bounds.left == edge.p1.point.x &&
                           map.room[room_id].metadata[j].position.y + map.room[room_id].bounds.top == edge.p1.point.y)
                        {
                            if(map.room[room_id].metadata[j].position.x == 0)                                       edge.p1.doorDirection=DIR_WEST;
                            else if(map.room[room_id].metadata[j].position.y == 0)                                  edge.p1.doorDirection=DIR_NORTH;
                            else if(map.room[room_id].metadata[j].position.x == map.room[room_id].bounds.width-1)   edge.p1.doorDirection=DIR_EAST;
                            else if(map.room[room_id].metadata[j].position.y == map.room[room_id].bounds.height-1)  edge.p1.doorDirection=DIR_SOUTH;

                            edge.p1.doorID = j;
                            break;
                        }
                    }
                }

                room_id   = edge.p2.room.id;
                for(int j = 0; j < map.room[room_id].metadata.size(); j++)
                {
                    if(map.room[room_id].metadata[j].type == "Door")
                    {
                        if(map.room[room_id].metadata[j].position.x + map.room[room_id].bounds.left == edge.p2.point.x &&
                           map.room[room_id].metadata[j].position.y + map.room[room_id].bounds.top == edge.p2.point.y)
                        {
                            if(map.room[room_id].metadata[j].position.x == 0)                                       edge.p2.doorDirection=DIR_WEST;
                            else if(map.room[room_id].metadata[j].position.y == 0)                                  edge.p2.doorDirection=DIR_NORTH;
                            else if(map.room[room_id].metadata[j].position.x == map.room[room_id].bounds.width-1)   edge.p2.doorDirection=DIR_EAST;
                            else if(map.room[room_id].metadata[j].position.y == map.room[room_id].bounds.height-1)  edge.p2.doorDirection=DIR_SOUTH;

                            edge.p2.doorID = j;
                            break;
                        }
                    }
                }

                // TODO(Connor): Replace with a binary sort for log(n) preformance
                bool needAdded=true;
                for(uint32 i = 0; i < sortedGraphMap.size(); i++)
                {
                    if(sortedGraphMap[i].length > edge.length)
                    {
                        sortedGraphMap.insert(sortedGraphMap.begin()+i, edge);
                        needAdded=false;
                        break;
                    }
                }
                if(needAdded)
                {
                    sortedGraphMap.push_back(edge);
                }
            }
        }

        // Generate mininium spanning tree from graph map
        // ----------------------------------------------

        // Generate disjointed set of rooms to organise connections
        struct roomDoorID
        {
            uint32 room;
            uint32 door;
        };

        vector<roomDoorID> inUseDoors;
        vector<uint32> disjointedSetRoomParents;
        for(uint32 i = 0; i < map.room.size(); i++)
        {
            disjointedSetRoomParents.push_back(i);
        }

        for(uint32 i = 0 ; i < sortedGraphMap.size(); i++)
        {
            // Check if unique
            uint32 set_p1 = disjointedSetRoomParents[sortedGraphMap[i].p1.room.id];
            uint32 set_p2 = disjointedSetRoomParents[sortedGraphMap[i].p2.room.id];

            bool allowLoop=dist_real32(random_engine) > 0.99;

            if(set_p1 != set_p2 || allowLoop)
            {
                bool isValid=true;

                // Search if door is already in use
                for(uint32 j = 0; j < inUseDoors.size(); j++)
                {
                    if((sortedGraphMap[i].p1.room.id == inUseDoors[j].room &&
                        sortedGraphMap[i].p1.doorID  == inUseDoors[j].door) ||
                       (sortedGraphMap[i].p2.room.id == inUseDoors[j].room &&
                        sortedGraphMap[i].p2.doorID  == inUseDoors[j].door))
                    {
                        isValid=false;
                        break;
                    }
                }

                if(allowLoop)
                {
                    // Search if Room Connection already exists
                    // This is to make loops more intresting
                    uint32 min_current_room_id = min(sortedGraphMap[i].p1.room.id, sortedGraphMap[i].p2.room.id);
                    uint32 max_current_room_id = max(sortedGraphMap[i].p1.room.id, sortedGraphMap[i].p2.room.id);
                    for(uint32 j = 0;j < map.graphMap.size(); j++)
                    {
                        uint32 min_exists_room_id = min(map.graphMap[j].p1.room.id, map.graphMap[j].p2.room.id);
                        uint32 max_exists_room_id = max(map.graphMap[j].p1.room.id, map.graphMap[j].p2.room.id);

                        if(min_current_room_id == min_exists_room_id &&
                           max_current_room_id == max_exists_room_id)
                        {
                            isValid = false;
                            break;
                        }
                    }
                }

                if(isValid)
                {
                    uint32 newSet = max(set_p1, set_p2);
                    uint32 oldSet = min(set_p1, set_p2);
                    for(uint32 x = 0; x < disjointedSetRoomParents.size(); x++)
                    {
                        if(disjointedSetRoomParents[x] == oldSet)
                        {
                            disjointedSetRoomParents[x] = newSet;
                        }
                    }

                    map.graphMap.push_back(sortedGraphMap[i]);
                    map.graphMap[map.graphMap.size()-1].is_main_branch = !allowLoop;

                    roomDoorID id;
                    id.door = sortedGraphMap[i].p1.doorID;
                    id.room = sortedGraphMap[i].p1.room.id;
                    inUseDoors.push_back(id);
                    id.door = sortedGraphMap[i].p2.doorID;
                    id.room = sortedGraphMap[i].p2.room.id;
                    inUseDoors.push_back(id);
                }
            }
        }
    }

    // Generate Hallways from graphMap
    uint32 hallwayWidth = 5;
    uint32 hallwayWallSize = 1;
    uint32 roomGroupStart = map.room.size();

    for(uint32 i = 0; i < map.graphMap.size(); i++)
    {
        vector<sf::Vector2i> path;

        /*bool           createMidIntersection = false;
        sf::Vector2i   midpoint = sf::Vector2i((map.graphMap[i].p1.point.x+map.graphMap[i].p2.point.x)/2,(map.graphMap[i].p1.point.y+map.graphMap[i].p2.point.y)/2);

        if(kMapDirectionNormals[map.graphMap[i].p1.doorDirection].x + kMapDirectionNormals[map.graphMap[i].p2.doorDirection].x == 0 &&
           kMapDirectionNormals[map.graphMap[i].p1.doorDirection].y + kMapDirectionNormals[map.graphMap[i].p2.doorDirection].y == 0 &&
           !doesRectContainRoom(map, sf::IntRect(midpoint.x-hallwayWallSize, midpoint.y-hallwayWallSize, hallwayWidth, hallwayWidth)))
        {
            createMidIntersection=true;
        }

        if(createMidIntersection)
        {
            path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), map.graphMap[i]);
        }else{
            path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), map.graphMap[i]);
        }*/
        path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), map.graphMap[i]);

        map.graphMap[i].graphPath=path;


        int32 LAST_MIN_X, LAST_MIN_Y, LAST_MAX_X, LAST_MAX_Y;

        // Create Hallway Rooms
        for(uint32 j = 1; j < path.size(); j++)
        {
            // Room Size
            int32 MIN_X = min(path[j].x, path[j-1].x);
            int32 MIN_Y = min(path[j].y, path[j-1].y);
            int32 MAX_X = max(path[j].x, path[j-1].x) + hallwayWidth;
            int32 MAX_Y = max(path[j].y, path[j-1].y) + hallwayWidth;

            int32 NonAdjusted_MIN_X = MIN_X;
            int32 NonAdjusted_MAX_X = MAX_X;
            int32 NonAdjusted_MIN_Y = MIN_Y;
            int32 NonAdjusted_MAX_Y = MAX_Y;

            // Last Room cropping
            if(j > 1)
            {
                if(MAX_X - MIN_X == hallwayWidth)
                {
                    // NORTH
                    if(path[j].y == MIN_Y)
                    {
                        MAX_Y = LAST_MIN_Y;
                    // SOUTH
                    }else{
                        MIN_Y = LAST_MAX_Y;
                    }
                }else{
                    // WEST
                    if(path[j].x == MIN_X)
                    {
                        MAX_X = LAST_MIN_X;
                    // EAST
                    }else{
                        MIN_X = LAST_MAX_X;
                    }
                }
            }

            if(MAX_X - MIN_X > 0 && MAX_Y - MIN_Y > 0)
            {
                // Create Room of walls
                MapRoom room = createRoom(sf::Vector2u(MAX_X - MIN_X, MAX_Y - MIN_Y), 2);
                room.room_group = roomGroupStart+i;

                room.bounds.left = MIN_X;
                room.bounds.top  = MIN_Y;

                map.room.push_back(room);

                // Connect To Start Room
                MapRoomConnection connection;
                connection.primaryRoom.id   = map.graphMap[i].p2.room.id;
                connection.secondaryRoom.id = map.room.size()-1;
                map.room[connection.primaryRoom.id].connection.push_back(connection);
                swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                map.room[connection.primaryRoom.id].connection.push_back(connection);

                // Connect to last hallway
                for(uint32 x = 1; x < j; x++)
                {
                    connection.primaryRoom.id   = map.room.size()-1;
                    connection.secondaryRoom.id = map.room.size()-x-1;
                    map.room[connection.primaryRoom.id].connection.push_back(connection);
                    swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                    map.room[connection.primaryRoom.id].connection.push_back(connection);
                }

                // Connect To Target Room
                connection.primaryRoom.id   = map.graphMap[i].p1.room.id;
                connection.secondaryRoom.id = map.room.size()-1;
                map.room[connection.primaryRoom.id].connection.push_back(connection);
                swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                map.room[connection.primaryRoom.id].connection.push_back(connection);

                // Set Last Values for next iteration cropping
                LAST_MIN_X = MIN_X;
                LAST_MAX_X = MAX_X;
                LAST_MIN_Y = MIN_Y;
                LAST_MAX_Y = MAX_Y;
            }
        }

        // Clear hallway paths
        for(uint32 j = 1; j < path.size(); j++)
        {
            // Hallway Empty Path
            int32 MIN_X = min(path[j].x, path[j-1].x) + hallwayWallSize;
            int32 MIN_Y = min(path[j].y, path[j-1].y) + hallwayWallSize;
            int32 MAX_X = max(path[j].x, path[j-1].x) + hallwayWidth - hallwayWallSize*2;
            int32 MAX_Y = max(path[j].y, path[j-1].y) + hallwayWidth - hallwayWallSize*2;

            for(uint32 y = MIN_Y; y <= MAX_Y; y++)
            {
                for(uint32 x = MIN_X; x <= MAX_X; x++)
                {
                    setTileFromGlobalPos(map, sf::Vector2i(x,y), 1);
                }
            }
        }

        // Clear Entrance and Exit
        sf::IntRect roomConnectorBlock;
        switch(map.graphMap[i].p1.doorDirection)
        {
            case DIR_NORTH:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p1.point.x,map.graphMap[i].p1.point.y-hallwayWallSize,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_SOUTH:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p1.point.x,map.graphMap[i].p1.point.y,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_EAST:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p1.point.x,map.graphMap[i].p1.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
            case DIR_WEST:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p1.point.x-hallwayWallSize,map.graphMap[i].p1.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
        }
        setTileAreaFromGlobalPos(map, roomConnectorBlock, 1);
        switch(map.graphMap[i].p2.doorDirection)
        {
            case DIR_NORTH:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p2.point.x,map.graphMap[i].p2.point.y-hallwayWallSize,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_SOUTH:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p2.point.x,map.graphMap[i].p2.point.y,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_EAST:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p2.point.x,map.graphMap[i].p2.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
            case DIR_WEST:
            {
                roomConnectorBlock = sf::IntRect(map.graphMap[i].p2.point.x-hallwayWallSize,map.graphMap[i].p2.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
        }
        setTileAreaFromGlobalPos(map, roomConnectorBlock, 1);
    }

    // Sort map rooms by height
    generateSortKeysForMap(map);

    return map;
}

bool generateRoomClusterNode(GameMap &map, mt19937 &random_engine, RoomIndexConfigFile &indexFile, MapRoom_Refrence parentRoom, uint32 clusterCount, real32 maxRoomDistance, real32 minRoomDistance, uint32 roomBoundaryExtend, MapRoom *enforceRoom)
{
    vector<uint32> clusterRoomList;
    uniform_real_distribution<real32> dist_real32(0.0f,1.0f);
    uint32 startIndex = map.room.size();
    clusterRoomList.push_back(parentRoom.id);

    uint32 failCount = 0;

    while(map.room.size()-startIndex < clusterCount)
    {
        uniform_int_distribution<uint32> dist_int(0,map.room.size()-startIndex);
        uint32 targetRoom = dist_int(random_engine);

        // Get target room
        if(targetRoom == 0)
            targetRoom = parentRoom.id;
        else
            targetRoom = targetRoom-1+startIndex;

        MapRoom room;
        if(enforceRoom == NULL)
        {
            room = loadRoomFromChanceSplit(indexFile, dist_real32(random_engine));
        }else{
            room = *enforceRoom;
        }
        real32 angle = dist_real32(random_engine)*kTAU;

        sf::Vector2f perimeterTargetRoomPoint = getPerimeterPointByAngle(map.room[targetRoom].bounds, angle);
        sf::Vector2f perimeterNewRoomPoint = getPerimeterPointByAngle(room.bounds, angle-kPI);
        real32 distance = minRoomDistance + dist_real32(random_engine)*(maxRoomDistance-minRoomDistance);
        angle = getAngle(sf::Vector2f(map.room[targetRoom].bounds.width/2.0, map.room[targetRoom].bounds.height/2.0), perimeterNewRoomPoint);

        room.bounds.left = map.room[targetRoom].bounds.left + perimeterTargetRoomPoint.x + cos(angle)*distance - perimeterNewRoomPoint.x;
        room.bounds.top  = map.room[targetRoom].bounds.top  + perimeterTargetRoomPoint.y + sin(angle)*distance - perimeterNewRoomPoint.y;


        sf::IntRect roomBoundaryZone = room.bounds;
        roomBoundaryZone.left   -= roomBoundaryExtend;
        roomBoundaryZone.top    -= roomBoundaryExtend;
        roomBoundaryZone.width  += roomBoundaryExtend*2;
        roomBoundaryZone.height += roomBoundaryExtend*2;

        // Check for collisions
        bool NoCollision = true;
        for(uint32 i = 0; i < map.room.size() && NoCollision; i++)
        {
            if(map.room[i].bounds.intersects(roomBoundaryZone))
            {
                NoCollision = false;
            }
        }

        // Add room to list
        if(NoCollision)
        {
            room.room_group = map.room.size()-1;
            map.room.push_back(room);
            clusterRoomList.push_back(map.room.size()-1);
        }else if(targetRoom = parentRoom.id){
            failCount++;
            if(failCount > 10)
            {
                return false;
            }
        }
    }

    vector<MapRoom_GraphEdge> clusterGraphMap;

    generateGraphMap(map, clusterGraphMap, clusterRoomList, random_engine);
    createHallwaysFromGraphMap(map, clusterGraphMap, 5, 1);

    map.graphMap.insert(map.graphMap.begin(), clusterGraphMap.begin(), clusterGraphMap.end());

    return true;
}

void generateGraphMap(GameMap &map, vector<MapRoom_GraphEdge> &outputMap, vector<uint32> &roomList, mt19937 &random_engine)
{
    // Create list of all door points
    vector<Vec2f> doorPoints;
    for(uint32 listID = 0; listID < roomList.size(); listID++)
    {
        uint32 i = roomList[listID];
        for(int j = 0; j < map.room[i].metadata.size(); j++)
        {
            if(map.room[i].metadata[j].type == "Door" && !map.room[i].metadata[j].data.isMember("InUse"))
            {
                doorPoints.push_back(Vec2f(map.room[i].bounds.left + map.room[i].metadata[j].position.x, map.room[i].bounds.top + map.room[i].metadata[j].position.y));
            }
        }
    }

    // Generate door node edges through triangulation
    Delaunay triangulation;
    vector<Edge> rawedges;

    if(doorPoints.size() > 0)
    {
        triangulation.triangulate(doorPoints);
        rawedges = triangulation.getEdges();
    }



    vector<MapRoom_GraphEdge> sortedGraphMap;

    for(uint32 i = 0; i < rawedges.size(); i++)
    {
        MapRoom_GraphEdge edge;

        uint found = 0;
        for(uint32 listID = 0; listID < roomList.size() && found != 2; listID++)
        {
            uint32 x = roomList[listID];
            if(map.room[x].bounds.contains(rawedges[i].p1.x, rawedges[i].p1.y))
            {
                edge.p1.point   = sf::Vector2i(rawedges[i].p1.x, rawedges[i].p1.y);
                edge.p1.room.id = x;
                found++;
            }

            if(map.room[x].bounds.contains(rawedges[i].p2.x, rawedges[i].p2.y))
            {
                edge.p2.point   = sf::Vector2i(rawedges[i].p2.x, rawedges[i].p2.y);
                edge.p2.room.id = x;
                found++;
            }
        }

        if(found == 2 && edge.p1.room.id != edge.p2.room.id)
        {
            edge.length = getDistance(edge.p1.point, edge.p2.point);

            // Find Room Door Direction
            int room_id   = edge.p1.room.id;
            for(int j = 0; j < map.room[room_id].metadata.size(); j++)
            {
                if(map.room[room_id].metadata[j].type == "Door")
                {
                    if(map.room[room_id].metadata[j].position.x + map.room[room_id].bounds.left == edge.p1.point.x &&
                       map.room[room_id].metadata[j].position.y + map.room[room_id].bounds.top == edge.p1.point.y)
                    {
                        if(map.room[room_id].metadata[j].position.x == 0)                                       edge.p1.doorDirection=DIR_WEST;
                        else if(map.room[room_id].metadata[j].position.y == 0)                                  edge.p1.doorDirection=DIR_NORTH;
                        else if(map.room[room_id].metadata[j].position.x == map.room[room_id].bounds.width-1)   edge.p1.doorDirection=DIR_EAST;
                        else if(map.room[room_id].metadata[j].position.y == map.room[room_id].bounds.height-1)  edge.p1.doorDirection=DIR_SOUTH;

                        edge.p1.doorID = j;
                        break;
                    }
                }
            }

            room_id   = edge.p2.room.id;
            for(int j = 0; j < map.room[room_id].metadata.size(); j++)
            {
                if(map.room[room_id].metadata[j].type == "Door")
                {
                    if(map.room[room_id].metadata[j].position.x + map.room[room_id].bounds.left == edge.p2.point.x &&
                       map.room[room_id].metadata[j].position.y + map.room[room_id].bounds.top == edge.p2.point.y)
                    {
                        if(map.room[room_id].metadata[j].position.x == 0)                                       edge.p2.doorDirection=DIR_WEST;
                        else if(map.room[room_id].metadata[j].position.y == 0)                                  edge.p2.doorDirection=DIR_NORTH;
                        else if(map.room[room_id].metadata[j].position.x == map.room[room_id].bounds.width-1)   edge.p2.doorDirection=DIR_EAST;
                        else if(map.room[room_id].metadata[j].position.y == map.room[room_id].bounds.height-1)  edge.p2.doorDirection=DIR_SOUTH;

                        edge.p2.doorID = j;
                        break;
                    }
                }
            }

            // TODO(Connor): Replace with a binary sort for log(n) preformance
            bool needAdded=true;
            for(uint32 i = 0; i < sortedGraphMap.size(); i++)
            {
                if(sortedGraphMap[i].length > edge.length)
                {
                    sortedGraphMap.insert(sortedGraphMap.begin()+i, edge);
                    needAdded=false;
                    break;
                }
            }
            if(needAdded)
            {
                sortedGraphMap.push_back(edge);
            }
        }
    }

    // Generate mininium spanning tree from graph map
    // ----------------------------------------------
    uniform_real_distribution<real32> dist_real32(0.0f,1.0f);

    // Generate disjointed set of rooms to organise connections
    struct roomDoorID
    {
        uint32 room;
        uint32 door;
    };

    vector<roomDoorID> inUseDoors;
    unordered_map<uint32, uint32> disjointedSetRoomParents;
    for(uint32 i = 0; i < roomList.size(); i++)
    {
        disjointedSetRoomParents[roomList[i]] = roomList[i];
    }

    for(uint32 i = 0 ; i < sortedGraphMap.size(); i++)
    {
        // Check if unique
        uint32 set_p1 = disjointedSetRoomParents[sortedGraphMap[i].p1.room.id];
        uint32 set_p2 = disjointedSetRoomParents[sortedGraphMap[i].p2.room.id];

        bool allowLoop=dist_real32(random_engine) > 0.99;

        if(set_p1 != set_p2 || allowLoop)
        {
            bool isValid=true;

            // Search if door is already in use
            for(uint32 j = 0; j < inUseDoors.size(); j++)
            {
                if((sortedGraphMap[i].p1.room.id == inUseDoors[j].room &&
                    sortedGraphMap[i].p1.doorID  == inUseDoors[j].door) ||
                   (sortedGraphMap[i].p2.room.id == inUseDoors[j].room &&
                    sortedGraphMap[i].p2.doorID  == inUseDoors[j].door))
                {
                    isValid=false;
                    break;
                }
            }

            if(allowLoop)
            {
                // Search if Room Connection already exists
                // This is to make loops more intresting
                uint32 min_current_room_id = min(sortedGraphMap[i].p1.room.id, sortedGraphMap[i].p2.room.id);
                uint32 max_current_room_id = max(sortedGraphMap[i].p1.room.id, sortedGraphMap[i].p2.room.id);
                for(uint32 j = 0;j < outputMap.size(); j++)
                {
                    uint32 min_exists_room_id = min(outputMap[j].p1.room.id, outputMap[j].p2.room.id);
                    uint32 max_exists_room_id = max(outputMap[j].p1.room.id, outputMap[j].p2.room.id);

                    if(min_current_room_id == min_exists_room_id &&
                       max_current_room_id == max_exists_room_id)
                    {
                        isValid = false;
                        break;
                    }
                }
            }

            if(isValid)
            {
                uint32 newSet = max(set_p1, set_p2);
                uint32 oldSet = min(set_p1, set_p2);
                for(uint32 x = 0; x < disjointedSetRoomParents.size(); x++)
                {
                    if(disjointedSetRoomParents[x] == oldSet)
                    {
                        disjointedSetRoomParents[x] = newSet;
                    }
                }
                map.room[sortedGraphMap[i].p1.room.id].doorsFree--;
                map.room[sortedGraphMap[i].p2.room.id].doorsFree--;
                outputMap.push_back(sortedGraphMap[i]);
                outputMap[outputMap.size()-1].is_main_branch = !allowLoop;

                roomDoorID id;
                id.door = sortedGraphMap[i].p1.doorID;
                id.room = sortedGraphMap[i].p1.room.id;
                inUseDoors.push_back(id);
                map.room[id.room].metadata[id.door].data["InUse"] = true;
                id.door = sortedGraphMap[i].p2.doorID;
                id.room = sortedGraphMap[i].p2.room.id;
                inUseDoors.push_back(id);
                map.room[id.room].metadata[id.door].data["InUse"] = true;


            }
        }
    }
}

void createHallwaysFromGraphMap(GameMap &map, vector<MapRoom_GraphEdge> &graphMap, uint32 hallwayWidth, uint32 hallwayWallSize)
{
    uint32 roomGroupStart = map.room.size();

    for(uint32 i = 0; i < graphMap.size(); i++)
    {
        vector<sf::Vector2i> path;

        /*bool           createMidIntersection = false;
        sf::Vector2i   midpoint = sf::Vector2i((graphMap[i].p1.point.x+graphMap[i].p2.point.x)/2,(graphMap[i].p1.point.y+graphMap[i].p2.point.y)/2);

        if(kMapDirectionNormals[graphMap[i].p1.doorDirection].x + kMapDirectionNormals[graphMap[i].p2.doorDirection].x == 0 &&
           kMapDirectionNormals[graphMap[i].p1.doorDirection].y + kMapDirectionNormals[graphMap[i].p2.doorDirection].y == 0 &&
           !doesRectContainRoom(map, sf::IntRect(midpoint.x-hallwayWallSize, midpoint.y-hallwayWallSize, hallwayWidth, hallwayWidth)))
        {
            createMidIntersection=true;
        }

        if(createMidIntersection)
        {
            path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), graphMap[i]);
        }else{
            path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), graphMap[i]);
        }*/
        path = getRoomToRoomPath(map, sf::Vector2u(hallwayWidth, hallwayWidth), graphMap[i]);

        graphMap[i].graphPath=path;


        int32 LAST_MIN_X, LAST_MIN_Y, LAST_MAX_X, LAST_MAX_Y;

        // Create Hallway Rooms
        for(uint32 j = 1; j < path.size(); j++)
        {
            // Room Size
            int32 MIN_X = min(path[j].x, path[j-1].x);
            int32 MIN_Y = min(path[j].y, path[j-1].y);
            int32 MAX_X = max(path[j].x, path[j-1].x) + hallwayWidth;
            int32 MAX_Y = max(path[j].y, path[j-1].y) + hallwayWidth;

            int32 NonAdjusted_MIN_X = MIN_X;
            int32 NonAdjusted_MAX_X = MAX_X;
            int32 NonAdjusted_MIN_Y = MIN_Y;
            int32 NonAdjusted_MAX_Y = MAX_Y;

            // Last Room cropping
            if(j > 1)
            {
                if(MAX_X - MIN_X == hallwayWidth)
                {
                    // NORTH
                    if(path[j].y == MIN_Y)
                    {
                        MAX_Y = LAST_MIN_Y;
                    // SOUTH
                    }else{
                        MIN_Y = LAST_MAX_Y;
                    }
                }else{
                    // WEST
                    if(path[j].x == MIN_X)
                    {
                        MAX_X = LAST_MIN_X;
                    // EAST
                    }else{
                        MIN_X = LAST_MAX_X;
                    }
                }
            }

            if(MAX_X - MIN_X > 0 && MAX_Y - MIN_Y > 0)
            {
                // Create Room of walls
                MapRoom room = createRoom(sf::Vector2u(MAX_X - MIN_X, MAX_Y - MIN_Y), 2);
                room.room_group = roomGroupStart+i;

                room.bounds.left = MIN_X;
                room.bounds.top  = MIN_Y;

                map.room.push_back(room);

                // Connect To Start Room
                MapRoomConnection connection;
                connection.primaryRoom.id   = graphMap[i].p2.room.id;
                connection.secondaryRoom.id = map.room.size()-1;
                map.room[connection.primaryRoom.id].connection.push_back(connection);
                swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                map.room[connection.primaryRoom.id].connection.push_back(connection);

                // Connect to last hallway
                for(uint32 x = 1; x < j; x++)
                {
                    connection.primaryRoom.id   = map.room.size()-1;
                    connection.secondaryRoom.id = map.room.size()-x-1;
                    map.room[connection.primaryRoom.id].connection.push_back(connection);
                    swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                    map.room[connection.primaryRoom.id].connection.push_back(connection);
                }

                // Connect To Target Room
                connection.primaryRoom.id   = graphMap[i].p1.room.id;
                connection.secondaryRoom.id = map.room.size()-1;
                map.room[connection.primaryRoom.id].connection.push_back(connection);
                swap(connection.primaryRoom.id,connection.secondaryRoom.id);
                map.room[connection.primaryRoom.id].connection.push_back(connection);

                // Set Last Values for next iteration cropping
                LAST_MIN_X = MIN_X;
                LAST_MAX_X = MAX_X;
                LAST_MIN_Y = MIN_Y;
                LAST_MAX_Y = MAX_Y;
            }
        }

        // Clear hallway paths
        for(uint32 j = 1; j < path.size(); j++)
        {
            // Hallway Empty Path
            int32 MIN_X = min(path[j].x, path[j-1].x) + hallwayWallSize;
            int32 MIN_Y = min(path[j].y, path[j-1].y) + hallwayWallSize;
            int32 MAX_X = max(path[j].x, path[j-1].x) + hallwayWidth - hallwayWallSize*2;
            int32 MAX_Y = max(path[j].y, path[j-1].y) + hallwayWidth - hallwayWallSize*2;

            for(uint32 y = MIN_Y; y <= MAX_Y; y++)
            {
                for(uint32 x = MIN_X; x <= MAX_X; x++)
                {
                    setTileFromGlobalPos(map, sf::Vector2i(x,y), 1);
                }
            }
        }

        // Clear Entrance and Exit
        sf::IntRect roomConnectorBlock;
        switch(graphMap[i].p1.doorDirection)
        {
            case DIR_NORTH:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p1.point.x,graphMap[i].p1.point.y-hallwayWallSize,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_SOUTH:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p1.point.x,graphMap[i].p1.point.y,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_EAST:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p1.point.x,graphMap[i].p1.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
            case DIR_WEST:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p1.point.x-hallwayWallSize,graphMap[i].p1.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
        }
        setTileAreaFromGlobalPos(map, roomConnectorBlock, 1);
        switch(graphMap[i].p2.doorDirection)
        {
            case DIR_NORTH:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p2.point.x,graphMap[i].p2.point.y-hallwayWallSize,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_SOUTH:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p2.point.x,graphMap[i].p2.point.y,
                                                 hallwayWidth-hallwayWallSize*2, hallwayWallSize+1);
            }break;
            case DIR_EAST:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p2.point.x,graphMap[i].p2.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
            case DIR_WEST:
            {
                roomConnectorBlock = sf::IntRect(graphMap[i].p2.point.x-hallwayWallSize,graphMap[i].p2.point.y,
                                                 hallwayWallSize+1,hallwayWidth-hallwayWallSize*2);
            }break;
        }
        setTileAreaFromGlobalPos(map, roomConnectorBlock, 1);
    }
}


MapRoom_Refrence getRandomOuterEdgeRoomFromGroup(GameMap &map, mt19937 &random_engine, uint32 offsetRoom, uint32 roomListLength, real32 precentileLowerLimit, real32 precentileUpperLimit)
{
    sf::Vector2f centerMass;
    for(uint32 i = offsetRoom; i < offsetRoom+roomListLength; i++)
    {
        centerMass = centerMass + getCenterOfRect(map.room[i].bounds);
    }
    centerMass.x /= roomListLength;
    centerMass.y /= roomListLength;

    vector<uint32> sortedRoomID;
    vector<real32> sortedRoomDistance;
    for(uint32 i = offsetRoom; i < offsetRoom+roomListLength; i++)
    {
        real32 distance = getDistance(getCenterOfRect(map.room[i].bounds), centerMass);

        int32 insertPosition = -1;
        for(uint32 j = 0; j < sortedRoomID.size(); j++)
        {
            if(distance < sortedRoomDistance[j])
            {
                insertPosition = j;
                break;
            }
        }

        if(insertPosition >= 0)
        {
            sortedRoomID.insert(sortedRoomID.begin()+insertPosition, i);
            sortedRoomDistance.insert(sortedRoomDistance.begin()+insertPosition, distance);
        }else{
            sortedRoomID.push_back(i);
            sortedRoomDistance.push_back(distance);
        }
    }

    uint32 lowerPrecentileIndex = floor((roomListLength-1) * precentileLowerLimit);
    uint32 upperPrecentileIndex = ceil((roomListLength-1) * precentileUpperLimit);
    uniform_int_distribution<uint32> dist_indexRange(lowerPrecentileIndex, upperPrecentileIndex);

    MapRoom_Refrence ref;
    ref.id = dist_indexRange(random_engine);
    return ref;
}

bool generateDungeonNodeSection(GameMap &map, mt19937 &random_engine, RoomIndexConfigFile &indexFile, MapFlowGraph::Node *parent, MapRoom_Refrence ref)
{
    cout << parent->layer << endl;

    uint32 startRoomOffset = map.room.size();
    bool   generationComplete = true;
    switch(parent->type)
    {
        case MapFlowGraph::NODE_START:
        {
            generationComplete = generateRoomClusterNode(map, random_engine, indexFile, ref, 5, 14, 30, 5);
            cout << "START" << endl;
        }break;
        case MapFlowGraph::NODE_END:
        {
            generationComplete = generateRoomClusterNode(map, random_engine, indexFile, ref, 1, 14, 30, 5);
            cout << "END" << endl;
        }break;
        case MapFlowGraph::NODE_JOINT:
        {
            generationComplete = generateRoomClusterNode(map, random_engine, indexFile, ref, 5, 14, 30, 5);
            cout << "JOINT" << endl;
        }break;
        case MapFlowGraph::NODE_KEY:
        {
            generationComplete = generateRoomClusterNode(map, random_engine, indexFile, ref, 1, 14, 30, 5);
            cout << "KEY" << endl;
        }break;
    }

    uint32 endRoomOffset = map.room.size()-startRoomOffset;

    if(generationComplete == false)
        return false;

    for(uint32 i = 0; i < parent->children.size(); i++)
    {
        cout << "child: " << i << endl;

        bool shouldRepeat = true;
        while(shouldRepeat)
        {
            uint32 iteration=0;
            ref = getRandomOuterEdgeRoomFromGroup(map, random_engine, startRoomOffset, endRoomOffset, 0.7, 0.9);
            while(map.room[ref.id].doorsFree == 0)
            {
                ref = getRandomOuterEdgeRoomFromGroup(map, random_engine, startRoomOffset, endRoomOffset, max(0.7-iteration*0.1, 0.0), min(0.9+iteration*0.01,1.0));
                iteration++;
            }
            cout << "Door Left: " << map.room[ref.id].doorsFree << endl;

            shouldRepeat = !generateDungeonNodeSection(map, random_engine, indexFile, parent->children[i], ref);
        }
    }

    return true;
}

GameMap generateRandomGenericDungeonUsingMapFlow(mt19937 &random_engine, string roomdata_filename)
{
    GameMap map;
    RoomIndexConfigFile indexFile = loadRoomIndexConfigFile(roomdata_filename);
    uniform_real_distribution<real32> dist_real32(0.0f,1.0f);

    map.graphFlow = MapFlowGraph::generateGraph(random_engine);

    map.room.push_back(loadRoomFromChanceSplit(indexFile, 0));
    map.room[0].bounds.left = 5000;
    map.room[0].bounds.top  = 5000;
    MapRoom_Refrence ref;
    ref.id = 0;
    generateRoomClusterNode(map, random_engine, indexFile, ref, 50, 14, 30, 5);
    //generateDungeonNodeSection(map, random_engine, indexFile, &map.graphFlow, ref);


    /*
        Pseudo Code

        GenerateNodeSection:
            Randomly_Place_Challenge_Rooms()    x RandomRoomLimit
            Generate_MST_For_Rooms()
            // MST Notes:
            // Instead of the mst nodes being based off of rooms in the original algorithim
            // base them off of room doors this way I can add exceptions in room metadata
            // to assign which doors can connect to which. This for can allow for 100% connectivity
            // for uniquely designed rooms which might have chasms in there tiledata to divide the room

            Add_Loops_To_MST()
            CreateHallwaysFromGraphMap()        x EdgeRoomGraph

            For Node Children:
                Select random unused room door on section edge

                Create_NodeType_Room(selected)
                Link(selected, section)
                CreateHallway(selected)
                If LockCount > 0
                    CreateLockedDoor(selected);

                For Node Child LockCount-1:
                    Select Previous challenge room
                    Create_Challenge_Room(selected)
                    Link(selected, section)
                    CreateHallway(selected)
                    CreateLockedDoor(selected);

                GenerateNodeSection(selected);
    */
    generateSortKeysForMap(map);

    return map;
}

MapRoom createRoom(sf::Vector2u size, byte tile_type)
{
    MapRoom output;
    output.bounds.width  = size.x;
    output.bounds.height = size.y;
    output.tilemap = new MapTile[size.x*size.y];

    for(uint32 i = 0; i < size.x*size.y; i++)
    {
        output.tilemap[i].tileID = tile_type;
    }

    return output;
}

void generateTileAO(MapRoom &room, Tileset &tileset, uint32 x, uint32 y)
{
    room.tilemap[y*room.bounds.width+x].AO = 0;

    if(tileset.tile[room.getTile(x, y).tileID].isFloor)
    {
        for(uint32 i = 1; i < 9; i++)
        {
            sf::Vector2i normal = kTileNeighbourDirectionNormals[i];
            if(tileset.tile[room.getTile(x+normal.x, y+normal.y).tileID].isSolid)
            {
                room.tilemap[y*room.bounds.width+x].AO = room.tilemap[y*room.bounds.width+x].AO | (1 << (i-1));
            }
        }
        // Corners Only Valid if not connected to tiles of similar direction
        for (uint32 i = 0; i < 4; i++)
        {
            if ((room.tilemap[y*room.bounds.width+x].AO & (uint32)(1 << ((i*2+1)%8))) and ((room.tilemap[y*room.bounds.width+x].AO & (uint32)(1 << ((i*2)%8))) or (room.tilemap[y*room.bounds.width+x].AO & (uint32)(1 << ((i*2+2)%8)))))
            {
                room.tilemap[y*room.bounds.width+x].AO = room.tilemap[y*room.bounds.width+x].AO & ~(1 << ((i*2+1)%8));
            }
        }

    }
}

void generateRoomAO(MapRoom &room, Tileset &tileset)
{
    for(uint32 y = 0; y < room.bounds.height; y++)
    {
        for(uint32 x = 0; x < room.bounds.width; x++)
        {
            generateTileAO(room, tileset, x, y);
        }
    }
}

void generateMapAO(GameMap &map, Tileset &tileset)
{
    for(uint32 i = 0; i < map.room.size(); i++)
    {
        generateRoomAO(map.room[i], tileset);
    }
}

void generateTilesetAOSprites(Tileset& tileset, sf::Vector2u size, real32 shadowLength, sf::Color shadowMaxColour, sf::Color shadowMinColour)
{
    sf::Vector2f origin = sf::Vector2f(0,0);
    sf::Vector2f dist   = sf::Vector2f(shadowLength, shadowLength*0.625);

    tileset.AO_Sprites.clear();
    tileset.AO_Sprites.resize(8);
    // NORTH
    tileset.AO_Sprites[4].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[4].append(sf::Vertex(sf::Vector2f(origin.x,origin.y), shadowMaxColour));
    tileset.AO_Sprites[4].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y), shadowMaxColour));
    tileset.AO_Sprites[4].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y+dist.y), shadowMinColour));
    tileset.AO_Sprites[4].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+dist.y), shadowMinColour));

    // NORTH EAST
    tileset.AO_Sprites[3].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[3].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y), shadowMaxColour));
    tileset.AO_Sprites[3].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y+dist.y), shadowMinColour));
    tileset.AO_Sprites[3].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x,origin.y+dist.y), shadowMinColour));
    tileset.AO_Sprites[3].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x,origin.y), shadowMinColour));

    // EAST
    tileset.AO_Sprites[2].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[2].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x, origin.y), shadowMinColour));
    tileset.AO_Sprites[2].append(sf::Vertex(sf::Vector2f(origin.x+size.x, origin.y),        shadowMaxColour));
    tileset.AO_Sprites[2].append(sf::Vertex(sf::Vector2f(origin.x+size.x, origin.y+size.y), shadowMaxColour));
    tileset.AO_Sprites[2].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x, origin.y+size.y), shadowMinColour));

    // SOUTH EAST
    tileset.AO_Sprites[1].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[1].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x, origin.y+size.y-dist.y), shadowMinColour));
    tileset.AO_Sprites[1].append(sf::Vertex(sf::Vector2f(origin.x+size.x, origin.y+size.y-dist.y),        shadowMinColour));
    tileset.AO_Sprites[1].append(sf::Vertex(sf::Vector2f(origin.x+size.x, origin.y+size.y),               shadowMaxColour));
    tileset.AO_Sprites[1].append(sf::Vertex(sf::Vector2f(origin.x+size.x-dist.x, origin.y+size.y),        shadowMinColour));

    // SOUTH
    tileset.AO_Sprites[0].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[0].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+size.y-dist.y), shadowMinColour));
    tileset.AO_Sprites[0].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y+size.y-dist.y), shadowMinColour));
    tileset.AO_Sprites[0].append(sf::Vertex(sf::Vector2f(origin.x+size.x,origin.y+size.y), shadowMaxColour));
    tileset.AO_Sprites[0].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+size.y),        shadowMaxColour));

    // SOUTH WEST
    tileset.AO_Sprites[7].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[7].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+size.y), shadowMaxColour));
    tileset.AO_Sprites[7].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+size.y-dist.y), shadowMinColour));
    tileset.AO_Sprites[7].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y+size.y-dist.y), shadowMinColour));
    tileset.AO_Sprites[7].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y+size.y), shadowMinColour));

    // WEST
    tileset.AO_Sprites[6].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[6].append(sf::Vertex(sf::Vector2f(origin.x,origin.y), shadowMaxColour));
    tileset.AO_Sprites[6].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y), shadowMinColour));
    tileset.AO_Sprites[6].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y+size.y), shadowMinColour));
    tileset.AO_Sprites[6].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+size.y), shadowMaxColour));

    // NORTH WEST
    tileset.AO_Sprites[5].setPrimitiveType(sf::PrimitiveType::Quads);
    tileset.AO_Sprites[5].append(sf::Vertex(sf::Vector2f(origin.x,origin.y), shadowMaxColour));
    tileset.AO_Sprites[5].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y), shadowMinColour));
    tileset.AO_Sprites[5].append(sf::Vertex(sf::Vector2f(origin.x+dist.x,origin.y+dist.y), shadowMinColour));
    tileset.AO_Sprites[5].append(sf::Vertex(sf::Vector2f(origin.x,origin.y+dist.y), shadowMinColour));
}

int32 setTileFromGlobalPos(GameMap &map, sf::Vector2i position, byte tile_id)
{
    for(uint32 i = 0; i < map.room.size(); i++)
    {
        if(map.room[i].bounds.contains(position.x, position.y))
        {
            int32 _x = position.x-map.room[i].bounds.left;
            int32 _y = position.y-map.room[i].bounds.top;

            map.room[i].tilemap[map.room[i].bounds.width * _y + _x].tileID = tile_id;
            return i;
        }
    }

    return -1;
}

bool setTileAreaFromGlobalPos(GameMap &map, sf::IntRect area, byte tile_id)
{
    bool ranIntoAnyValidTiles = false;
    for(uint32 y = 0; y < area.height; y++)
    {
        for(uint32 x = 0; x < area.width; x++)
        {
            int32 _x = x + area.left;
            int32 _y = y + area.top;

            int32 room_id = setTileFromGlobalPos(map, sf::Vector2i(_x, _y), tile_id);
            ranIntoAnyValidTiles = ranIntoAnyValidTiles || room_id != -1;
        }
    }

    return ranIntoAnyValidTiles;
}

int32   roomAtGlobalPos(GameMap &map, sf::Vector2i position)
{
    for(uint32 i = 0; i < map.room.size(); i++)
    {
        if(map.room[i].bounds.contains(position.x, position.y))
        {
            return i;
        }
    }

    return -1;
}

bool doesRectContainRoom(GameMap &map, sf::IntRect rect)
{
    for(uint32 i = 0; i < map.room.size(); i++)
    {
        if(map.room[i].bounds.intersects(rect))
        {
            return true;
        }
    }
    return false;
}

void setRectTileArea(MapRoom &room, byte tile_id, sf::IntRect area)
{
    // Crop area to fit
    area.left   = min(max(area.left, 0),    room.bounds.width);
    area.top    = min(max(area.top, 0),     room.bounds.height);
    area.width  = min(max(area.width, 0),   room.bounds.width);
    area.height = min(max(area.height, 0),  room.bounds.height);

    // NOTE(Connor): If this ever becomes a Preformance crucial function convert y && x for loops to an index based one
    for(uint32 y = 0; y < area.height; y++)
    {
        for(uint32 x = 0; x < area.width; x++)
        {
            uint32 _x = x + area.left;
            uint32 _y = y + area.top;
            room.tilemap[_y * room.bounds.width + _x].tileID = tile_id;
        }
    }
}

bool doesRectContainSolidTiles(GameMap &map, Tileset &tileset, uint32 room_id, sf::FloatRect rect, sf::Vector2f offset)
{
    for(real32 x = 0; x <= ceil(rect.width); x++)
    {
        for(real32 y = 0; y <= ceil(rect.height); y++)
        {
            uint32 tile_room = room_id;
            sf::Vector2f pos;
            pos.x = min(rect.width,  x) + offset.x + rect.left;
            pos.y = min(rect.height, y) + offset.y + rect.top;

            // If not in main room figure out which room point is in
            if(pos.x < 0 || pos.y < 0 || pos.x >= map.room[room_id].bounds.width || pos.y >= map.room[room_id].bounds.height)
            {
                real32 globalposX = pos.x + map.room[room_id].bounds.left;
                real32 globalposY = pos.y + map.room[room_id].bounds.top;
                for(uint32 i = 0; i < map.room[room_id].connection.size(); i++)
                {
                    if(map.room[map.room[room_id].connection[i].secondaryRoom.id].bounds.contains(globalposX, globalposY))
                    {
                        tile_room = map.room[room_id].connection[i].secondaryRoom.id;
                        MapRoom_Refrence old_room;
                        old_room.id = room_id;

                        pos = relativeRoomPosition(map, pos, old_room, map.room[room_id].connection[i].secondaryRoom);
                        break;
                    }
                }
            }

            uint32 tileID = map.room[tile_room].getTile(pos.x, pos.y).tileID;

            if(tileID < tileset.tileCount && tileset.tile[tileID].isSolid)
            {
                return true;
            }
        }
    }

    return false;
}

sf::Vector2f relativeRoomPosition(GameMap &map, sf::Vector2f position, MapRoom_Refrence old_room, MapRoom_Refrence new_room)
{
    sf::Vector2f roomDiffrence;
    roomDiffrence.x = map.room[old_room.id].bounds.left - map.room[new_room.id].bounds.left;
    roomDiffrence.y = map.room[old_room.id].bounds.top  - map.room[new_room.id].bounds.top;
    return position + roomDiffrence;
}

void attachRoom(GameMap &map, uint32 primaryRoom, uint32 secondaryRoom, sf::Vector2u primaryDoor, sf::Vector2u secondaryDoor, MapDirection direction)
{
    assert(primaryRoom < map.room.size() && secondaryRoom < map.room.size());

    MapRoomConnection connection;
    connection.primaryRoom.id   = primaryRoom;
    connection.secondaryRoom.id = secondaryRoom;
    connection.direction        = direction;

    map.room[primaryRoom].connection.push_back(connection);

    connection.secondaryRoom.id   = primaryRoom;
    connection.primaryRoom.id = secondaryRoom;

    map.room[secondaryRoom].connection.push_back(connection);

    map.room[secondaryRoom].bounds.left = map.room[primaryRoom].bounds.left - secondaryDoor.x + primaryDoor.x + kMapDirectionNormals[(uint32)direction].x;
    map.room[secondaryRoom].bounds.top  = map.room[primaryRoom].bounds.top  - secondaryDoor.y + primaryDoor.y + kMapDirectionNormals[(uint32)direction].y;
}

void generateSortKeysForMap(GameMap &map)
{
    map.sort_list.clear();
    map.sort_list.resize(map.room.size());
    for(uint32 i = 0; i < map.room.size(); i++)
    {
        map.sort_list[i].key     = map.room[i].bounds.top+map.room[i].bounds.height;
        map.sort_list[i].room_id = i;
    }

    insertion_sortRoomOrder(map);
}

void insertion_sortRoomOrder(GameMap &map)
{
    uint32 j;
    for (uint32 i = 0; i < map.sort_list.size(); i++){
        j = i;

        while (j > 0 && map.sort_list[j].key < map.sort_list[j-1].key)
        {
            iter_swap(map.sort_list.begin()+j, map.sort_list.begin()+j-1);
            j--;
        }
    }
}

void debugRenderRoom(sf::RenderTarget *target, GameMap &map, uint32 roomID)
{
    sf::RectangleShape roomBoundary;
    roomBoundary.setSize(sf::Vector2f(map.room[roomID].bounds.width * map.tileSize.x, map.room[roomID].bounds.height * map.tileSize.y));
    roomBoundary.setPosition(sf::Vector2f(map.room[roomID].bounds.left * map.tileSize.x, map.room[roomID].bounds.top * map.tileSize.y));
    roomBoundary.setOutlineThickness(4);
    roomBoundary.setOutlineColor(sf::Color::Green);
    roomBoundary.setFillColor(sf::Color::Transparent);
    target->draw(roomBoundary);
}

sf::IntRect getRoomGroupBounds(GameMap &map, uint32 room_group)
{
    int32 min_x=numeric_limits<int32>::max(), min_y=numeric_limits<int32>::max(), max_x=numeric_limits<int32>::min(), max_y=numeric_limits<int32>::min();

    for(uint32 i = 0; i < map.room.size(); i++)
    {
        if(map.room[i].room_group == room_group)
        {
            min_x = min((int32)map.room[i].bounds.left, min_x);
            min_y = min((int32)map.room[i].bounds.top,  min_y);
            max_x = max((int32)map.room[i].bounds.left + map.room[i].bounds.width, max_x);
            max_y = max((int32)map.room[i].bounds.top + map.room[i].bounds.height, max_y);
        }
    }

    return sf::IntRect(min_x, min_y, max_x-min_x, max_y-min_y);
}

#endif /* end of include guard: GAME_MAP_CPP */
