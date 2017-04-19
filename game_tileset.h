#ifndef GAME_TILESET_H
#define GAME_TILESET_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "game_consts.h"

using namespace std;

struct Tile_Template
{
    string      name        = "NULL";
    b32         isSolid     = false;
    b32         isFloor     = true;
    b32         isVisible   = true;
    sf::Texture texture;
    sf::Sprite  sprite;
};

struct Tileset
{
    vector<Tile_Template>   tile;
    vector<sf::VertexArray> AO_Sprites;
    sf::RenderStates        AO_RenderState;
    uint32                  tileCount = 0;
    sf::Vector2u            tileSize  = sf::Vector2u(32,32);
};

Tileset loadTilesetFromFile(string filename, sf::Vector2u tileSize);

#endif /* end of include guard: GAME_TILESET_H */
