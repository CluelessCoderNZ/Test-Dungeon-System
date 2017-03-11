#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <SFML/Graphics.hpp>
#include <random>
#include <cmath>
#include "game_consts.h"

sf::FloatRect scaleRect(sf::IntRect rect, sf::Vector2f scalar);

real32 getAngle(sf::Vector2f a, sf::Vector2f b);
real32 getAngle(sf::Vector2i a, sf::Vector2i b);

real32 getDistance(sf::Vector2i a, sf::Vector2i b);
real32 getDistance(sf::Vector2f a, sf::Vector2f b);

real32 getDotProduct(sf::Vector2i a, sf::Vector2i b);
real32 getDotProduct(sf::Vector2f a, sf::Vector2f b);

uint32 getManhattanDistance(sf::Vector2i a, sf::Vector2i b);


real32 getRandomFloat();
int32  getRandomIntger(int32 MIN, int32 MAX);

#endif /* end of include guard: MATH_UTILS_H */
