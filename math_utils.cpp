#ifndef MATH_UTILS_CPP
#define MATH_UTILS_CPP
#include "math_utils.h"

sf::FloatRect scaleRect(sf::IntRect rect, sf::Vector2f scalar)
{
    return sf::FloatRect(rect.left * scalar.x, rect.top * scalar.y, rect.width * scalar.x, rect.height  * scalar.y);
}

sf::Vector2f getCenterOfRect(sf::IntRect rect)
{
    return sf::Vector2f(rect.left+rect.width/2, rect.top+rect.height/2);
}

sf::Vector2f  getPerimeterPoint(sf::IntRect rect, real32 cursor)
{
    cursor = (rect.width * 2 + rect.height * 2)*cursor;

    if(cursor < rect.width)
    {
        return sf::Vector2f(cursor, 0);
    }else if(cursor < rect.width + rect.height)
    {
        return sf::Vector2f(rect.width, cursor - rect.width);
    }else if(cursor < rect.width * 2 + rect.height)
    {
        return sf::Vector2f(rect.width - (cursor - rect.width - rect.height), rect.height);
    }else
    {
        return sf::Vector2f(0, rect.height - (cursor - rect.width * 2 - rect.height));
    }
}

sf::Vector2f getPerimeterPointByAngle(sf::IntRect rect, real32 theta)
{
    while (theta < -kPI)
    {
        theta += kTAU;
    }

    while (theta > kPI)
    {
        theta -= kTAU;
    }

    real32 aa = rect.width;
    real32 bb = rect.height;

    real32 rectAtan = atan2f(bb, aa);
    real32 tanTheta = tan(theta);

    uint32 region;
    if ((theta > -rectAtan) && (theta <= rectAtan) )
    {
        region = 1;
    }else if ((theta > rectAtan) && (theta <= (kPI - rectAtan)))
    {
        region = 2;
    }else if ((theta > (kPI - rectAtan)) || (theta <= -(kPI - rectAtan)))
    {
        region = 3;
    }else
    {
        region = 4;
    }

    sf::Vector2f edgePoint(rect.width/2, rect.height/2);
    real32 xFactor = 1;
    real32 yFactor = 1;

    switch (region)
    {
        case 1: yFactor = -1;       break;
        case 2: yFactor = -1;       break;
        case 3: xFactor = -1;       break;
        case 4: xFactor = -1;       break;
    }

    if ((region == 1) || (region == 3))
    {
        edgePoint.x += xFactor * (aa / 2.);
        edgePoint.y += yFactor * (aa / 2.) * tanTheta;
    }else
    {
        edgePoint.x += xFactor * (bb / (2. * tanTheta));
        edgePoint.y += yFactor * (bb /  2.);
    }

    return edgePoint;
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
