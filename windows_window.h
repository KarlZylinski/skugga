#pragma once
#include <windows.h>
#include "window_state.h"

struct WindowsWindow
{
    HWND handle;
    WNDCLASSEX window_class;
    WindowState state;
};

void create_window(WindowsWindow* w);
void process_all_window_messsages();
