#ifndef MATH_UTILS_CPP
#define MATH_UTILS_CPP
#include "math_utils.h"

sf::FloatRect scaleRect(sf::IntRect rect, sf::Vector2f scalar)
{
    return sf::FloatRect(rect.left * scalar.x, rect.top * scalar.y, rect.width * scalar.x, rect.height  * scalar.y);
}

real32 getAngle(sf::Vector2f a, sf::Vector2f b)
{
    return atan2(a.y-b.y, a.x-b.x);
}

real32 getAngle(sf::Vector2i a, sf::Vector2i b)
{
    return atan2(a.y-b.y, a.x-b.x);
}

real32 getDistance(sf::Vector2i a, sf::Vector2i b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

real32 getDistance(sf::Vector2f a, sf::Vector2f b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}

real32 getDotProduct(sf::Vector2i a, sf::Vector2i b)
{
    return a.x * b.x + a.y + b.y;
}

real32 getDotProduct(sf::Vector2f a, sf::Vector2f b)
{
    return a.x * b.x + a.y + b.y;
}

uint32 getManhattanDistance(sf::Vector2i a, sf::Vector2i b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}


mt19937 rand_engine(time(NULL));

real32 getRandomFloat()
{
    uniform_real_distribution<real32> dist(0.0f,1.0f);

    return dist(rand_engine);
}

int32 getRandomIntger(int32 MIN, int32 MAX)
{
    uniform_int_distribution<int32> dist(MIN,MAX);

    return dist(rand_engine);
}

#endif /* end of include guard: MATH_UTILS_CPP */
