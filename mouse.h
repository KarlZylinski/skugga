#pragma once

#include "math.h"

namespace mouse
{

void init();
void add_delta(const Vector2i& delta);
const Vector2i& delta();
void end_of_frame();

}