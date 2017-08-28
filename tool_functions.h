#ifndef TOOL_FUNCTIONS_H
#define TOOL_FUNCTIONS_H
#include <fstream>
#include <sstream>
#include <iostream>
#include "json/json.h"
#include "jsoncpp.cpp"
#include "game_consts.h"
#include "math_utils.h"
#include "math_utils.cpp"

using namespace std;

string fastReadFile(string filename);
Json::Value readJsonFile(string filename);
uint32 str2uint(string a);
real32 interpolate(real32 a, real32 b, real32 t);
sf::Color interpolate(sf::Color a, sf::Color b, real32 t);
real32 precentDiff(real32 min, real32 max, real32 x);

string whitespace(uint32 size);

void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, sf::Color colour);
void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, real32 thickness, sf::Color colour);

#endif /* end of include guard: TOOL_FUNCTIONS_H */
