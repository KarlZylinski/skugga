struct Window
{
    HWND handle;
    WNDCLASSEX window_class;
    bool closed;
};

namespace window
{
LRESULT window_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    Window* window = (Window*)GetWindowLongPtr(window_handle, GWLP_USERDATA);

    if (window == nullptr)
        return DefWindowProc(window_handle, message, wparam, lparam);

    switch(message)
    {
    case WM_QUIT:
    case WM_CLOSE:
        window->closed = true;
        return 0;
    }

    return DefWindowProc(window_handle, message, wparam, lparam);
}

void init(Window* w)
{
    HINSTANCE instance_handle = GetModuleHandle(nullptr);
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance_handle;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"Skugga";
    w->window_class = wc;
    RegisterClassEx(&wc);
    RECT window_rect = {0, 0, 800, 800};
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, false);
    w->handle = CreateWindowEx(
        0,
        L"Skugga",
        L"Skugga",
        WS_OVERLAPPEDWINDOW,
        300,
        300,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        nullptr,
        nullptr,
        instance_handle,
        nullptr);
    ShowWindow(w->handle, true);
    SetWindowLongPtr(w->handle, GWLP_USERDATA, (LONG_PTR)w);
}

void process_all_messsages()
{
    MSG message;
    while(PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}
} // namespace window
