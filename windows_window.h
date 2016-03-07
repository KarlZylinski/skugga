#pragma once
#include <windows.h>
#include "window_state.h"

namespace windows
{

struct Window
{
    HWND handle;
    WNDCLASSEX window_class;
    WindowState state;
};

namespace window
{

void init(Window* w);
void process_all_messsages();

}
}
