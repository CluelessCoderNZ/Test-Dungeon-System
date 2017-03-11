#ifndef MAP_FLOW_GRAPH_CPP
#define MAP_FLOW_GRAPH_CPP
#include "map_flow_graph.h"

MapFlowGraph::Node MapFlowGraph::generateGraph(mt19937 &random_engine)
{
    uniform_real_distribution<real32> dist_real32(0.0f,1.0f);

    // Initalize default map graph
    MapFlowGraph::Node rootNode;
    rootNode.type    = NODE_START;

    rootNode.children.push_back(new MapFlowGraph::Node(1));
    rootNode.children[0]->type  = NODE_END;

    std::vector<MapFlowGraph::Node*> createdNodes;
    createdNodes.push_back(&rootNode);

    uint32 smallLockCount = 3;
    uint32 nodeCount = 4;
    uint32 nodesCreated = 0;
    int32  deepestLayer = 0;
    uint32 deepestNode = 0;

    while(nodesCreated < nodeCount)
    {
        MapFlowGraph::Node* targetNode = createdNodes.at(round(dist_real32(random_engine)*(createdNodes.size()-1)));

        targetNode->children.push_back(new MapFlowGraph::Node(targetNode->layer + 1));
        createdNodes.push_back(targetNode->children.back());

        if(createdNodes.back()->layer > deepestLayer)
        {
            deepestLayer = createdNodes.back()->layer;
            deepestNode  = createdNodes.size()-1;
        }

        nodesCreated++;
    }

    uint32 customKeyTypeCount = 2;


    int32 bossKeyParentNode = 0;
    if(dist_real32(random_engine) > 0.5)
    {
        bossKeyParentNode = deepestNode;
        MapFlowGraph::Node* bossKeyNode = new MapFlowGraph::Node(NODE_KEY, 1);
        bossKeyNode->layer = createdNodes[bossKeyParentNode]->layer + 1;

        createdNodes[bossKeyParentNode]->children.push_back(bossKeyNode);
    }else{
        bossKeyParentNode = 0;
        MapFlowGraph::Node* bossKeyNode = new MapFlowGraph::Node(NODE_KEY, 1);
        bossKeyNode->layer = createdNodes[bossKeyParentNode]->layer + 1;
        bossKeyNode->locks.push_back({1, customKeyTypeCount});

        createdNodes[bossKeyParentNode]->children.push_back(bossKeyNode);

        uint32 secondaryParentNode = deepestNode;
        MapFlowGraph::Node* secondaryKeyNode = new MapFlowGraph::Node(NODE_KEY, customKeyTypeCount++);
        secondaryKeyNode->layer = createdNodes[secondaryParentNode]->layer + 1;

        createdNodes[secondaryParentNode]->children.push_back(secondaryKeyNode);
    }

    // TODO(Connor): Think of a more compilcated distribution for keys that goals are covered by locks
    for(uint32 i = 0; i < smallLockCount; i++)
    {
        MapFlowGraph::Node* targetLockNode = createdNodes.at(1+round(dist_real32(random_engine)*(createdNodes.size()-2)));

        targetLockNode->locks.push_back({1,0});

        while(true)
        {
            MapFlowGraph::Node* targetKeyNode = createdNodes.at(round(dist_real32(random_engine)*(createdNodes.size()-1)));

            if(targetLockNode->layer >= targetKeyNode->layer+1)
            {
                targetKeyNode->children.push_back(new MapFlowGraph::Node(targetKeyNode->layer + 1));
                targetKeyNode->children.back()->type     = NODE_KEY;
                break;
            }
        }
    }

    createdNodes.push_back(rootNode.children[0]);
    rootNode.children[0]->locks.push_back({1, 1});

    rootNode.getDepthOfChildNode();
    rootNode.getWidthOfChildNode();
    rootNode.sortChildrenByHeight();
    return rootNode;
}

void MapFlowGraph::renderGraphNode(sf::RenderWindow& window, MapFlowGraph::Node *node, sf::Vector2f position)
{
    const uint32 nodeSize = 32;
    const uint32 outlineThickness = 2;
    const uint32 nodeGap = 16;

    // Render Locks
    for(uint32 i = 0 ; i < node->locks.size(); i++)
    {
        drawLine(window, position, position+sf::Vector2f(0,nodeSize*1.5), outlineThickness, sf::Color::White);

        sf::RectangleShape lockShape;
        lockShape.setOutlineThickness(outlineThickness);
        lockShape.setSize(sf::Vector2f((nodeSize)-outlineThickness*2, (nodeSize)-outlineThickness*2));
        lockShape.setOrigin(nodeSize/2, nodeSize/2);
        lockShape.setFillColor(MapFlowGraph::kLockTypeColours[min(node->locks[i].type, MapFlowGraph::kLockTypeCount)]);
        lockShape.setPosition(position);
        window.draw(lockShape);

        position = position+sf::Vector2f(0,nodeSize*1.5);
    }


    // Render First branching connector
    if(node->children.size() > 0)
    {
        drawLine(window, position, position+sf::Vector2f(0,nodeSize*1.5), outlineThickness, sf::Color::White);
    }

    // Render Node
    switch (node->type)
    {
        case MapFlowGraph::NODE_START:
        {
            sf::CircleShape nodeShape;
            nodeShape.setOutlineThickness(outlineThickness);
            nodeShape.setRadius(nodeSize/2.0-outlineThickness);
            nodeShape.setOrigin(nodeSize/2, nodeSize/2);
            nodeShape.setFillColor(sf::Color(75, 10, 75));
            nodeShape.setPosition(position);
            window.draw(nodeShape);
        }break;
        case MapFlowGraph::NODE_END:
        {
            sf::CircleShape nodeShape;
            nodeShape.setOutlineThickness(outlineThickness);
            nodeShape.setRadius(nodeSize/2.0-outlineThickness);
            nodeShape.setOrigin(nodeSize/2, nodeSize/2);
            nodeShape.setFillColor(sf::Color(75, 10, 75));
            nodeShape.setPosition(position);
            window.draw(nodeShape);
        }break;
        case MapFlowGraph::NODE_JOINT:
        {
            sf::CircleShape nodeShape;
            nodeShape.setOutlineThickness(outlineThickness);
            nodeShape.setRadius(nodeSize/2.0-outlineThickness);
            nodeShape.setOrigin(nodeSize/2, nodeSize/2);
            nodeShape.setFillColor(sf::Color::Black);
            nodeShape.setPosition(position);
            window.draw(nodeShape);
        }break;
        case MapFlowGraph::NODE_KEY:
        {
            sf::RectangleShape nodeShape;
            nodeShape.setOutlineThickness(outlineThickness);
            nodeShape.setSize(sf::Vector2f((nodeSize*0.75)-outlineThickness*2, (nodeSize*0.75)-outlineThickness*2));
            nodeShape.setOrigin(nodeSize/2, nodeSize/2);
            nodeShape.setRotation(45);
            nodeShape.setFillColor(MapFlowGraph::kLockTypeColours[min(node->key.type, MapFlowGraph::kLockTypeCount)]);
            nodeShape.setPosition(position);
            window.draw(nodeShape);
        }break;
    }

    uint32 indentPlacement = 0;
    for(uint32 i = 0; i < node->children.size(); i++)
    {
        if(i > 0)
        {
            drawLine(window, position+sf::Vector2f(indentPlacement,nodeSize), position+sf::Vector2f(indentPlacement,nodeSize*2), outlineThickness, sf::Color::White);
        }
        MapFlowGraph::renderGraphNode(window, node->children[i], position+sf::Vector2f(indentPlacement, nodeSize*2));
        if(i + 1 < node->children.size())
            indentPlacement+=nodeGap+nodeSize*node->children[i]->width;
    }

    if(node->children.size() > 1)
        drawLine(window, position+sf::Vector2f(0,nodeSize), position+sf::Vector2f(indentPlacement,nodeSize), outlineThickness, sf::Color::White);

}

#endif /* end of include guard: MAP_FLOW_GRAPH_CPP */
