namespace window
{

typedef void(*KeyPressedCallback)(Key key);
typedef void(*KeyReleasedCallback)(Key key);
typedef void(*MouseMovedCalledback)(const Vector2i& delta);

} // namespace window

struct Window
{
    bool closed;
    window::KeyPressedCallback key_pressed_callback;
    window::KeyReleasedCallback key_released_callback;
    window::MouseMovedCalledback mouse_moved_callback;
};
