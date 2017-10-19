#ifndef MAP_FLOW_GRAPH_H
#define MAP_FLOW_GRAPH_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "tool_functions.h"

using namespace std;

namespace MapFlowGraph
{
    const uint32    kLockTypeCount = 2;
    const sf::Color kLockTypeColours[] =
    {
        sf::Color(190, 20, 20),     // small key
        sf::Color(20, 140, 150),    // Boss key
        sf::Color(20, 170, 30)      // Custom Key
    };

    enum Node_Type
    {
        NODE_JOINT,    // Defines branching paths (should have children)
        NODE_START,    // Defines start of path
        NODE_END,      // Defines target of path
        NODE_KEY       // Defines a node key end (Should have no children)
    };

    struct Lock
    {
        uint32 keysNeeded; // For switch based locks these might need multiple swtiches flicked to open the door
        uint32 type; // Key type must match to unlock
    };

    struct Key
    {
        bool   isInfinte=false; // For item based "locks" they are considered infinite
        uint32 type = 0;
    };

    // HACK(Connor): Children deletion not handled well, will not free memory on program exit
    struct Node
    {
        vector<MapFlowGraph::Node*>  children;
        vector<MapFlowGraph::Lock>  locks;       // These are locks in front of the node

        Node_Type                  type = NODE_JOINT;
        Key                        key;         // Key data if is key node
        uint32                     layer = 0;
        uint32                     height = 0;
        uint32                     width = 1;

        Node(uint32 _layer)
        {
            layer = _layer;
            type  = NODE_JOINT;
        }

        Node(Node_Type _type=NODE_JOINT, uint32 _keyType=0)
        {
            type        = _type;
            key.type    = _keyType;
            children.clear();
        }

        void sortChildrenByHeight()
        {
            sort( children.begin( ), children.end( ), [ ]( const MapFlowGraph::Node* lhs, const MapFlowGraph::Node* rhs )
            {
               return lhs->height < rhs->height;
            });

            for(uint32 i = 0; i < children.size(); i++)
            {
                children[i]->sortChildrenByHeight();
            }
        }

        uint32 getDepthOfChildNode()
        {
            uint32 output = layer;
            for(uint32 i = 0; i < children.size(); i++)
            {
                uint32 depth = children[i]->getDepthOfChildNode();
                if(depth > output)
                {
                    output = depth;
                }
            }
            output += locks.size();

            height = output;
            return output;
        }

        uint32 getWidthOfChildNode()
        {
            uint32 output = 1 + children.size();
            for(uint32 i = 0; i < children.size(); i++)
            {
                uint32 childWidth = children[i]->getWidthOfChildNode();
                if(childWidth > output)
                {
                    output = childWidth;
                }
            }

            width = output;
            return output;
        }
    };

    MapFlowGraph::Node generateGraph(mt19937 &random_engine);
    void renderGraphNode(sf::RenderWindow& window, Node *node, sf::Vector2f offset=sf::Vector2f(0,0));
}

#endif /* end of include guard: MAP_FLOW_GRAPH_H */
