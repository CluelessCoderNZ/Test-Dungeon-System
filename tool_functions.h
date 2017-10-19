#ifndef TOOL_FUNCTIONS_H
#define TOOL_FUNCTIONS_H
#include <fstream>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <bitset>
#include <iomanip>
#include "json/json.h"
#include "game_consts.h"
#include "math_utils.h"

using namespace std;

string numToStr(real32 value, int32 sf = -1);
string numToStr(int32 value);
string binaryToStr(byte value);
bool   str2char(string &str, char* dest, uint32 buffer_size);

string variableToStr(sf::Vector2u  value);
string variableToStr(sf::Vector2f  value);
string variableToStr(sf::Vector2i  value);
string variableToStr(sf::IntRect   value);
string variableToStr(sf::FloatRect value);
string variableToStr(sf::Color     value);
string variableToStr(uint32        value);
string variableToStr(real32        value);
string variableToStr(int32         value);
string variableToStr(bool          value);

string fastReadFile(string filename);
Json::Value readJsonFile(string filename);

bool createLinearJsonFromMemory(Json::Value &root, byte* source, string format);
bool saveLinearJsonFileFromMemory(string filename, byte* source, string format);
bool loadLinearJsonIntoMemory(Json::Value &root, byte* dest, string format, bool haltIfMemberDoesNotExist=true);

uint32 str2uint(string a);
real32 interpolate(real32 a, real32 b, real32 t);
sf::Color interpolate(sf::Color a, sf::Color b, real32 t);
real32 precentDiff(real32 min, real32 max, real32 x);

string whitespace(uint32 size);

void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, sf::Color colour);
void drawLine(sf::RenderTarget &target, sf::Vector2f p1, sf::Vector2f p2, real32 thickness, sf::Color colour);

#endif /* end of include guard: TOOL_FUNCTIONS_H */
