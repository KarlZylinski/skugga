#pragma once

#include "key.h"

namespace keyboard
{

void init();
bool is_held(Key key);
bool is_presssed(Key key);
bool is_released(Key key);
void pressed(Key key);
void released(Key key);
void end_of_frame();

}
