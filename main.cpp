#include <windows.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <cmath>

struct Color {
    float r, g, b, a;
};

struct Vector3 {
    float x, y, z;
};

struct Vector4 {
    float x, y, z, w;
};

struct Vertex {
    Vector3 position;
    Color color;
};

struct Matrix4x4 {
    Vector4 x, y, z, w;
};

float dot(const Vector4& v1, const Vector4& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

Matrix4x4 mul(const Matrix4x4& m1, const Matrix4x4& m2)
{
    return Matrix4x4 {
        m1.x.x * m2.x.x + m1.x.y * m2.y.x + m1.x.z * m2.z.x + m1.x.w * m2.w.x,
        m1.x.x * m2.x.y + m1.x.y * m2.y.y + m1.x.z * m2.z.y + m1.x.w * m2.w.y,
        m1.x.x * m2.x.z + m1.x.y * m2.y.z + m1.x.z * m2.z.z + m1.x.w * m2.w.z,
        m1.x.x * m2.x.w + m1.x.y * m2.y.w + m1.x.z * m2.z.w + m1.x.w * m2.w.w,

        m1.y.x * m2.x.x + m1.y.y * m2.y.x + m1.y.z * m2.z.x + m1.y.w * m2.w.x,
        m1.y.x * m2.x.y + m1.y.y * m2.y.y + m1.y.z * m2.z.y + m1.y.w * m2.w.y,
        m1.y.x * m2.x.z + m1.y.y * m2.y.z + m1.y.z * m2.z.z + m1.y.w * m2.w.z,
        m1.y.x * m2.x.w + m1.y.y * m2.y.w + m1.y.z * m2.z.w + m1.y.w * m2.w.w,

        m1.z.x * m2.x.x + m1.z.y * m2.y.x + m1.z.z * m2.z.x + m1.z.w * m2.w.x,
        m1.z.x * m2.x.y + m1.z.y * m2.y.y + m1.z.z * m2.z.y + m1.z.w * m2.w.y,
        m1.z.x * m2.x.z + m1.z.y * m2.y.z + m1.z.z * m2.z.z + m1.z.w * m2.w.z,
        m1.z.x * m2.x.w + m1.z.y * m2.y.w + m1.z.z * m2.z.w + m1.z.w * m2.w.w,

        m1.w.x * m2.x.x + m1.w.y * m2.y.x + m1.w.z * m2.z.x + m1.w.w * m2.w.x,
        m1.w.x * m2.x.y + m1.w.y * m2.y.y + m1.w.z * m2.z.y + m1.w.w * m2.w.y,
        m1.w.x * m2.x.z + m1.w.y * m2.y.z + m1.w.z * m2.z.z + m1.w.w * m2.w.z,
        m1.w.x * m2.x.w + m1.w.y * m2.y.w + m1.w.z * m2.z.w + m1.w.w * m2.w.w
    };
}

struct ConstantBuffer {
    Matrix4x4 model_view_projection;
};

bool run = true;
LRESULT window_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

int main()
{
    HINSTANCE instance_handle = GetModuleHandle(nullptr);
    HWND handle = {0};
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance_handle;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"Skugga";
    RegisterClassEx(&wc);
    RECT window_rect = {0, 0, 800, 800};
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, false);
    handle = CreateWindowEx(
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
    ShowWindow(handle, true);
    DXGI_SWAP_CHAIN_DESC scd = {0};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = handle;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;
    IDXGISwapChain* swapchain;
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11RenderTargetView* back_buffer;
    ID3D11InputLayout* input_layout;
    ID3D11VertexShader* vertex_shader;
    ID3D11PixelShader* pixel_shader;
    ID3D11Buffer* vertex_buffer;
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        0,
        0,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &device,
        nullptr,
        &device_context);
    ID3D11Texture2D* back_buffer_texture;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_texture);
    device->CreateRenderTargetView(back_buffer_texture, nullptr, &back_buffer);
    back_buffer_texture->Release();
    device_context->OMSetRenderTargets(1, &back_buffer, nullptr);
    D3D11_VIEWPORT viewport = {0};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 800;
    device_context->RSSetViewports(1, &viewport);
    ID3DBlob* vs_blob;
    ID3DBlob* ps_blob;
    D3DCompileFromFile(L"shader.shader", 0, 0, "VShader", "vs_4_0", 0, 0, &vs_blob, 0);
    D3DCompileFromFile(L"shader.shader", 0, 0, "PShader", "ps_4_0", 0, 0, &ps_blob, 0);
    device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);
    device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);
    device_context->VSSetShader(vertex_shader, 0, 0);
    device_context->PSSetShader(pixel_shader, 0, 0);
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    device->CreateInputLayout(ied, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
    device_context->IASetInputLayout(input_layout);
    Vertex vertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
        {-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
    };
    D3D11_BUFFER_DESC bd = {0};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&bd, nullptr, &vertex_buffer);
    D3D11_MAPPED_SUBRESOURCE ms;
    device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, vertices, sizeof(vertices));
    device_context->Unmap(vertex_buffer, 0);
    ID3D11Buffer* constant_buffer;
    ConstantBuffer constant_buffer_data;
    Matrix4x4 projection_matrix = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,1,
        0,0,0,0
    };
    Matrix4x4 model_matrix = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,3,1
    };
    constant_buffer_data.model_view_projection = mul(model_matrix, projection_matrix);
    D3D11_BUFFER_DESC cbd;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    cbd.StructureByteStride = 0;
    device->CreateBuffer(&cbd, nullptr, &constant_buffer);
    D3D11_MAPPED_SUBRESOURCE ms_constant_buffer;
    device_context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms_constant_buffer);
    memcpy(ms_constant_buffer.pData, &constant_buffer_data, sizeof(constant_buffer_data));
    MSG message;
    float i = 0.0f;
    while(run)
    {
        i = i + 0.0001f;
        while(PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        Color c = {0.2f, 0.0f, 0.0f, 1.0f};
        device_context->ClearRenderTargetView(back_buffer, &c.r);
        unsigned stride = sizeof(Vertex);
        unsigned offset = 0;
        model_matrix.w.z = (sinf(i) + 2.0f) / 3.0f;
        model_matrix.w.x = cosf(i) / 3.0f;
        model_matrix.x.y = cosf(i);
        constant_buffer_data.model_view_projection = mul(model_matrix, projection_matrix);
        memcpy(ms_constant_buffer.pData, &constant_buffer_data, sizeof(constant_buffer_data));
        device_context->VSSetConstantBuffers(0, 1, &constant_buffer);
        device_context->PSSetConstantBuffers(0, 1, &constant_buffer);
        device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
        device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        device_context->Draw(3, 0);
        swapchain->Present(0, 0);
    }
    swapchain->Release();
    input_layout->Release();
    vertex_shader->Release();
    pixel_shader->Release();
    vertex_buffer->Release();
    swapchain->Release();
    back_buffer->Release();
    device->Release();
    device_context->Release();
    return 0;
}

LRESULT window_proc(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message)
    {
    case WM_QUIT:
    case WM_CLOSE:
        run = false;
        return 0;
    }

    return DefWindowProc(window_handle, message, wparam, lparam);
}