#pragma once

#include "key.h"

namespace keyboard
{

void init();
bool held(Key key);
bool presssed(Key key);
bool released(Key key);

}
