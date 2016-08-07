#pragma once

struct Color
{
    float r, g, b, a;
};

struct ColorUNorm
{
    unsigned char r, g, b, a;
};

namespace color
{

Color random();
bool almost_equal(const Color& c1, const Color& c2);

}

__forceinline void operator+=(Color& c1, const Color& c2)
{
    c1.r += c2.r;
    c1.g += c2.g;
    c1.b += c2.b;
    c1.a += c2.a;
}
