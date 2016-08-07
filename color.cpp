#include "color.h"
#include "helpers.h"
#include <stdlib.h>

namespace color
{

Color random()
{
    return Color { rand()%255/255.0f, rand()%255/255.0f, rand()%255/255.0f, 1.0f };
}

bool almost_equal(const Color& c1, const Color& c2)
{
    return ::almost_equal(c1.r, c2.r) && ::almost_equal(c1.g, c2.g) && ::almost_equal(c1.b, c2.b) && ::almost_equal(c1.a, c2.a);
}

} // namesapce color


void operator+=(Color& c1, const Color& c2)
{
    c1.r += c2.r;
    c1.g += c2.g;
    c1.b += c2.b;
    c1.a += c2.a;
}
