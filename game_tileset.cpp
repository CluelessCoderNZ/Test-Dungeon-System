#ifndef GAME_TILESET_CPP
#define GAME_TILESET_CPP
#include "game_tileset.h"

Tileset loadTilesetFromFile(string filename, sf::Vector2u tileSize)
{
    Tileset   output;
    sf::Image tilesetImage;
    tilesetImage.loadFromFile(filename);
    output.tileCount = (tilesetImage.getSize().x / tileSize.x) * (tilesetImage.getSize().y / tileSize.y) + 1;

    output.tile.resize(output.tileCount);
    // Unlisted Blank Tile
    output.tile[0].name         = "Air";
    output.tile[0].isSolid      = false;
    output.tile[0].isVisible    = false;

    uint32 tilesPerRow = tilesetImage.getSize().x / tileSize.x;
    for(uint32 i = 1; i < output.tileCount; i++)
    {
        output.tile[i].texture.loadFromImage(tilesetImage, sf::IntRect(((i-1) % tilesPerRow)*tileSize.x, ((i-1) / tilesPerRow)*tileSize.y, tileSize.x, tileSize.y));
        output.tile[i].sprite.setTexture(output.tile[i].texture);
        output.tile[i].sprite.setOrigin(0, 12);
    }

    return output;
}

#endif /* end of include guard: GAME_TILESET_CPP */
